/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002, 2003 Sun Microsystems Inc.,
 * Copyright 2001, 2002, 2003 Ximian, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"
#include "dbus/dbus-glib-lowlevel.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <atk/atk.h>
#include <atk/atkobject.h>
#include <atk/atknoopobject.h>
#include "accessible.h"

#undef SPI_BRIDGE_DEBUG

#define DBG(a,b) if(_dbg>=(a))b

#define bridge_threads_leave() \
  if (!during_init_shutdown && !g_main_context_is_owner (NULL)) atk_misc_threads_leave(misc);
#define bridge_threads_enter() \
  if (!during_init_shutdown && !g_main_context_is_owner (NULL)) atk_misc_threads_enter(misc);

typedef struct _SpiAppData SpiAppData;
struct _SpiAppData
{
  AtkObject *root;
  DRouteData droute;
};

int _dbg = 0;
static const char *registry = NULL;
static char *device_event_controller = NULL;
static SpiAppData *this_app = NULL;
static gboolean registry_died = FALSE;
static gboolean atk_listeners_registered = FALSE;
static gint toplevels = 0;
static gboolean exiting = FALSE;
static AtkMisc *misc = NULL;
static gboolean during_init_shutdown = TRUE;

static guint atk_signal_text_changed;
static guint atk_signal_children_changed;
static guint atk_signal_active_descendant_changed;
static guint atk_signal_text_selection_changed;

/* NOT YET USED
   static guint atk_signal_row_reordered;
   static guint atk_signal_row_inserted;
   static guint atk_signal_row_deleted;
   static guint atk_signal_column_reordered;
   static guint atk_signal_column_inserted;
   static guint atk_signal_column_deleted;
*/

static guint atk_signal_link_selected;
static guint atk_signal_bounds_changed;

static const char *spi_atk_bridge_get_registry (void);
static gboolean spi_atk_bridge_do_registration         (void);
static void     spi_atk_bridge_toplevel_added          (AtkObject             *object,
                                                        guint                 index,
                                                        AtkObject             *child);
static void     spi_atk_bridge_toplevel_removed        (AtkObject             *object,
                                                        guint                 index,
                                                        AtkObject             *child);

static void     spi_atk_bridge_exit_func               (void);
static void     spi_atk_register_event_listeners       (void);
static void     spi_atk_bridge_focus_tracker           (AtkObject             *object);
static gchar   *spi_atk_bridge_get_registry_ior        (void);
static void     spi_atk_bridge_register_application    (const char *registry);
static gboolean spi_atk_bridge_property_event_listener (GSignalInvocationHint *signal_hint,
							guint                  n_param_values,
							const GValue          *param_values,
							gpointer               data);

static gboolean
spi_atk_bridge_window_event_listener (GSignalInvocationHint *signal_hint,
				guint n_param_values,
				const GValue *param_values,
				gpointer data);
static gboolean
spi_atk_bridge_document_event_listener (GSignalInvocationHint *signal_hint,
                                guint n_param_values,
                                const GValue *param_values,
                                gpointer data);
static gboolean
spi_atk_bridge_state_event_listener (GSignalInvocationHint *signal_hint,
				     guint n_param_values,
				     const GValue *param_values,
				     gpointer data);
static gboolean spi_atk_bridge_signal_listener         (GSignalInvocationHint *signal_hint,
							guint                  n_param_values,
							const GValue          *param_values,
							gpointer               data);
static gint     spi_atk_bridge_key_listener            (AtkKeyEventStruct     *event,
							gpointer               data);
static void     spi_atk_tidy_windows                   (void);
static void     deregister_application                 (SpiAppData *app);
static void reinit_register_vars (void);

/* For automatic libgnome init */
extern void gnome_accessibility_module_init     (void);
extern void gnome_accessibility_module_shutdown (void);

static int     atk_bridge_initialized = FALSE;
static pid_t   atk_bridge_pid = 0;
static guint   atk_bridge_focus_tracker_id = 0;
static guint   atk_bridge_key_event_listener_id = 0;
static GArray *listener_ids = NULL;

/*
 *   These exported symbols are hooked by gnome-program
 * to provide automatic module initialization and shutdown.
 */
extern void gnome_accessibility_module_init     (void);
extern void gnome_accessibility_module_shutdown (void);

static void
spi_atk_bridge_init_event_type_consts ()
{
  static gboolean done = FALSE;

  if (done)
    return;

  atk_signal_children_changed = g_signal_lookup ("children_changed", 
					      ATK_TYPE_OBJECT);
  atk_signal_text_changed = g_signal_lookup ("text_changed", 
					     ATK_TYPE_TEXT);
  atk_signal_bounds_changed = g_signal_lookup ("bounds_changed", 
					      ATK_TYPE_COMPONENT);
  atk_signal_active_descendant_changed = 
         g_signal_lookup ("active_descendant_changed", 
		          ATK_TYPE_OBJECT); 
  atk_signal_link_selected = g_signal_lookup ("link_selected", 
					      ATK_TYPE_HYPERTEXT);
  atk_signal_text_selection_changed = g_signal_lookup ("text_selection_changed", 
					      ATK_TYPE_TEXT);
  done = TRUE;
}

