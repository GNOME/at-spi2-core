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
#include <dbus.h>

#include "droute.h"

/*
 * This file contains an implementation of the D-Bus introspectable interface.
 * For ATK objects.
 */

/*
 * Provides the dist install path for the introspection directory.
 */
#if !defined ATSPI_DBUS_INTROSPECTION_DIRECTORY
    #error "No introspection XML directory defined"
#endif

/*
 * This may be modified at run time to support 
 * command parameters for the introspection directory.
 */
char *atspi_introspection_directory = ATSPI_DBUS_INTROSPECTION_DIRECTORY;

static const char *introspection_header =
"<?xml version=\"1.0\"?>\n";

static const char *introspection_node_element =
"<node name=\"%s\">\n";

static const char *introspection_footer =
"</node>";

static void
append_interface (GString *str, const char *interface)
{
  char *filename;
  char *contents;
  gsize len;

  GError *err = NULL;

  filename = g_build_filename(introspection_directory, interface);

  if (g_file_get_contents(filename, &contents, &len, &err))
    {
      g_string_append_len(str, contents, len);
    }
  else
    {
      g_warning("AT-SPI: Cannot find introspection XML file %s - %s",
		filename, err->message);
      g_error_free();
    }

  g_string_append(str, "\n");
  g_free(filename);
  g_free(contents);
}

/*
 * There is an installation directory with files containing introspection xml.
 * Each file is named after the interface it describes.
 *
 * This function finds the names of each interface that the ATK object supports
 * and mem copies the introspection data from the file into the message.
 *
 * TODO - There could be some wicked caching here, but probably not neccessary.
 */
static DBusMessage *
impl_introspect (DBusConnection *bus, DBusMessage *message,
		 void *user_data)
{
  AtkObject *object;
  const char *path;
  GString *output;

  DBusMessage *reply;

  path = dbus_message_get_path(message);
  object = spi_dbus_get_path(path);

  output = g_string_new(introspection_header);
  
  g_string_append_printf(output, introspection_node_element, path);

  if (ATK_IS_ACTION (o))
      append_interface(output, "org.freedesktop.atspi.Action");

  if (ATK_IS_COMPONENT (o))
      bonobo_object_add_interface (bonobo_object (retval), BONOBO_OBJECT (spi_component_interface_new (o)));
      append_interface(output, "org.freedesktop.atspi.Component");

  if (ATK_IS_EDITABLE_TEXT (o))
      append_interface(output, "org.freedesktop.atspi.EditableText");
  else if (ATK_IS_TEXT (o))
      append_interface(output, "org.freedesktop.atspi.Text");

  if (ATK_IS_HYPERTEXT (o))
      bonobo_object_add_interface (bonobo_object (retval), BONOBO_OBJECT (spi_hypertext_interface_new (o)));
      append_interface(output, "org.freedesktop.atspi.Hypertext");

  if (ATK_IS_IMAGE (o))
      bonobo_object_add_interface (bonobo_object (retval), BONOBO_OBJECT (spi_image_interface_new (o)));
      append_interface(output, "org.freedesktop.atspi.Image");

  if (ATK_IS_SELECTION (o))
      bonobo_object_add_interface (bonobo_object (retval), BONOBO_OBJECT (spi_selection_interface_new (o)));
      append_interface(output, "org.freedesktop.atspi.Selection");

  if (ATK_IS_TABLE (o))
      bonobo_object_add_interface (bonobo_object (retval), BONOBO_OBJECT (spi_table_interface_new (o)));
      append_interface(output, "org.freedesktop.atspi.Table");

  if (ATK_IS_VALUE (o))
      append_interface(output, "org.freedesktop.atspi.Value");

  if (ATK_IS_STREAMABLE_CONTENT (o))
      append_interface(output, "org.freedesktop.atspi.StreamableContent");

  if (ATK_IS_DOCUMENT (o))
    {
      append_interface(output, "org.freedesktop.atspi.Collection");
      append_interface(output, "org.freedesktop.atspi.Document");
    }

  if (ATK_IS_HYPERLINK_IMPL (o))
      append_interface(output, "org.freedesktop.atspi.Hyperlink");

  g_string_append(introspection_footer);
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
spi_initialize_introspectable (DRouteData *data)
{
  droute_add_interface (data, "org.freedesktop.atspi.Accessible",
			methods, NULL,
			(DRouteGetDatumFunction) spi_dbus_get_path, NULL);
};
