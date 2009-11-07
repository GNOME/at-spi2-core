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

#include <dbus/dbus.h>

#include "paths.h"
#include "registry.h"
#include "deviceeventcontroller.h"

#if !defined ATSPI_INTROSPECTION_PATH
    #error "No introspection XML directory defined"
#endif

static gchar *dbus_name = NULL;

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

  registry = spi_registry_new (bus);
  dec = spi_registry_dec_new (registry, bus);

  g_main_loop_run (mainloop);
  return 0;
}