static gboolean
post_init (gpointer data)
{
  during_init_shutdown = FALSE;
  return FALSE;
}

static DBusObjectPathVTable droute_vtable =
{
  NULL,
  &droute_message,
  NULL, NULL, NULL, NULL
};

static gchar* atspi_dbus_name;
static gboolean atspi_no_register; 

static GOptionEntry atspi_option_entries[] = 
{
  {"atspi-dbus-name", 0, 0, G_OPTION_ARG_STRING, &atspi_dbus_name, "D-Bus bus name to register as", NULL},
  {"atspi-no-register", 0, 0, G_OPTION_ARG_NONE, &atspi_no_register, "Do not register with Registry Daemon", NULL},
  {NULL}
};

static SpiAppData *
spi_app_init (AtkObject *root, gint *argc, gchar **argv[])
{
  GOptionContext *opt;
  SpiAppData *ad = g_new0(SpiAppData, 1);
  GError *err = NULL;
  DBusError error;
  int i;

  opt = g_option_context_new(NULL);
  g_option_context_add_main_entries(opt, atspi_option_entries, NULL);
  g_option_context_set_ignore_unknown_options(opt, TRUE);
  if (!g_option_context_parse(opt, argc, argv, &err))
      g_warning("Option parsing failed: %s\n", err->message);

  dbus_error_init(&error);
  ad->root = root;
  ad->droute.bus = dbus_bus_get(DBUS_BUS_SESSION, &error);

  if (!ad->droute.bus)
  {
    g_warning("Couldn't connect to dbus: %s\n", error.message);
    free(ad);
    return NULL;
  }
  if (atspi_dbus_name != NULL && dbus_bus_request_name(ad->droute.bus, 
			    			       atspi_dbus_name,
						       0,
						       &error))
  {
    g_print("\nRecieved D-Bus name - %s\n", atspi_dbus_name);
  }
  spi_register_tree_object(ad->droute.bus, &ad->droute, "/org/freedesktop/atspi/tree");
  if (!dbus_connection_try_register_fallback (ad->droute.bus, 
					      "/org/freedesktop/atspi/accessible", 
					      &droute_vtable, 
					      &ad->droute, 
					      &error))
  {
    g_warning("Couldn't register droute.\n");
    free(ad);
    return NULL;
  }

  dbus_connection_setup_with_g_main(ad->droute.bus, g_main_context_default());

  spi_dbus_initialize (&ad->droute);
  return ad;
}

static int
atk_bridge_init (gint *argc, gchar **argv[])
{
  const char *debug_env_string = g_getenv ("AT_SPI_DEBUG");
  gchar *fname;
  gboolean success = FALSE;

  if (atk_bridge_initialized)
    {
      return 0;
    }
  atk_bridge_initialized = TRUE;
  atk_bridge_pid = getpid ();

  misc = atk_misc_get_instance();

  if (g_getenv ("ATK_BRIDGE_REDIRECT_LOG"))
    {
      fname = g_strconcat ("/tmp/", g_get_prgname (), ".at-spi-log", NULL);
      /* make sure we're not being redirected - security issue */
      if (!g_file_test (fname, G_FILE_TEST_IS_SYMLINK))
	  freopen (fname, "w", stderr);
      g_free (fname);
    }

  if (debug_env_string) 
      _dbg = (int) g_ascii_strtod (debug_env_string, NULL);

  /* Connect to dbus */
  this_app = spi_app_init (atk_get_root (), argc, argv);

  /*
   * We only want to enable the bridge for top level
   * applications, we detect bonobo components by seeing
   * if they were activated with the intention of extracting
   * an impl. by IID - very solid.
   */
#ifdef WITH_BONOBO
  // TODO: Figure out if this is still needed
  if (bonobo_activation_iid_get ())
#else
  if (0)
#endif
    {
      DBG (1, g_message ("Found Bonobo component\n"));
      g_signal_connect (atk_get_root (), 
                        "children-changed::add",
                        (GCallback) spi_atk_bridge_toplevel_added, 
                        NULL);
      g_signal_connect (atk_get_root (), 
                        "children-changed::remove",
                        (GCallback) spi_atk_bridge_toplevel_removed, 
                        NULL);
      /* in this case we redefine 'success' to mean 'registry is present' */
      success = (spi_atk_bridge_get_registry () != NULL);
    }
  else
    {
      success = spi_atk_bridge_do_registration ();
    }
  /*
   * we must emit events even if we are not registered as a
   * full-fledged app; See bugzilla #400709.
   */
  if (success) 
    {
      spi_atk_register_event_listeners ();
      spi_atk_bridge_init_event_type_consts ();
    }
  else
    {
      atk_bridge_initialized = FALSE;
    }
  g_idle_add (post_init, NULL);

  return 0;
}

