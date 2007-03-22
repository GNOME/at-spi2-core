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

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <libbonobo.h>
#include <orbit/orbit.h>
#include <atk/atk.h>
#include <atk/atkobject.h>
#include <atk/atknoopobject.h>
#include <libspi/Accessibility.h>
#include <libspi/spi-private.h>
#include "remoteobject.h"
#include "accessible.h"
#include "application.h"
#include <bonobo-activation/bonobo-activation-register.h>

#undef SPI_BRIDGE_DEBUG

#define DBG(a,b) if(_dbg>=(a))b

#define bridge_threads_leave() \
  if (!during_init_shutdown && !g_main_context_is_owner (NULL)) atk_misc_threads_leave(misc);
#define bridge_threads_enter() \
  if (!during_init_shutdown && !g_main_context_is_owner (NULL)) atk_misc_threads_enter(misc);

int _dbg = 0;
static CORBA_Environment ev;
static Accessibility_Registry registry = CORBA_OBJECT_NIL;
static Accessibility_DeviceEventController device_event_controller = CORBA_OBJECT_NIL;
static SpiApplication *this_app = NULL;
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

static Accessibility_Registry spi_atk_bridge_get_registry (void);
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
static void     spi_atk_bridge_register_application    (Accessibility_Registry registry);
static gboolean spi_atk_bridge_property_event_listener (GSignalInvocationHint *signal_hint,
							guint                  n_param_values,
							const GValue          *param_values,
							gpointer               data);

static void     spi_atk_bridge_init_nil                (CORBA_any *any, 
							AtkObject *obj);
static void     spi_atk_bridge_init_object             (CORBA_any *any, 
							AtkObject *obj,
							CORBA_Object *c_obj);
static void     spi_atk_bridge_init_string             (CORBA_any *any, 
							AtkObject *obj, 
							gchar **string);
static void     spi_atk_bridge_init_rect               (CORBA_any *any, 
							AtkObject *obj, 
							AtkRectangle *rect);

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
static void     deregister_application                 (BonoboObject          *app);
static void     reinit_register_vars                   (void);

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
post_init (void)
{
  during_init_shutdown = FALSE;
  return FALSE;
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

  if (!bonobo_init (argc, argv ? *argv : NULL))
    {
      g_error ("Could not initialize Bonobo");
    }

  /* Create the accessible application server object */
  this_app = spi_application_new (atk_get_root ());
  /*
   * We only want to enable the bridge for top level
   * applications, we detect bonobo components by seeing
   * if they were activated with the intention of extracting
   * an impl. by IID - very solid.
   */
  if (bonobo_activation_iid_get ())
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
      success = (spi_atk_bridge_get_registry () != CORBA_OBJECT_NIL);
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
  CORBA_Environment ev;

  CORBA_exception_init(&ev);

  if (spi_atk_bridge_get_registry () == CORBA_OBJECT_NIL)
    {
      g_warning ("Could not locate registry");
      return FALSE;
    }

  bonobo_activate ();

  /* Create the accessible application server object */
  if (this_app == NULL)
    this_app = spi_application_new (atk_get_root ());

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
  BonoboObject *app = (BonoboObject *) this_app;

  toplevels--;
  if (toplevels == 0)
    {
      deregister_application (app);
      reinit_register_vars ();
    }
  if (toplevels < 0)
    {
      g_warning ("More toplevels removed than added\n");
      toplevels = 0;
    }
}

