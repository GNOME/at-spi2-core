/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; https://wiki.gnome.org/Accessibility)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/*
 * Simple test application for AT-SPI.
 *
 * The only thing this application does, is registering itself to the AT-SPI
 * registry and then waiting to get killed by some external force.
 */

#include <atspi/atspi.h>
#include <dbus/dbus.h>
#include <glib.h>
#include <stdio.h>

static GMainLoop *mainloop;

int
register_app ()
{
  DBusConnection *connection = NULL;
  DBusMessage *message;
  DBusMessageIter iter;
  DBusMessageIter subiter;
  DBusError error;
  DBusMessage *reply;
  const gchar *name;
  gchar *path;

  /* Set up D-Bus connection and register bus name */
  dbus_error_init (&error);
  connection = atspi_get_a11y_bus ();
  if (!connection)
    {
      printf ("Couldn't get a11y bus!\n");
      return -1;
    }

  /* Register this app by sending a signal out to AT-SPI registry daemon */
  message = dbus_message_new_method_call (ATSPI_DBUS_NAME_REGISTRY,
                                          ATSPI_DBUS_PATH_ROOT,
                                          ATSPI_DBUS_INTERFACE_SOCKET,
                                          "Embed");

  dbus_message_iter_init_append (message, &iter);

  name = dbus_bus_get_unique_name (connection);
  path = g_strdup (ATSPI_DBUS_PATH_NULL);

  dbus_message_iter_open_container (&iter, DBUS_TYPE_STRUCT, NULL,
                                    &subiter);
  dbus_message_iter_append_basic (&subiter, DBUS_TYPE_STRING, &name);
  dbus_message_iter_append_basic (&subiter, DBUS_TYPE_OBJECT_PATH, &path);
  dbus_message_iter_close_container (&iter, &subiter);

  g_free (path);

  reply = dbus_connection_send_with_reply_and_block (connection, message, -1, &error);
  if (!reply)
    {
      printf ("Did not get a reply from the registry.\n");
      dbus_message_unref (message);
      dbus_error_free (&error);
      return -1;
    }

  dbus_message_unref (message);
  dbus_message_unref (reply);
  dbus_error_free (&error);
  return 0;
}

int
main (int argc, char *argv[])
{
  int ret = register_app ();
  if (ret)
    {
      printf ("Failed to send dbus signals. Aborting.\n");
      return ret;
    }

  // This keeps the test-application runnig indefinitely, i.e.
  // until killed by an external signal.
  mainloop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (mainloop);

  return 0;
}