static gboolean
spi_atk_bridge_do_registration (void)
{
  if (spi_atk_bridge_get_registry () == NULL)
    {
      g_warning ("Could not locate registry");
      return FALSE;
    }

  /* Create the accessible application server object */
  if (this_app == NULL)
    this_app = spi_app_init (atk_get_root (), 0, NULL);

  DBG (1, g_message ("About to register application\n"));

  spi_atk_bridge_register_application (spi_atk_bridge_get_registry ());
  
  g_atexit (spi_atk_bridge_exit_func);

  DBG (1, g_message ("Application registered & listening\n"));
  return TRUE;
}

static void
spi_atk_bridge_toplevel_added (AtkObject *object,
                               guint     index,
                               AtkObject *child)
{
  if (toplevels == 0)
    {
      spi_atk_bridge_do_registration ();
    }
  toplevels++;
}

static void
spi_atk_bridge_toplevel_removed (AtkObject *object,
                                 guint     index,
                                 AtkObject *child)
{
  toplevels--;
  if (toplevels == 0)
    {
      deregister_application (this_app);
      reinit_register_vars ();
    }
  if (toplevels < 0)
    {
      g_warning ("More toplevels removed than added\n");
      toplevels = 0;
    }
}

static void
spi_atk_bridge_register_application (const char *registry)
{
  DBusMessage *message, *reply;
  DBusError error;

  bridge_threads_leave ();
  message = dbus_message_new_method_call (SPI_DBUS_NAME_REGISTRY, SPI_DBUS_PATH_REGISTRY, SPI_DBUS_INTERFACE_REGISTRY, "registerApplication");
  dbus_error_init (&error);
  reply = dbus_connection_send_with_reply_and_block(this_app->droute.bus, message, 1000, &error);
  if (error.message) g_warning (error.message);
  if (reply) dbus_message_unref (reply);
  if (message) dbus_message_unref (message);
  bridge_threads_enter ();
}

/* 
 * Returns a 'canonicalized' value for DISPLAY,
 * with the screen number stripped off if present.
 */
static const gchar*
spi_display_name (void)
{
    static const char *canonical_display_name = NULL;
    if (!canonical_display_name)
      {
        const gchar *display_env = g_getenv ("AT_SPI_DISPLAY");
	if (!display_env)
	  {
	    display_env = g_getenv ("DISPLAY");
	    if (!display_env || !display_env[0]) 
		canonical_display_name = ":0";
	    else
	      {
		gchar *display_p, *screen_p;
		canonical_display_name = g_strdup (display_env);
		display_p = strrchr (canonical_display_name, ':');
		screen_p = strrchr (canonical_display_name, '.');
		if (screen_p && display_p && (screen_p > display_p))
		  {
		    *screen_p = '\0';
		  }
	      }
	  }
	else
	  {
	    canonical_display_name = display_env;
	  }
      }
    return canonical_display_name;
}

static     Display *bridge_display = NULL;

static gchar *
spi_atk_bridge_get_registry_ior (void)
{
     
     Atom AT_SPI_IOR;
     Atom actual_type;
     int actual_format;
     unsigned char *data = NULL;  
     unsigned long nitems;
     unsigned long leftover;
     if (!bridge_display) 
       bridge_display = XOpenDisplay (spi_display_name ());

     AT_SPI_IOR = XInternAtom (bridge_display, "AT_SPI_IOR", False); 
     XGetWindowProperty(bridge_display, 
			XDefaultRootWindow (bridge_display),
			AT_SPI_IOR, 0L, 
			(long)BUFSIZ, False, 
			(Atom) 31, &actual_type, &actual_format,
			&nitems, &leftover, &data);
     if (data == NULL)
	  g_warning (_("AT_SPI_REGISTRY was not started at session startup."));
     
     return (gchar *) data;
     
}


static const char *
spi_atk_bridge_get_registry (void)
{
  // TODO: check for registry dying, as the old code attempted to do
  return "org.freedesktop.atspi.registry";
}

static const char *
spi_atk_bridge_get_dec (void)
{
  return "/dec";
}

int
gtk_module_init (gint *argc, gchar **argv[])
{
	//printf("Pointer to argc %x %x\n", (short) ((argc && 0xffff0000) >> 16), (short) (argc && 0xffff));
	return atk_bridge_init (argc, argv);
}

static void
add_signal_listener (const char *signal_name)
{
  guint id;

  id = atk_add_global_event_listener (
    spi_atk_bridge_signal_listener, signal_name);

  g_array_append_val (listener_ids, id);
}

