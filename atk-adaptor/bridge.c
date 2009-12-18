/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008, 2009 Codethink Ltd.
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
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <atk/atk.h>

#include <droute/droute.h>

#include "bridge.h"
#include "event.h"
#include "accessible-register.h"
#include "adaptors.h"

#include "common/spi-dbus.h"

/*
 * Provides the path for the introspection directory.
 */
#if !defined ATSPI_INTROSPECTION_PATH
    #error "No introspection XML directory defined"
#endif

/*---------------------------------------------------------------------------*/

SpiAppData *atk_adaptor_app_data = NULL;

static const AtkMisc *atk_misc = NULL;

/*static Display *bridge_display = NULL;*/

/*---------------------------------------------------------------------------*/

/*
 * Returns a 'canonicalized' value for DISPLAY,
 * with the screen number stripped off if present.
 *
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

/*---------------------------------------------------------------------------*/

/*
 * Gets the IOR from the XDisplay.
 * Not currently used in D-Bus version, but something similar
 * may be employed in the future for accessing the registry daemon
 * bus name.
 */

static DBusConnection *
spi_atk_bridge_get_bus (void)
{
     Atom AT_SPI_BUS;
     Atom actual_type;
     Display *bridge_display;
     int actual_format;
     unsigned char *data = NULL;  
     unsigned long nitems;
     unsigned long leftover;

     DBusConnection *bus = NULL;
     DBusError       error;

     bridge_display = XOpenDisplay (spi_display_name ());
     if (!bridge_display)
	g_error ("AT_SPI: Could not get the display\n");

     AT_SPI_BUS = XInternAtom (bridge_display, "AT_SPI_BUS", False); 
     XGetWindowProperty(bridge_display, 
                        XDefaultRootWindow (bridge_display),
                        AT_SPI_BUS, 0L,
                        (long)BUFSIZ, False,
                        (Atom) 31, &actual_type, &actual_format,
                        &nitems, &leftover, &data);

     dbus_error_init (&error);

     if (data == NULL)
     {
         g_warning ("AT-SPI: Accessibility bus not found - Using session bus.\n");
         bus = dbus_bus_get (DBUS_BUS_SESSION, &error);
         if (!bus)
             g_error ("AT-SPI: Couldn't connect to bus: %s\n", error.message);
     }
     else
     {
	 bus = dbus_connection_open (data, &error);
         if (!bus)
         {
             g_error ("AT-SPI: Couldn't connect to bus: %s\n", error.message);
         }
	 else
         {
	     if (!dbus_bus_register (bus, &error))
	         g_error ("AT-SPI: Couldn't register with bus: %s\n");
         } 
     }

     return bus;
}

/*---------------------------------------------------------------------------*/

static void
register_application (SpiAppData *app)
{
  DBusMessage *message;
  DBusMessageIter iter;
  DBusError error;
  const char *uname = NULL;

  dbus_error_init (&error);

  message = dbus_message_new_method_call (SPI_DBUS_NAME_REGISTRY,
                                          SPI_DBUS_PATH_REGISTRY,
                                          SPI_DBUS_INTERFACE_REGISTRY,
                                          "RegisterApplication");
  dbus_message_set_no_reply (message, TRUE);

  uname = dbus_bus_get_unique_name(app->bus);
  if (!uname)
  {
      g_error ("AT-SPI: Couldn't get unique name for this connection");
  }

  dbus_message_iter_init_append(message, &iter);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &uname);
  dbus_connection_send (app->bus, message, NULL);
  if (message) dbus_message_unref (message);
}

/*---------------------------------------------------------------------------*/

