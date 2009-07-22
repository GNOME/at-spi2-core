/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#include <stdlib.h>
#include <config.h>
#include <string.h>
#include <glib.h>

#include <spi-common/spi-dbus.h>
#include <droute/droute.h>

#include "registry.h"
#include "deviceeventcontroller.h"

#if !defined ATSPI_INTROSPECTION_PATH
    #error "No introspection XML directory defined"
#endif

#ifdef HAVE_SM
#include <X11/SM/SMlib.h>
#endif


static gchar *dbus_name = NULL;

static void registry_session_init (const char *previous_client_id, const char *exe);
static GOptionEntry optentries[] =
{
  {"dbus-name", 0, 0, G_OPTION_ARG_STRING, &dbus_name, "Well-known name to register with D-Bus", NULL},
  {NULL}
};

int
main (int argc, char **argv)
{
  SpiRegistry *registry;
  SpiDEController *dec;
  DRouteContext *droute;
  gchar *introspection_directory;

  GMainLoop *mainloop;
  DBusConnection *bus;

  GOptionContext *opt;

  GError *err = NULL;
  DBusError error;
  int ret;

  g_type_init();

  /* We depend on GDK as well as XLib for device event processing */
  gdk_init(&argc, &argv);

  /*Parse command options*/
  opt = g_option_context_new(NULL);
  g_option_context_add_main_entries(opt, optentries, NULL);

  if (!g_option_context_parse(opt, &argc, &argv, &err))
      g_error("Option parsing failed: %s\n", err->message);

  if (dbus_name == NULL)
      dbus_name = SPI_DBUS_NAME_REGISTRY;

  dbus_error_init (&error);
  bus = dbus_bus_get(DBUS_BUS_SESSION, &error);
  if (!bus)
  {
    g_warning("Couldn't connect to dbus: %s\n", error.message);
  }

  mainloop = g_main_loop_new (NULL, FALSE);
  dbus_connection_setup_with_g_main(bus, g_main_context_default());

  ret = dbus_bus_request_name(bus, dbus_name, DBUS_NAME_FLAG_DO_NOT_QUEUE, &error);
  if (ret == DBUS_REQUEST_NAME_REPLY_EXISTS)
    {
      g_error("Could not obtain D-Bus name - %s\n", dbus_name);
    }
  else
    {
      g_print ("SpiRegistry daemon is running with well-known name - %s\n", dbus_name);
    }

  /* Get D-Bus introspection directory */
  introspection_directory = (char *) g_getenv("ATSPI_INTROSPECTION_PATH");
  if (introspection_directory == NULL)
      introspection_directory = ATSPI_INTROSPECTION_PATH;

  /* Set up D-Route for use by the dec */
  droute = droute_new (bus, introspection_directory);

  registry = spi_registry_new (bus, droute);
  dec = spi_registry_dec_new (registry, bus, droute);


      /* If DESKTOP_AUTOSTART_ID exists, assume we're started by session
       * manager and connect to it. */
      const char *desktop_autostart_id = g_getenv ("DESKTOP_AUTOSTART_ID");
      if (desktop_autostart_id != NULL) {
        char *client_id = g_strdup (desktop_autostart_id);
        /* Unset DESKTOP_AUTOSTART_ID in order to avoid child processes to
         * use the same client id. */
        g_unsetenv ("DESKTOP_AUTOSTART_ID");
        registry_session_init (client_id, argv[0]);
        g_free (client_id);
      }


  g_main_loop_run (mainloop);
  return 0;
}

void
registry_session_init (const char *previous_client_id, const char *exe)
{
#ifdef HAVE_SM
  char buf[256];
  char *client_id;

  SmcConn session_connection =
  SmcOpenConnection (NULL, /* use SESSION_MANAGER env */
                     NULL, /* means use existing ICE connection */
                     SmProtoMajor,
                     SmProtoMinor,
                     0,
                     NULL,
                     (char*) previous_client_id,
                     &client_id,
                     255, buf);

  if (session_connection != NULL) {
    SmProp prop1, prop2, prop3, prop4, prop5, prop6, *props[6];
    SmPropValue prop1val, prop2val, prop3val, prop4val, prop5val, prop6val;
    char pid[32];
    char hint = SmRestartImmediately;
    char priority = 1; /* low to run before other apps */

    prop1.name = SmProgram;
    prop1.type = SmARRAY8;
    prop1.num_vals = 1;
    prop1.vals = &prop1val;
    prop1val.value = exe;
    prop1val.length = strlen (exe);

    /* twm sets getuid() for this, but the SM spec plainly
     * says pw_name, twm is on crack
     */
    prop2.name = SmUserID;
    prop2.type = SmARRAY8;
    prop2.num_vals = 1;
    prop2.vals = &prop2val;
    prop2val.value = (char*) g_get_user_name ();
    prop2val.length = strlen (prop2val.value);

    prop3.name = SmRestartStyleHint;
    prop3.type = SmCARD8;
    prop3.num_vals = 1;
    prop3.vals = &prop3val;
    prop3val.value = &hint;
    prop3val.length = 1;

    sprintf (pid, "%d", getpid ());
    prop4.name = SmProcessID;
    prop4.type = SmARRAY8;
    prop4.num_vals = 1;
    prop4.vals = &prop4val;
    prop4val.value = pid;
    prop4val.length = strlen (prop4val.value);

    /* Always start in home directory */
    prop5.name = SmCurrentDirectory;
    prop5.type = SmARRAY8;
    prop5.num_vals = 1;
    prop5.vals = &prop5val;
    prop5val.value = (char*) g_get_home_dir ();
    prop5val.length = strlen (prop5val.value);

    prop6.name = "_GSM_Priority";
    prop6.type = SmCARD8;
    prop6.num_vals = 1;
    prop6.vals = &prop6val;
    prop6val.value = &priority;
    prop6val.length = 1;

    props[0] = &prop1;
    props[1] = &prop2;
    props[2] = &prop3;
    props[3] = &prop4;
    props[4] = &prop5;
    props[5] = &prop6;

    SmcSetProperties (session_connection, 6, props);
  }

#endif
}