static void
spi_atk_register_event_listeners (void)
{
  /*
   * kludge to make sure the Atk interface types are registered, otherwise
   * the AtkText signal handlers below won't get registered
   */
  guint      id;
  GObject   *ao = g_object_new (ATK_TYPE_OBJECT, NULL);
  AtkObject *bo = atk_no_op_object_new (ao);


  if (atk_listeners_registered) 
    {
      g_object_unref (G_OBJECT (bo));
      g_object_unref (ao);
      return;
    }

  atk_listeners_registered = TRUE;

  /* Register for focus event notifications, and register app with central registry  */

  listener_ids = g_array_sized_new (FALSE, TRUE, sizeof (guint), 16);

  atk_bridge_focus_tracker_id = atk_add_focus_tracker (spi_atk_bridge_focus_tracker);

  id = atk_add_global_event_listener (spi_atk_bridge_property_event_listener,
				      "Gtk:AtkObject:property-change");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_window_event_listener,
				      "window:create");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_window_event_listener,
				      "window:destroy");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_window_event_listener,
				      "window:minimize");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_window_event_listener,
				      "window:maximize");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_window_event_listener,
				      "window:restore");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_window_event_listener,
				      "window:activate");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_window_event_listener,
				      "window:deactivate");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_document_event_listener,
				      "Gtk:AtkDocument:load-complete");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_document_event_listener,
				      "Gtk:AtkDocument:reload");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_document_event_listener,
				      "Gtk:AtkDocument:load-stopped");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_state_event_listener,
				      "Gtk:AtkObject:state-change");
  g_array_append_val (listener_ids, id);

  add_signal_listener ("Gtk:AtkObject:children-changed");
  add_signal_listener ("Gtk:AtkObject:visible-data-changed");
  add_signal_listener ("Gtk:AtkObject:active-descendant-changed");
  add_signal_listener ("Gtk:AtkComponent:bounds-changed");
  add_signal_listener ("Gtk:AtkSelection:selection-changed");
  add_signal_listener ("Gtk:AtkText:text-selection-changed");
  add_signal_listener ("Gtk:AtkText:text-changed");
  add_signal_listener ("Gtk:AtkText:text-caret-moved");
  add_signal_listener ("Gtk:AtkTable:row-inserted");
  add_signal_listener ("Gtk:AtkTable:row-reordered");
  add_signal_listener ("Gtk:AtkTable:row-deleted");
  add_signal_listener ("Gtk:AtkTable:column-inserted");
  add_signal_listener ("Gtk:AtkTable:column-reordered");
  add_signal_listener ("Gtk:AtkTable:column-deleted");
  add_signal_listener ("Gtk:AtkTable:model-changed");
  add_signal_listener ("Gtk:AtkHypertext:link-selected");
/*
 * May add the following listeners to implement preemptive key listening for GTK+
 *
 * atk_add_global_event_listener (spi_atk_bridge_widgetkey_listener, "Gtk:GtkWidget:key-press-event");
 * atk_add_global_event_listener (spi_atk_bridge_widgetkey_listener, "Gtk:GtkWidget:key-release-event");
 */
  atk_bridge_key_event_listener_id = atk_add_key_event_listener (
    spi_atk_bridge_key_listener, NULL);
  
  g_object_unref (G_OBJECT (bo));
  g_object_unref (ao);
}

static void
deregister_application (SpiAppData *app)
{
  const char *registry = spi_atk_bridge_get_registry ();
  bridge_threads_leave ();
  // todo: deregister
  bridge_threads_enter ();
}

static void
spi_atk_bridge_exit_func (void)
{
  SpiAppData *app = (SpiAppData *) this_app;

  DBG (1, g_message ("exiting bridge\n"));

  if (!app)
    {
      return;
    }
  if (atk_bridge_pid != getpid ())
    {
      _exit (0);
    }

  during_init_shutdown = TRUE;
  exiting = TRUE;
  /*
   * Check whether we still have windows which have not been deleted.
   */
  spi_atk_tidy_windows ();
  /*
   *  FIXME: this may be incorrect for apps that do their own bonobo
   *  shutdown, until we can explicitly shutdown to get the ordering
   *  right.
   */
  if (!registry_died)
    deregister_application (this_app);
  this_app = NULL;
  DBG (1, g_message ("bridge exit func complete.\n"));

  if (g_getenv ("AT_BRIDGE_SHUTDOWN"))
    {
    }
  if (bridge_display)
    XCloseDisplay (bridge_display);
}

void
gnome_accessibility_module_init (void)
{
  atk_bridge_init (0, NULL);

  if (g_getenv ("AT_BRIDGE_SHUTDOWN"))
    {
	g_print("Atk Accessibility bridge initialized\n");
    }
}

void
gnome_accessibility_module_shutdown (void)
{
  int     i;
  GArray *ids = listener_ids;
  
  if (!atk_bridge_initialized)
    {
      return;
    }
  during_init_shutdown = TRUE;
  atk_bridge_initialized = FALSE;

  if (g_getenv ("AT_BRIDGE_SHUTDOWN"))
    {
	g_print("Atk Accessibility bridge shutdown\n");
    }

  listener_ids = NULL;
  if (atk_bridge_focus_tracker_id)
        atk_remove_focus_tracker (atk_bridge_focus_tracker_id);
  
  for (i = 0; ids && i < ids->len; i++)
    {
          atk_remove_global_event_listener (g_array_index (ids, guint, i));
    }
  
  if (atk_bridge_key_event_listener_id)
          atk_remove_key_event_listener (atk_bridge_key_event_listener_id);

  deregister_application (this_app);
  this_app = NULL;

  misc = NULL;
}