static void
deregister_application (SpiAppData *app)
{
  DBusMessage *message;
  DBusMessageIter iter;
  DBusError error;
  const char *uname;

  dbus_error_init (&error);

  message = dbus_message_new_method_call (SPI_DBUS_NAME_REGISTRY,
                                          SPI_DBUS_PATH_REGISTRY,
                                          SPI_DBUS_INTERFACE_REGISTRY,
                                          "DeregisterApplication");
  dbus_message_set_no_reply (message, TRUE);

  uname = dbus_bus_get_unique_name(app->bus);

  dbus_message_iter_init_append(message, &iter);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &uname);
  dbus_connection_send (app->bus, message, NULL);
  if (message) dbus_message_unref (message);
}

/*---------------------------------------------------------------------------*/

static void
exit_func (void)
{
  if (!atk_adaptor_app_data)
    {
      return;
    }

  spi_atk_tidy_windows ();
  spi_atk_deregister_event_listeners();
  deregister_application (atk_adaptor_app_data);

  g_free(atk_adaptor_app_data);
  atk_adaptor_app_data = NULL;

  /* Not currently creating an XDisplay */
#if 0
  if (bridge_display)
    XCloseDisplay (bridge_display);
#endif
}

/*---------------------------------------------------------------------------*/

#ifdef __ATK_PLUG_H__
static AtkPlugClass *plug_class;
static AtkSocketClass *socket_class;

static gchar *
get_plug_id (AtkPlug *plug)
{
  const char *uname = dbus_bus_get_unique_name(atk_adaptor_app_data->bus);
  gchar *path;
  GString *str = g_string_new (NULL);

  path = atk_dbus_object_to_path (ATK_OBJECT(plug), TRUE);
  g_string_printf (str, "%s:%s", uname, path);
  g_free (path);
  return g_string_free (str, FALSE);
}

static void
socket_embed_hook (AtkSocket *socket, gchar *plug_id)
{
  AtkObject *accessible = ATK_OBJECT(socket);
  /* Force registration */
  gchar *path = atk_dbus_object_to_path (accessible, TRUE);
  spi_emit_cache_update (accessible, atk_adaptor_app_data->bus);
  g_free (path);
}

static void
install_plug_hooks ()
{
  gpointer data;
  
  data = g_type_class_ref (ATK_TYPE_PLUG);
  plug_class = ATK_PLUG_CLASS (data);
  data = g_type_class_ref (ATK_TYPE_SOCKET);
  socket_class = ATK_SOCKET_CLASS (data);
  plug_class->get_object_id = get_plug_id;
  socket_class->embed = socket_embed_hook;
}
#endif

gchar *atspi_dbus_name = NULL;
static gboolean atspi_no_register = FALSE;

static GOptionEntry atspi_option_entries[] =
{
  {"atspi-dbus-name", 0, 0, G_OPTION_ARG_STRING, &atspi_dbus_name, "D-Bus bus name to register as", NULL},
  {"atspi-no-register", 0, 0, G_OPTION_ARG_NONE, &atspi_no_register, "Do not register with Registry Daemon", NULL},
  {NULL}
};

/*
 * spi_app_init
 *
 * The following needs to be initialized.
 *
 * - DRoute for routing message to their accessible objects.
 * - Event handlers for emmitting signals on specific ATK events.
 * - Application registration with the AT-SPI registry.
 *
 */
