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
#include "bridge.h"
#include "atk-dbus.h"

#undef SPI_BRIDGE_DEBUG

#define DBG(a,b) if(_dbg>=(a))b

int _dbg = 0;
static const char *registry = NULL;
SpiAppData *this_app = NULL;
static gboolean registry_died = FALSE;
static gint toplevels = 0;
static gboolean exiting = FALSE;
static AtkMisc *misc = NULL;
static gboolean during_init_shutdown = TRUE;

static const char *spi_atk_bridge_get_registry (void);
static char *device_event_controller = NULL;
static void     spi_atk_bridge_register_application    (const char *registry);
static gchar   *spi_atk_bridge_get_registry_ior        (void);
static gboolean spi_atk_bridge_do_registration         (void);
static void     spi_atk_bridge_toplevel_added          (AtkObject             *object,
                                                        guint                 index,
                                                        AtkObject             *child);
static void     spi_atk_bridge_toplevel_removed        (AtkObject             *object,
                                                        guint                 index,
                                                        AtkObject             *child);

static void     spi_atk_bridge_exit_func               (void);
static void     spi_atk_tidy_windows                   (void);
static void     deregister_application                 (SpiAppData *app);
static void reinit_register_vars (void);

/* For automatic libgnome init */
extern void gnome_accessibility_module_init     (void);
extern void gnome_accessibility_module_shutdown (void);

static int     atk_bridge_initialized = FALSE;
static pid_t   atk_bridge_pid = 0;

/*
 *   These exported symbols are hooked by gnome-program
 * to provide automatic module initialization and shutdown.
 */
extern void gnome_accessibility_module_init     (void);
extern void gnome_accessibility_module_shutdown (void);

void
spi_atk_register_event_listeners(void);

void
spi_atk_deregister_event_listeners (void);

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
  g_print("D-Bus unique name is : %s\n", dbus_bus_get_unique_name(ad->droute.bus));

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

  atk_dbus_initialize (&ad->droute);
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

  message = dbus_message_new_method_call (SPI_DBUS_NAME_REGISTRY, SPI_DBUS_PATH_REGISTRY, SPI_DBUS_INTERFACE_REGISTRY, "registerApplication");
  dbus_error_init (&error);
  reply = dbus_connection_send_with_reply_and_block(this_app->droute.bus, message, 1000, &error);
  if (error.message) g_print (error.message);
  if (reply) dbus_message_unref (reply);
  if (message) dbus_message_unref (message);
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
deregister_application (SpiAppData *app)
{
  const char *registry = spi_atk_bridge_get_registry ();
  // todo: deregister
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

  spi_atk_deregister_event_listeners();

  deregister_application (this_app);
  this_app = NULL;

  misc = NULL;
}

static void
reinit_register_vars (void)
{
  registry = NULL;
  device_event_controller = NULL;
  this_app = NULL;
}