static void emit(AtkObject *object, const char *name, const char *detail, dbus_int32_t detail1, dbus_int32_t detail2, int type, const void *val)
{
  DBusMessage *sig;
  char *path = spi_dbus_get_path(object);
  DBusMessageIter iter, sub;
  const char *type_as_string = NULL;
  dbus_int32_t dummy = 0;

  spi_dbus_update_cache(&this_app->droute);
  if (type == DBUS_TYPE_OBJECT_PATH)
  {
    type_as_string = "o";
    if (!val) val = "";
  }
  else if (type == DBUS_TYPE_STRING)
  {
    type_as_string = "s";
    if (!val) val = "";
  }
  else if (type == DBUS_TYPE_INT32) type_as_string = "i";
  else if (type == DBUS_TYPE_UINT32) type_as_string = "u";
  else if (type == DBUS_TYPE_INVALID)
  {
    type = DBUS_TYPE_UINT32;
    type_as_string = "u";
    if (!val) val = &dummy;
  }
  else
  {
    g_warning("Unknown type %d in property change signal", type);
  }
  sig = dbus_message_new_signal(path, SPI_DBUS_INTERFACE_ACCESSIBLE, name);
  dbus_message_iter_init_append(sig, &iter);
  if (!detail) detail = "";
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &detail);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &detail1);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &detail2);
  dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT, type_as_string, &sub);
  dbus_message_iter_append_basic(&sub, type, &val);
  dbus_message_iter_close_container(&iter, &sub);
  dbus_connection_send(this_app->droute.bus, sig, NULL);
  g_free(path);
  dbus_message_unref(sig);
}

static void
spi_atk_bridge_focus_tracker (AtkObject *object)
{
  emit(object, "focus", NULL, 0, 0, DBUS_TYPE_INVALID, NULL);
}

static void emit_rect(AtkObject *object, const char *name, const char *detail, AtkRectangle *rect)
{
  DBusMessage *sig;
  char *path = spi_dbus_get_path(object);
  DBusMessageIter iter, iter_variant, sub;
  dbus_uint32_t x, y, width, height;
  dbus_int32_t dummy = 0;

  spi_dbus_update_cache(&this_app->droute);
  x = rect->x;
  y = rect->y;
  width = rect->width;
  height = rect->height;
  sig = dbus_message_new_signal(path, SPI_DBUS_INTERFACE_ACCESSIBLE, name);
  if (!detail) detail = "";
  if (sig)
    {
      dbus_message_iter_init_append (sig, &iter);
      dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &detail);
      dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &dummy);
      dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &dummy);
      if (!dbus_message_iter_open_container (&iter, DBUS_TYPE_VARIANT, "(iiii)", &iter_variant))
	goto oom;
      if (!dbus_message_iter_open_container (&iter_variant, DBUS_TYPE_STRUCT, NULL, &sub))
	goto oom;
      dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &x);
      dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &y);
      dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &width);
      dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &height);
      if (!dbus_message_iter_close_container (&iter_variant, &sub))
	goto oom;
      if (!dbus_message_iter_close_container (&iter, &iter_variant))
	goto oom;
    }
  dbus_connection_send(this_app->droute.bus, sig, NULL);
oom:
  g_free(path);
  dbus_message_unref(sig);
}

static const char *PropertyChange = "object_property_change";