static int
adaptor_init (gint *argc, gchar **argv[])
{
  GOptionContext *opt;
  GError *err = NULL;
  DBusError error;
  DBusConnection *bus;
  AtkObject *root;
  gchar *introspection_directory;
  static gboolean inited = FALSE;

  if (inited)
    return 0;

  inited = TRUE;

  DRoutePath *treepath, *accpath;

  root = atk_get_root ();
  g_return_val_if_fail (root, 0);

  /* Parse command line options */
  opt = g_option_context_new(NULL);
  g_option_context_add_main_entries(opt, atspi_option_entries, NULL);
  g_option_context_set_ignore_unknown_options(opt, TRUE);
  if (!g_option_context_parse(opt, argc, argv, &err))
      g_warning("AT-SPI Option parsing failed: %s\n", err->message);

  /* Allocate global data and do ATK initializations */
  atk_adaptor_app_data = g_new0 (SpiAppData, 1);
  atk_misc = atk_misc_get_instance ();
  atk_adaptor_app_data->root = root;

  /* Set up D-Bus connection and register bus name */
  dbus_error_init (&error);
  atk_adaptor_app_data->bus = spi_atk_bridge_get_bus ();
  if (!atk_adaptor_app_data->bus)
  {
    g_free(atk_adaptor_app_data);
    atk_adaptor_app_data = NULL;
    return 0;
  }

  if (atspi_dbus_name != NULL)
  {
    if (dbus_bus_request_name(atk_adaptor_app_data->bus, atspi_dbus_name, 0, &error))
    {
      g_print("AT-SPI Recieved D-Bus name - %s\n", atspi_dbus_name);
    }
    else
    {
      g_print("AT-SPI D-Bus name requested but could not be allocated - %s\n", atspi_dbus_name);
    }
  }

  dbus_connection_setup_with_g_main(atk_adaptor_app_data->bus, g_main_context_default());

  /* Get D-Bus introspection directory */
  introspection_directory = (char *) g_getenv("ATSPI_INTROSPECTION_PATH");
  if (introspection_directory == NULL)
      introspection_directory = ATSPI_INTROSPECTION_PATH;

  /* Register droute for routing AT-SPI messages */
  atk_adaptor_app_data->droute = droute_new (atk_adaptor_app_data->bus, introspection_directory);

  treepath = droute_add_one (atk_adaptor_app_data->droute,
                             "/org/freedesktop/atspi/tree",
                             NULL);

  accpath = droute_add_many (atk_adaptor_app_data->droute,
                             "/org/freedesktop/atspi/accessible",
                             NULL,
                             (DRouteGetDatumFunction) atk_dbus_path_to_gobject);

  /* Register all interfaces with droute and set up application accessible db */
  spi_initialize_tree (treepath);

  spi_initialize_accessible (accpath);
  spi_initialize_application (accpath);
  spi_initialize_action(accpath);
  spi_initialize_collection (accpath);
  spi_initialize_component (accpath);
  spi_initialize_document (accpath);
  spi_initialize_editabletext (accpath);
  spi_initialize_hyperlink (accpath);
  spi_initialize_hypertext (accpath);
  spi_initialize_image (accpath);
  spi_initialize_selection (accpath);
  spi_initialize_table (accpath);
  spi_initialize_text (accpath);
  spi_initialize_value (accpath);

  /* Initialize the AtkObject registration */
  atk_dbus_initialize (atk_adaptor_app_data->root);

  /* Register methods to send D-Bus signals on certain ATK events */
  spi_atk_register_event_listeners ();

#ifdef __ATK_PLUG_H__
  /* Hook our plug-and socket functions */
  install_plug_hooks ();
#endif

  /* Register this app by sending a signal out to AT-SPI registry daemon */
  if (!atspi_no_register)
    register_application (atk_adaptor_app_data);

  g_atexit (exit_func);

  return 0;
}

/*---------------------------------------------------------------------------*/

int
gtk_module_init (gint *argc, gchar **argv[])
{
  const gchar *load_bridge = g_getenv ("NO_AT_BRIDGE");

  if (!load_bridge || g_ascii_strtod (load_bridge, NULL) == 0)
    {
	return adaptor_init (argc, argv);
    }
  return 0;
}

void
gnome_accessibility_module_init (void)
{
  const gchar *load_bridge = g_getenv ("NO_AT_BRIDGE");

  if (!load_bridge || g_ascii_strtod (load_bridge, NULL) == 0)
    {
      adaptor_init (NULL, NULL);

      if (g_getenv ("AT_SPI_DEBUG"))
        {
	    g_print("Atk Accessibility bridge initialized\n");
        }
    }
}

void
gnome_accessibility_module_shutdown (void)
{
  spi_atk_deregister_event_listeners ();
  exit_func ();
}
/*END------------------------------------------------------------------------*/