static void
spi_atk_bridge_register_application (Accessibility_Registry registry)
{
  bridge_threads_leave ();
  Accessibility_Registry_registerApplication (spi_atk_bridge_get_registry (),
                                              BONOBO_OBJREF (this_app),
                                              &ev);
  bridge_threads_enter ();
  if (ev._major != CORBA_NO_EXCEPTION)
    CORBA_exception_free (&ev);
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


static Accessibility_Registry
spi_atk_bridge_get_registry (void)
{
  CORBA_Environment ev;
  char *ior =  NULL;

  if (registry_died || (registry == CORBA_OBJECT_NIL)) {
	  CORBA_exception_init (&ev);
	  if (registry_died) 
            {
              if (exiting)
                return CORBA_OBJECT_NIL;
              else
	        DBG (1, g_warning ("registry died! restarting..."));
            }

	  /* XXX: This presumes that the registry has successfully restarted itself already...*/
	  ior = (char *) spi_atk_bridge_get_registry_ior ();

	  if (ior != NULL)
	       registry = CORBA_ORB_string_to_object (bonobo_activation_orb_get (), 
						      ior, &ev);
	  else {
	       g_warning ("IOR not set.");  
	       registry = CORBA_OBJECT_NIL;
	  }
	  
	  if (ev._major != CORBA_NO_EXCEPTION)
	  {
		  g_error ("Accessibility app error: exception during "
			   "registry activation from id: %s\n",
			   CORBA_exception_id (&ev));
		  CORBA_exception_free (&ev);
	  }
	  
	  if (registry_died && registry) {
		  registry_died = FALSE;
		  spi_atk_bridge_register_application (registry);
	  }
  }
  return registry;
}

static Accessibility_DeviceEventController
spi_atk_bridget_get_dec (void)
{
  CORBA_Environment ev;

  if (device_event_controller != CORBA_OBJECT_NIL)
    {
      if (ORBit_small_get_connection_status (device_event_controller)
	  == ORBIT_CONNECTION_CONNECTED)
        return device_event_controller;
    }

  CORBA_exception_init (&ev);

  device_event_controller =
    Accessibility_Registry_getDeviceEventController (
	    spi_atk_bridge_get_registry (), &ev);

  if (BONOBO_EX (&ev))
    {
      g_warning (_("failure: no device event controller found.\n"));
      registry_died = TRUE;
      device_event_controller = CORBA_OBJECT_NIL;
    }

  return device_event_controller;
}

int
gtk_module_init (gint *argc, gchar **argv[])
{
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


  if (atk_listeners_registered) return;

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
deregister_application (BonoboObject *app)
{
  Accessibility_Registry registry = spi_atk_bridge_get_registry ();
  bridge_threads_leave ();
  Accessibility_Registry_deregisterApplication (registry, BONOBO_OBJREF (app), &ev);
  bridge_threads_enter ();

  device_event_controller = bonobo_object_release_unref (device_event_controller, &ev);
  registry = bonobo_object_release_unref (registry, &ev);
  
  app = bonobo_object_unref (app);
}

static void
spi_atk_bridge_exit_func (void)
{
  BonoboObject *app = (BonoboObject *) this_app;

  DBG (1, g_message ("exiting bridge\n"));

  if (!app)
    {
      return;
    }
  this_app = NULL;
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
  if (!bonobo_is_initialized ())
    {
      DBG (1, g_warning ("Re-initializing bonobo\n"));
      g_assert (bonobo_init (0, NULL));
      g_assert (bonobo_activate ());
    }
  
  if (!registry_died)
    deregister_application (app);

  DBG (1, g_message ("bridge exit func complete.\n"));

  if (g_getenv ("AT_BRIDGE_SHUTDOWN"))
    {
      g_assert (!bonobo_debug_shutdown ());
    }
  if (bridge_display)
    XCloseDisplay (bridge_display);
}

void
gnome_accessibility_module_init (void)
{
  atk_bridge_init (NULL, NULL);

  if (g_getenv ("AT_BRIDGE_SHUTDOWN"))
    {
	g_print("Atk Accessibility bridge initialized\n");
    }
}

void
gnome_accessibility_module_shutdown (void)
{
  BonoboObject *app = (BonoboObject *) this_app;
  int     i;
  GArray *ids = listener_ids;
  
  if (!atk_bridge_initialized)
    {
      return;
    }
  during_init_shutdown = TRUE;
  atk_bridge_initialized = FALSE;
  this_app = NULL;

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

  deregister_application (app);

  misc = NULL;
}

static void
spi_atk_bridge_focus_tracker (AtkObject *object)
{
  SpiAccessible *source;
  Accessibility_Event e;
  
  source = spi_accessible_new (object);
  
  CORBA_exception_init (&ev);
  
  e.type = "focus:";
  e.source = BONOBO_OBJREF (source);
  e.detail1 = 0;
  e.detail2 = 0;
  spi_atk_bridge_init_nil (&e.any_data, object);
  if (BONOBO_EX (&ev))
      registry_died = TRUE;
  else {
      bridge_threads_leave ();   
      Accessibility_Registry_notifyEvent (spi_atk_bridge_get_registry (), 
					  &e, &ev);
      bridge_threads_enter ();
  }
  if (BONOBO_EX (&ev))
    registry_died = TRUE;

  bonobo_object_unref (source);
  
  CORBA_exception_free (&ev);
}

static void
spi_atk_emit_eventv (const GObject         *gobject,
		     long                   detail1,
		     long                   detail2,
		     CORBA_any             *any,
		     const char            *format, ...)
{
  va_list             args;
  Accessibility_Event e;
  AtkObject          *aobject;
  SpiAccessible      *source = NULL;
  Accessibility_Registry registry;
  const gchar *name;
#ifdef SPI_BRIDGE_DEBUG
  CORBA_string s = NULL;
#endif
  
  va_start (args, format);
  
  if (ATK_IS_IMPLEMENTOR (gobject))
    {
      aobject = atk_implementor_ref_accessible (ATK_IMPLEMENTOR (gobject));
      source  = spi_accessible_new (aobject);
      g_object_unref (G_OBJECT (aobject));
    }
  else if (ATK_IS_OBJECT (gobject))
    {
      aobject = ATK_OBJECT (gobject);
      if (SPI_IS_REMOTE_OBJECT (aobject))
         e.source = spi_remote_object_get_accessible (SPI_REMOTE_OBJECT (aobject));
      else
         source  = spi_accessible_new (aobject);
    }
  else
    {
      aobject = NULL;
      DBG (0, g_warning ("received property-change event from non-AtkImplementor"));
      va_end (args);
      return;
    }
  name = atk_object_get_name (aobject);
  e.type = g_strdup_vprintf (format, args);
  if (source) e.source = BONOBO_OBJREF (source);
  e.detail1 = detail1;
  e.detail2 = detail2;
  if (any) e.any_data = *any;
  else spi_atk_bridge_init_nil (&e.any_data, aobject);

#ifdef SPI_BRIDGE_DEBUG
  if (e.source != CORBA_OBJECT_NIL)
      s = Accessibility_Accessible__get_name (e.source, &ev);
  g_message ("Emitting event '%s' (%lu, %lu) on %s",
	     e.type, e.detail1, e.detail2, s);
  CORBA_free (s);
#endif
  CORBA_exception_init (&ev);
  registry = spi_atk_bridge_get_registry ();
  if (!registry_died)
  {
    bridge_threads_leave (); 
    Accessibility_Registry_notifyEvent (registry, 
                                        &e, &ev);
    bridge_threads_enter ();
#ifdef SPI_BRIDGE_DEBUG
    if (ev._major != CORBA_NO_EXCEPTION)
        g_message ("error emitting event %s, (%d) %s",
                   e.type,
                   ev._major,
                   CORBA_exception_id(&ev));
#endif	      
    if (BONOBO_EX (&ev)) registry_died = TRUE;
  }

  if (source)
      bonobo_object_unref (BONOBO_OBJECT (source));
  else
      Bonobo_Unknown_unref (e.source, &ev);
  
  CORBA_exception_free (&ev);
  
  g_free (e.type);

  if (!any && e.any_data._release) CORBA_free (e.any_data._value);

  if (any && any->_release) CORBA_free (any->_value);
  
  va_end (args);

}

static gboolean
spi_atk_bridge_property_event_listener (GSignalInvocationHint *signal_hint,
					guint n_param_values,
					const GValue *param_values,
					gpointer data)
{
  AtkPropertyValues *values;
  GObject *gobject;
  const gchar *prop_name;
  CORBA_any any;
  const gchar *sp = NULL;
  AtkObject *ao;
  SpiAccessible *s_ao = NULL;
  CORBA_Object c_obj;
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

  gobject = g_value_get_object (param_values + 0);
  name = atk_object_get_name (ATK_OBJECT (gobject));
  values = (AtkPropertyValues*) g_value_get_pointer (param_values + 1);

  prop_name = values->property_name;
  if (strcmp (prop_name, "accessible-name") == 0)
    {
      spi_atk_bridge_init_string (&any, 
				  ATK_OBJECT (gobject),
				  (gchar **)&name);
    }
  else if (strcmp (prop_name, "accessible-description") == 0)
    {
      sp = atk_object_get_description (ATK_OBJECT (gobject));
      spi_atk_bridge_init_string (&any, 
				  ATK_OBJECT (gobject),
				  (gchar **)&sp);
    }
  else if (strcmp (prop_name, "accessible-parent") == 0)
    {
      ao = atk_object_get_parent (ATK_OBJECT (gobject));
      if (ao) 
        {
          s_ao = spi_accessible_new (ao);
          c_obj = BONOBO_OBJREF (s_ao);
          spi_atk_bridge_init_object (&any, 
				      ATK_OBJECT (gobject),
				      &c_obj);
	}
      else
        {
          spi_atk_bridge_init_nil (&any,
				   ATK_OBJECT (gobject));
        }
    }
  else if (strcmp (prop_name, "accessible-table-summary") == 0)
    {
      ao = atk_table_get_summary (ATK_TABLE (gobject));
      if (ao) 
        {
          s_ao = spi_accessible_new (ao);
          c_obj = BONOBO_OBJREF (s_ao);
          spi_atk_bridge_init_object (&any, 
				      ATK_OBJECT (gobject),
				      &c_obj);
	}
      else
        {
	  spi_atk_bridge_init_nil (&any,
				   ATK_OBJECT (gobject));
        }
    }
  else if (strcmp (prop_name, "accessible-table-column-header") == 0)
    {
      i = g_value_get_int (&(values->new_value));
      ao = atk_table_get_column_header (ATK_TABLE (gobject), i);
      if (ao) 
        {
          s_ao = spi_accessible_new (ao);
          c_obj = BONOBO_OBJREF (s_ao);
          spi_atk_bridge_init_object (&any, 
				      ATK_OBJECT (gobject),
				      &c_obj);
	}
      else
        {
          spi_atk_bridge_init_nil (&any, ATK_OBJECT (gobject));
        }
    }
  else if (strcmp (prop_name, "accessible-table-row-header") == 0)
    {
      i = g_value_get_int (&(values->new_value));
      ao = atk_table_get_row_header (ATK_TABLE (gobject), i);
      if (ao) 
        {
          s_ao = spi_accessible_new (ao);
          c_obj = BONOBO_OBJREF (s_ao);
          spi_atk_bridge_init_object (&any, ATK_OBJECT (gobject), &c_obj);
	}
      else
        {
          spi_atk_bridge_init_nil (&any, ATK_OBJECT (gobject));
        }
    }
  else if (strcmp (prop_name, "accessible-table-row-description") == 0)
    {
      i = g_value_get_int (&(values->new_value));
      sp = atk_table_get_row_description (ATK_TABLE (gobject), i);
      spi_atk_bridge_init_string (&any, ATK_OBJECT (gobject), 
				  (gchar **)&sp);
    }
  else if (strcmp (prop_name, "accessible-table-column-description") == 0)
    {
      i = g_value_get_int (&(values->new_value));
      sp = atk_table_get_column_description (ATK_TABLE (gobject), i);
      spi_atk_bridge_init_string (&any, ATK_OBJECT (gobject), 
				  (gchar **)&sp);
    }
  else if (strcmp (prop_name, "accessible-table-caption-object") == 0)
    {
      ao = atk_table_get_caption (ATK_TABLE (gobject));
      sp = atk_object_get_name (ao);
      spi_atk_bridge_init_string (&any, ATK_OBJECT (gobject), 
				  (gchar **)&sp);
    }
  else
    {
      spi_atk_bridge_init_nil (&any, ATK_OBJECT (gobject));
    }

  spi_atk_emit_eventv (gobject, 0, 0, &any,
		       "object:property-change:%s", prop_name);

  if (s_ao) 
    bonobo_object_unref (BONOBO_OBJECT (s_ao));

  return TRUE;
}

static gboolean
spi_atk_bridge_state_event_listener (GSignalInvocationHint *signal_hint,
				     guint n_param_values,
				     const GValue *param_values,
				     gpointer data)
{
  GObject *gobject;
  gchar *property_name;
  gchar *type;
  unsigned long detail1;
#ifdef SPI_BRIDGE_DEBUG
  GSignalQuery signal_query;
  const gchar *name;
  
  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;
  fprintf (stderr, "Received (state) signal %s:%s\n",
	   g_type_name (signal_query.itype), name);
#endif

  gobject = g_value_get_object (param_values + 0);
  property_name = g_strdup (g_value_get_string (param_values + 1));
  detail1 = (g_value_get_boolean (param_values + 2))
    ? 1 : 0;
  type = g_strdup_printf ("object:state-changed:%s", property_name);
  spi_atk_emit_eventv (gobject, 
		       detail1,
		       0,
		       NULL,
		       type);
  g_free (property_name);
  g_free (type);
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
  
  keystroke->id        = (CORBA_long) event->keyval;
  keystroke->hw_code   = (CORBA_short) event->keycode;
  keystroke->timestamp = (CORBA_unsigned_long) event->timestamp;
  keystroke->modifiers = (CORBA_unsigned_short) (event->state & 0xFFFF);
  if (event->string)
    {
      gunichar c;

      keystroke->event_string = CORBA_string_dup (event->string);
      c = g_utf8_get_char_validated (event->string, -1);
      if (c > 0 && g_unichar_isprint (c))
        keystroke->is_text = CORBA_TRUE;
      else
        keystroke->is_text = CORBA_FALSE;
    }
  else
    {
      keystroke->event_string = CORBA_string_dup ("");
      keystroke->is_text = CORBA_FALSE;
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

static gint
spi_atk_bridge_key_listener (AtkKeyEventStruct *event, gpointer data)
{
  CORBA_boolean             result;
  Accessibility_DeviceEvent key_event;

  CORBA_exception_init (&ev);

  spi_init_keystroke_from_atk_key_event (&key_event, event);

  result = Accessibility_DeviceEventController_notifyListenersSync (
	  spi_atk_bridget_get_dec (), &key_event, &ev);

  if (key_event.event_string) CORBA_free (key_event.event_string);

  if (BONOBO_EX(&ev)) {
      result = FALSE;
      CORBA_exception_free (&ev);
  }

  return result;
}

static gboolean
spi_atk_bridge_signal_listener (GSignalInvocationHint *signal_hint,
				guint n_param_values,
				const GValue *param_values,
				gpointer data)
{
  GObject *gobject;
  GSignalQuery signal_query;
  const gchar *name;
  const gchar *detail;
  CORBA_any any;
  CORBA_Object c_obj;
  char *sp = NULL;
  AtkObject *ao;
  gint detail1 = 0, detail2 = 0;
  SpiAccessible *s_ao = NULL;
#ifdef SPI_BRIDGE_DEBUG
  const gchar *s, *s2;
#endif 
  
  g_signal_query (signal_hint->signal_id, &signal_query);

  name = signal_query.signal_name;
  if (signal_hint->detail)
    detail = g_quark_to_string (signal_hint->detail);
  else
    detail = NULL;

#ifdef SPI_BRIDGE_DEBUG
  s2 = g_type_name (G_OBJECT_TYPE (g_value_get_object (param_values + 0)));
  s = atk_object_get_name (ATK_OBJECT (g_value_get_object (param_values + 0)));
  fprintf (stderr, "Received signal %s:%s detail: %s from object %s (gail %s)\n",
	   g_type_name (signal_query.itype), name, 
	   detail ? detail : "<NULL>", s ? s : "<NULL>" , s2);
#endif
  
  gobject = g_value_get_object (param_values + 0);

  if (signal_query.signal_id == atk_signal_active_descendant_changed)
    {
      gpointer child = g_value_get_pointer (param_values + 1);

      g_return_val_if_fail (ATK_IS_OBJECT (child), TRUE);

      ao = ATK_OBJECT (child);

      detail1 = atk_object_get_index_in_parent (ao);
      s_ao = spi_accessible_new (ao);
      c_obj = BONOBO_OBJREF (s_ao);
      spi_atk_bridge_init_object (&any, ATK_OBJECT (gobject), &c_obj);
    }
  else if (signal_query.signal_id == atk_signal_link_selected)
    {
      if (G_VALUE_TYPE (param_values + 1) == G_TYPE_INT)
        detail1 = g_value_get_int (param_values + 1);
      spi_atk_bridge_init_nil (&any, ATK_OBJECT (gobject));
    }
  else if (signal_query.signal_id == atk_signal_bounds_changed)
    {
      AtkRectangle *atk_rect = NULL;

      if (G_VALUE_HOLDS_BOXED (param_values + 1))
	  atk_rect = g_value_get_boxed (param_values + 1);
      spi_atk_bridge_init_rect (&any, ATK_OBJECT (gobject), atk_rect);
    }
  else if ((signal_query.signal_id == atk_signal_children_changed) && gobject)
    {
      detail1 = g_value_get_uint (param_values + 1);
      ao = atk_object_ref_accessible_child (ATK_OBJECT (gobject), 
			        	    detail1);
      if (ao) 
        {
          s_ao = spi_accessible_new (ao);
          c_obj = BONOBO_OBJREF (s_ao);
          spi_atk_bridge_init_object (&any, ATK_OBJECT (gobject), &c_obj);
	  g_object_unref (ao);
	}
      else
	{
	  spi_atk_bridge_init_nil (&any, ATK_OBJECT (gobject));
	}
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
          sp = atk_text_get_text (ATK_TEXT (gobject),
	    		          detail1,
			          detail1+detail2);
          spi_atk_bridge_init_string (&any, ATK_OBJECT (gobject), 
				      (gchar **) &sp);
        }
      else if (signal_query.signal_id == atk_signal_text_selection_changed)
        {
          /* Return NULL as the selected string */
	  spi_atk_bridge_init_nil (&any, ATK_OBJECT (gobject));
        }
      else
        {
	  spi_atk_bridge_init_nil (&any, ATK_OBJECT (gobject));
        }
    }

  if (detail)
    spi_atk_emit_eventv (gobject, detail1, detail2, &any,
			 "object:%s:%s", name, detail);
  else
    spi_atk_emit_eventv (gobject, detail1, detail2, &any,
			 "object:%s", name);

  if (sp) 
    g_free (sp);

  if (s_ao)
     bonobo_object_unref (BONOBO_OBJECT (s_ao));

  return TRUE;
}

static gboolean
spi_atk_bridge_window_event_listener (GSignalInvocationHint *signal_hint,
				      guint n_param_values,
				      const GValue *param_values,
				      gpointer data)
{
  GObject *gobject;
  GSignalQuery signal_query;
  CORBA_any any;
  const gchar *name, *s;
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
  
  gobject = g_value_get_object (param_values + 0);

  s = atk_object_get_name (ATK_OBJECT (gobject));
  spi_atk_bridge_init_string (&any, ATK_OBJECT (gobject), (gchar **) &s);
  
  spi_atk_emit_eventv (gobject, 0, 0, &any,
		       "window:%s", name);
  return TRUE;
}

static gboolean
spi_atk_bridge_document_event_listener (GSignalInvocationHint *signal_hint,
                                      guint n_param_values,
                                      const GValue *param_values,
                                      gpointer data)
{
  GObject *gobject;
  GSignalQuery signal_query;
  CORBA_any any;
  const gchar *name, *s;
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

  gobject = g_value_get_object (param_values + 0);

  s = atk_object_get_name (ATK_OBJECT (gobject));
  spi_atk_bridge_init_string (&any, ATK_OBJECT (gobject), (gchar **) &s);

  spi_atk_emit_eventv (gobject, 0, 0, &any,
                       "document:%s", name);
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
      CORBA_any any;
      const gchar *name;
     
      child = atk_object_ref_accessible_child (root, i);
      stateset = atk_object_ref_state_set (child);
      
      name = atk_object_get_name (child);
      if (atk_state_set_contains_state (stateset, ATK_STATE_ACTIVE))
        {
          spi_atk_bridge_init_string (&any, child, (gchar**) &name);
          spi_atk_emit_eventv (G_OBJECT (child), 0, 0, &any, "window:deactivate");
          if (registry_died)
            return;
        }
      g_object_unref (stateset);

      spi_atk_bridge_init_string (&any, child, (gchar**) &name);
      spi_atk_emit_eventv (G_OBJECT (child), 0, 0, &any, "window:destroy");
      g_object_unref (child);
    }
}

static void
reinit_register_vars (void)
{
  registry = CORBA_OBJECT_NIL;
  device_event_controller = CORBA_OBJECT_NIL;
  this_app = NULL;
}

static void
spi_atk_bridge_init_base (CORBA_any *any, AtkObject *obj, 
			  Accessibility_Application *app, Accessibility_Role *role,
			  CORBA_string *name)
{
    const gchar *s = atk_object_get_name (obj);
    *app = spi_accessible_new_return (atk_get_root (), FALSE, NULL);
    *role = spi_role_from_atk_role (atk_object_get_role (obj));
    *name = s ? s : ""; /* string gets dup-ed in util.c spi_init_any_* */
}

static void     
spi_atk_bridge_init_nil  (CORBA_any *any, AtkObject *obj)
{
    Accessibility_Application app = CORBA_OBJECT_NIL;
    Accessibility_Role role = Accessibility_ROLE_UNKNOWN;
    CORBA_string name;
    spi_atk_bridge_init_base (any, obj, &app, &role, &name);
    spi_init_any_nil (any, app, role, name);
}

static void     
spi_atk_bridge_init_object (CORBA_any *any, AtkObject *obj, CORBA_Object *c_obj)
{
    Accessibility_Application app = CORBA_OBJECT_NIL;
    Accessibility_Role role = Accessibility_ROLE_UNKNOWN;
    CORBA_string name;
    spi_atk_bridge_init_base (any, obj, &app, &role, &name);
    spi_init_any_object (any, app, role, name, c_obj);
}

static void     
spi_atk_bridge_init_string (CORBA_any *any, AtkObject *obj, gchar **string)
{
    Accessibility_Application app = CORBA_OBJECT_NIL;
    Accessibility_Role role = Accessibility_ROLE_UNKNOWN;
    CORBA_string name;
    spi_atk_bridge_init_base (any, obj, &app, &role, &name);
    spi_init_any_string (any, app, role, name, string);
}

static void     
spi_atk_bridge_init_rect (CORBA_any *any, AtkObject *obj, AtkRectangle *rect)
{
    Accessibility_Application app = CORBA_OBJECT_NIL;
    Accessibility_Role role = Accessibility_ROLE_UNKNOWN;
    CORBA_string name;
    spi_atk_bridge_init_base (any, obj, &app, &role, &name);
    spi_init_any_rect (any, app, role, name, rect);
}