static gboolean
spi_atk_bridge_property_event_listener (GSignalInvocationHint *signal_hint,
					guint n_param_values,
					const GValue *param_values,
					gpointer data)
{
  AtkPropertyValues *values;
  AtkObject *obj;
  const gchar *prop_name;
  const gchar *sp = NULL;
  AtkObject *ao;
  char *s_ao = NULL;
  gint i;
  const gchar *name = NULL;

#ifdef SPI_BRIDGE_DEBUG
  GSignalQuery signal_query;
  const gchar *signame;
  const gchar *s, *s2;
  
  g_signal_query (signal_hint->signal_id, &signal_query);
  signame = signal_query.signal_name;

  s2 = g_type_name (G_OBJECT_TYPE (g_value_get_object (param_values + 0)));
  s = atk_object_get_name (ATK_OBJECT (g_value_get_object (param_values + 0)));
  values = (AtkPropertyValues*) g_value_get_pointer (param_values + 1);
  DBG (2, g_message ("Received (property) signal %s:%s:%s from object %s (gail %s)\n",
	   g_type_name (signal_query.itype), signame, values->property_name, s, s2));
  
#endif

  obj = g_value_get_object (param_values + 0);
  name = atk_object_get_name (obj);
  values = (AtkPropertyValues*) g_value_get_pointer (param_values + 1);

  prop_name = values->property_name;
  if (strcmp (prop_name, "accessible-name") == 0)
    {
      spi_dbus_notify_change(obj, FALSE, &this_app->droute);
    }
  else if (strcmp (prop_name, "accessible-description") == 0)
    {
      spi_dbus_notify_change(obj, FALSE, &this_app->droute);
    }
  else if (strcmp (prop_name, "accessible-parent") == 0)
    {
      spi_dbus_notify_change(obj, FALSE, &this_app->droute);
    }
  else if (strcmp (prop_name, "accessible-table-summary") == 0)
    {
      ao = atk_table_get_summary (ATK_TABLE (obj));
      s_ao = spi_dbus_get_path(ao);
      emit(obj, PropertyChange, prop_name, 0, 0, DBUS_TYPE_OBJECT_PATH, s_ao);
    }
  else if (strcmp (prop_name, "accessible-table-column-header") == 0)
    {
      i = g_value_get_int (&(values->new_value));
      ao = atk_table_get_column_header (ATK_TABLE (obj), i);
      s_ao = spi_dbus_get_path(ao);
      emit(obj, PropertyChange, prop_name, 0, 0, DBUS_TYPE_OBJECT_PATH, s_ao);
    }
  else if (strcmp (prop_name, "accessible-table-row-header") == 0)
    {
      i = g_value_get_int (&(values->new_value));
      ao = atk_table_get_row_header (ATK_TABLE (obj), i);
      s_ao = spi_dbus_get_path(ao);
      emit(obj, PropertyChange, prop_name, 0, 0, DBUS_TYPE_OBJECT_PATH, s_ao);
    }
  else if (strcmp (prop_name, "accessible-table-row-description") == 0)
    {
      i = g_value_get_int (&(values->new_value));
      sp = atk_table_get_row_description (ATK_TABLE (obj), i);
      emit(obj, PropertyChange, prop_name, 0, 0, DBUS_TYPE_STRING, sp);
    }
  else if (strcmp (prop_name, "accessible-table-column-description") == 0)
    {
      i = g_value_get_int (&(values->new_value));
      sp = atk_table_get_column_description (ATK_TABLE(obj), i);
      emit(obj, PropertyChange, prop_name, 0, 0, DBUS_TYPE_STRING, sp);
    }
  else if (strcmp (prop_name, "accessible-table-caption-object") == 0)
    {
      ao = atk_table_get_caption (ATK_TABLE(obj));
      sp = atk_object_get_name (ao);
      emit(obj, PropertyChange, prop_name, 0, 0, DBUS_TYPE_STRING, sp);
    }
  else
    {
      emit(obj, PropertyChange, prop_name, 0, 0, DBUS_TYPE_INVALID, NULL);
    }
  if (s_ao) g_free(s_ao);
  return TRUE;
}

static gboolean
spi_atk_bridge_state_event_listener (GSignalInvocationHint *signal_hint,
				     guint n_param_values,
				     const GValue *param_values,
				     gpointer data)
{
  AtkObject *obj;
  gchar *property_name;
  unsigned long detail1;
#ifdef SPI_BRIDGE_DEBUG
  GSignalQuery signal_query;
  const gchar *name;
  
  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;
  fprintf (stderr, "Received (state) signal %s:%s\n",
	   g_type_name (signal_query.itype), name);
#endif

  obj = ATK_OBJECT(g_value_get_object (param_values + 0));
  property_name = g_strdup (g_value_get_string (param_values + 1));
  /* Ignore defunct for now; we'll send a tree update to remove it when
     the object goes away */
  /* Also ignore state changes for objects not yet broadcast */
  if ((property_name && !strcmp(property_name, "defunct")) ||
      !spi_dbus_object_is_known(obj))
  {
    g_free(property_name);
    return TRUE;
  }
  detail1 = (g_value_get_boolean (param_values + 2))
    ? 1 : 0;
  emit(obj, "object_state_changed", property_name, detail1, 0, DBUS_TYPE_INVALID, NULL);
  g_free (property_name);
  return TRUE;
}

static void
spi_init_keystroke_from_atk_key_event (Accessibility_DeviceEvent  *keystroke,
				       AtkKeyEventStruct          *event)
{
#ifdef SPI_DEBUG
  if (event)
    {
      g_print ("event %c (%d)\n", (int) event->keyval, (int) event->keycode);
    }
  else
#endif
  if (!event)
    { /* this doesn't really need translating */
      g_print (_("WARNING: NULL key event reported."));
    }
  
  keystroke->id        = (dbus_int32_t) event->keyval;
  keystroke->hw_code   = (dbus_int16_t) event->keycode;
  keystroke->timestamp = (dbus_uint32_t) event->timestamp;
  keystroke->modifiers = (dbus_uint16_t) (event->state & 0xFFFF);
  if (event->string)
    {
      gunichar c;

      keystroke->event_string = g_strdup (event->string);
      c = g_utf8_get_char_validated (event->string, -1);
      if (c > 0 && g_unichar_isprint (c))
        keystroke->is_text = TRUE;
      else
        keystroke->is_text = FALSE;
    }
  else
    {
      keystroke->event_string = g_strdup ("");
      keystroke->is_text = FALSE;
    }
  switch (event->type)
    {
    case (ATK_KEY_EVENT_PRESS):
      keystroke->type = Accessibility_KEY_PRESSED_EVENT;
      break;
    case (ATK_KEY_EVENT_RELEASE):
      keystroke->type = Accessibility_KEY_RELEASED_EVENT;
      break;
    default:
      keystroke->type = 0;
      break;
    }
#if 0  
  g_print ("key_event type %d; val=%d code=%d modifiers=%x name=%s is_text=%d, time=%lx\n",
	   (int) keystroke->type, (int) keystroke->id, (int) keystroke->hw_code,
	   (int) keystroke->modifiers,
	   keystroke->event_string, (int) keystroke->is_text, (unsigned long) keystroke->timestamp);
#endif
}

