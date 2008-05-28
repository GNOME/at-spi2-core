/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Codethink Ltd.
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

#include <glib.h>
#include <accessible.h>

#include <droute/droute.h>
#include <droute/introspect-loader.h>

/*
 * This file contains an implementation of the D-Bus introspectable interface.
 * For ATK objects.
 */

/*
 * This function finds the names of each interface that the ATK object supports
 * and appends the introspection XML for each interface.
 */
static DBusMessage *
impl_introspect (DBusConnection *bus, DBusMessage *message,
		 void *user_data)
{
  AtkObject *object;
  const char *path;
  GString *output;
  char *final;

  DBusMessage *reply;

  path = dbus_message_get_path(message);
  object = spi_dbus_get_object(path);

  output = g_string_new(spi_introspection_header);
  
  g_string_append_printf(output, spi_introspection_node_element, path);

  spi_append_interface(output, "org.freedesktop.atspi.Accessible");

  if (ATK_IS_ACTION (object))
      spi_append_interface(output, "org.freedesktop.atspi.Action");

  if (ATK_IS_COMPONENT (object))
      spi_append_interface(output, "org.freedesktop.atspi.Component");

  if (ATK_IS_EDITABLE_TEXT (object))
      spi_append_interface(output, "org.freedesktop.atspi.EditableText");
  else if (ATK_IS_TEXT (object))
      spi_append_interface(output, "org.freedesktop.atspi.Text");

  if (ATK_IS_HYPERTEXT (object))
      spi_append_interface(output, "org.freedesktop.atspi.Hypertext");

  if (ATK_IS_IMAGE (object))
      spi_append_interface(output, "org.freedesktop.atspi.Image");

  if (ATK_IS_SELECTION (object))
      spi_append_interface(output, "org.freedesktop.atspi.Selection");

  if (ATK_IS_TABLE (object))
      spi_append_interface(output, "org.freedesktop.atspi.Table");

  if (ATK_IS_VALUE (object))
      spi_append_interface(output, "org.freedesktop.atspi.Value");

  if (ATK_IS_STREAMABLE_CONTENT (object))
      spi_append_interface(output, "org.freedesktop.atspi.StreamableContent");

  if (ATK_IS_DOCUMENT (object))
    {
      spi_append_interface(output, "org.freedesktop.atspi.Collection");
      spi_append_interface(output, "org.freedesktop.atspi.Document");
    }

  if (ATK_IS_HYPERLINK_IMPL (object))
      spi_append_interface(output, "org.freedesktop.atspi.Hyperlink");

  g_string_append(output, spi_introspection_footer);
  final = g_string_free(output, FALSE);

  reply = dbus_message_new_method_return (message);
  g_assert(reply != NULL);
  dbus_message_append_args(reply, DBUS_TYPE_STRING, &final,
			   DBUS_TYPE_INVALID);

  g_free(final);
  return reply;
}

static DRouteMethod methods[] = {
  {impl_introspect, "Introspect"},
  {NULL, NULL}
};

/*
 * Adds the introspectable interface to the DRoute object provided
 */
void
spi_initialize_introspectable (DRouteData *data, DRouteGetDatumFunction verify_object)
{
  droute_add_interface (data, "org.freedesktop.DBus.Introspectable",
			methods, NULL,
			verify_object, NULL);
};
