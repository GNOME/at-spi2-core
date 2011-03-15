/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 * Copyright 2011 Red Hat, Inc.
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

#include <config.h>

#include "libregistry-internals.h"
#include <X11/Xlib.h>
#include <string.h>
#include <stdio.h>

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

/*
 * Gets the IOR from the XDisplay.
 */
static char *
get_accessibility_bus_address_x11 (void)
{
  Atom AT_SPI_BUS;
  Atom actual_type;
  Display *bridge_display;
  int actual_format;
  unsigned char *data = NULL;
  unsigned long nitems;
  unsigned long leftover;

  bridge_display = XOpenDisplay (spi_display_name ());
  if (!bridge_display)
    {
      g_warning ("Could not open X display");
      return NULL;
    }
      
  AT_SPI_BUS = XInternAtom (bridge_display, "AT_SPI_BUS", False);
  XGetWindowProperty (bridge_display,
		      XDefaultRootWindow (bridge_display),
		      AT_SPI_BUS, 0L,
		      (long) BUFSIZ, False,
		      (Atom) 31, &actual_type, &actual_format,
		      &nitems, &leftover, &data);
  XCloseDisplay (bridge_display);

  return g_strdup (data);
}

static char *
get_accessibility_bus_address_dbus (void)
{
  DBusConnection *session_bus = NULL;
  DBusMessage *message;
  DBusMessage *reply;
  char *address = NULL;

  session_bus = dbus_bus_get (DBUS_BUS_SESSION, NULL);
  if (!session_bus)
    return NULL;

  message = dbus_message_new_method_call ("org.a11y.Bus",
					  "/org/a11y/bus",
					  "org.a11y.Bus",
					  "GetAddress");

  reply = dbus_connection_send_with_reply_and_block (session_bus,
						     message,
						     -1,
						     NULL);
  dbus_message_unref (message);

  if (!reply)
    return NULL;
  
  {
    const char *tmp_address;
    if (!dbus_message_get_args (reply,
				NULL,
				DBUS_TYPE_STRING,
				&tmp_address,
				DBUS_TYPE_INVALID))
      {
	dbus_message_unref (reply);
	return NULL;
      }
    address = g_strdup (tmp_address);
    dbus_message_unref (reply);
  }
  
  return address;
}

DBusConnection *
_libregistry_get_a11y_bus (void)
{
  DBusConnection *bus = NULL;
  DBusError error;
  char *address;

  address = get_accessibility_bus_address_x11 ();
  if (!address)
    address = get_accessibility_bus_address_dbus ();
  if (!address)
    return NULL;

  dbus_error_init (&error);
  bus = dbus_connection_open (address, &error);
  if (!bus)
    {
      g_warning ("Couldn't connect to accessibility bus: %s", error.message);
      return NULL;
    }
  else
    {
      if (!dbus_bus_register (bus, &error))
	{
	  g_warning ("Couldn't register with accessibility bus: %s", error.message);
	  return NULL;
	}
    }
  
  return bus;
}