static gboolean Accessibility_DeviceEventController_notifyListenersSync(const Accessibility_DeviceEvent *key_event)
{
  DBusMessage *message = dbus_message_new_method_call(SPI_DBUS_NAME_REGISTRY, SPI_DBUS_PATH_REGISTRY, SPI_DBUS_INTERFACE_DEC, "notifyListenersSync");
  DBusError error;
  dbus_bool_t consumed = FALSE;

  dbus_error_init(&error);
  if (spi_dbus_marshal_deviceEvent(message, key_event))
  {
    DBusMessage *reply = dbus_connection_send_with_reply_and_block(this_app->droute.bus, message, 1000, &error);
    if (reply)
    {
      DBusError error;
      dbus_error_init(&error);
      dbus_message_get_args(reply, &error, DBUS_TYPE_BOOLEAN, &consumed, DBUS_TYPE_INVALID);
      dbus_message_unref(reply);
    }
  }
  dbus_message_unref(message);
  return consumed;
}

static gint
spi_atk_bridge_key_listener (AtkKeyEventStruct *event, gpointer data)
{
  gboolean             result;
  Accessibility_DeviceEvent key_event;

  spi_init_keystroke_from_atk_key_event (&key_event, event);

  bridge_threads_leave ();
  result = Accessibility_DeviceEventController_notifyListenersSync (&key_event);
  bridge_threads_enter ();

  if (key_event.event_string) g_free (key_event.event_string);

  return result;
}

static gboolean
spi_atk_bridge_signal_listener (GSignalInvocationHint *signal_hint,
				guint n_param_values,
				const GValue *param_values,
				gpointer data)
{
  AtkObject *obj;
  GSignalQuery signal_query;
  const gchar *name;
  const gchar *detail = NULL;
  char *sp = NULL;
  AtkObject *ao;
  gint detail1 = 0, detail2 = 0;
  char *s_ao = NULL;
  gchar *sig_name;
  char *p;
#ifdef SPI_BRIDGE_DEBUG
  const gchar *s, *s2;
#endif 
  
  g_signal_query (signal_hint->signal_id, &signal_query);

  name = signal_query.signal_name;
  if (signal_hint->detail)
  {
    detail = g_quark_to_string (signal_hint->detail);
  }
  sig_name = g_strdup_printf("object_%s", name);
  while ((p = strchr(sig_name, '-')) != NULL) *p = '_';

#ifdef SPI_BRIDGE_DEBUG
  s2 = g_type_name (G_OBJECT_TYPE (g_value_get_object (param_values + 0)));
  s = atk_object_get_name (ATK_OBJECT (g_value_get_object (param_values + 0)));
  fprintf (stderr, "Received signal %s:%s detail: %s from object %s (gail %s)\n",
	   g_type_name (signal_query.itype), name, 
	   detail ? detail : "<NULL>", s ? s : "<NULL>" , s2);
#endif
  
  obj = ATK_OBJECT(g_value_get_object (param_values + 0));

  if (signal_query.signal_id == atk_signal_active_descendant_changed)
    {
      gpointer child = g_value_get_pointer (param_values + 1);

      g_return_val_if_fail (ATK_IS_OBJECT (child), TRUE);

      ao = ATK_OBJECT (child);

      detail1 = atk_object_get_index_in_parent (ao);
      s_ao = spi_dbus_get_path(child);
      emit(obj, sig_name, detail, detail1, 0, DBUS_TYPE_OBJECT_PATH, s_ao);
      g_free(s_ao);
    }
  else if (signal_query.signal_id == atk_signal_link_selected)
    {
      if (G_VALUE_TYPE (param_values + 1) == G_TYPE_INT)
        detail1 = g_value_get_int (param_values + 1);
      emit(obj, sig_name, detail, detail1, 0, DBUS_TYPE_INVALID, NULL);
    }
  else if (signal_query.signal_id == atk_signal_bounds_changed)
    {
      AtkRectangle *atk_rect = NULL;

      if (G_VALUE_HOLDS_BOXED (param_values + 1))
	  atk_rect = g_value_get_boxed (param_values + 1);
      emit_rect(obj, sig_name, detail, atk_rect);
    }
  else if ((signal_query.signal_id == atk_signal_children_changed) && obj)
    {
      spi_dbus_notify_change(obj, FALSE, &this_app->droute);
    }
  else
    {
      if (n_param_values >= 2)
        {
          if (G_VALUE_TYPE (param_values + 1) == G_TYPE_INT)
            detail1 = g_value_get_int (param_values + 1);
          if (n_param_values >= 3)
            {
              if (G_VALUE_TYPE (param_values + 2) == G_TYPE_INT)
                detail2 = g_value_get_int (param_values + 2);
            }
        }

      if (signal_query.signal_id == atk_signal_text_changed)
        {
          sp = atk_text_get_text (ATK_TEXT (obj),
	    		          detail1,
			          detail1+detail2);
          emit(obj, sig_name, detail, detail1, detail2, DBUS_TYPE_STRING, sp);
        }
      else if (signal_query.signal_id == atk_signal_text_selection_changed)
        {
          /* Return NULL as the selected string */
          emit(obj, sig_name, detail, detail1, detail2, DBUS_TYPE_STRING, "");
        }
      else
        {
          emit(obj, sig_name, detail, 0, 0, DBUS_TYPE_INVALID, NULL);
        }
    }

  if (sp) 
    g_free (sp);

  if (s_ao)
     g_free (s_ao);
  g_free(sig_name);

  return TRUE;
}

static gboolean
spi_atk_bridge_window_event_listener (GSignalInvocationHint *signal_hint,
				      guint n_param_values,
				      const GValue *param_values,
				      gpointer data)
{
  AtkObject *obj;
  GSignalQuery signal_query;
  const gchar *name, *s;
#ifdef SPI_BRIDGE_DEBUG
  const gchar *s2;
#endif
  
  g_signal_query (signal_hint->signal_id, &signal_query);

  name = signal_query.signal_name;
  gchar *sig_name;

#ifdef SPI_BRIDGE_DEBUG
  s2 = g_type_name (G_OBJECT_TYPE (g_value_get_object (param_values + 0)));
  s = atk_object_get_name (ATK_OBJECT (g_value_get_object (param_values + 0)));
  fprintf (stderr, "Received signal %s:%s from object %s (gail %s)\n",
	   g_type_name (signal_query.itype), name, s ? s : "<NULL>" , s2);
#endif
  
  obj = ATK_OBJECT(g_value_get_object (param_values + 0));

  s = atk_object_get_name (obj);
  sig_name = g_strdup_printf("window_%s", name);
  emit(obj, sig_name, NULL, 0, 0, DBUS_TYPE_STRING, s);
  g_free(sig_name);

  return TRUE;
}

static gboolean
spi_atk_bridge_document_event_listener (GSignalInvocationHint *signal_hint,
                                      guint n_param_values,
                                      const GValue *param_values,
                                      gpointer data)
{
  AtkObject *obj;
  GSignalQuery signal_query;
  const gchar *name, *s;
  gchar *sig_name;
#ifdef SPI_BRIDGE_DEBUG
  const gchar *s2;
#endif

  g_signal_query (signal_hint->signal_id, &signal_query);

  name = signal_query.signal_name;

#ifdef SPI_BRIDGE_DEBUG
  s2 = g_type_name (G_OBJECT_TYPE (g_value_get_object (param_values + 0)));
  s = atk_object_get_name (ATK_OBJECT (g_value_get_object (param_values + 0)));
  fprintf (stderr, "Received signal %s:%s from object %s (gail %s)\n",
           g_type_name (signal_query.itype), name, s ? s : "<NULL>" , s2);
#endif

  obj = ATK_OBJECT(g_value_get_object (param_values + 0));

  s = atk_object_get_name (obj);
  sig_name = g_strdup_printf("document_%s", name);
  emit(obj, sig_name, NULL, 0, 0, DBUS_TYPE_STRING, s);
  g_free(sig_name);
  return TRUE;
}

static void
spi_atk_tidy_windows (void)
{
  AtkObject *root;
  gint n_children;
  gint i;

  root = atk_get_root ();
  n_children = atk_object_get_n_accessible_children (root);
  for (i = 0; i < n_children; i++)
    {
      AtkObject *child;
      AtkStateSet *stateset;
      const gchar *name;
     
      child = atk_object_ref_accessible_child (root, i);
      stateset = atk_object_ref_state_set (child);
      
      name = atk_object_get_name (child);
      if (atk_state_set_contains_state (stateset, ATK_STATE_ACTIVE))
        {
	  emit(child, "window:deactivate", NULL, 0, 0, DBUS_TYPE_STRING, name);
        }
      g_object_unref (stateset);

      emit(child, "window:destroy", NULL, 0, 0, DBUS_TYPE_STRING, name);
      g_object_unref (child);
    }
}

static void
reinit_register_vars (void)
{
  registry = NULL;
  device_event_controller = NULL;
  this_app = NULL;
}
