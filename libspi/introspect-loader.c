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

/*
 * This file contains utilities for loading introspection XML
 * from the local file system.
 *
 * There is an installation directory with files containing introspection xml.
 * Each file is named after the interface it describes.
 */

/*
 * Provides the path for the introspection directory.
 */
#if !defined ATSPI_DBUS_INTROSPECTION_DIRECTORY
    #error "No introspection XML directory defined"
#endif

const char *spi_introspection_header =
"<?xml version=\"1.0\"?>\n";

const char *spi_introspection_node_element =
"<node name=\"%s\">\n";

const char *spi_introspection_footer =
"</node>";

void
spi_append_interface (GString *str, const char *interface)
{
  char *filename;
  char *contents;
  char *introspection_directory;
  gsize len;

  GError *err = NULL;

  introspection_directory = (char *) g_getenv("ATSPI_DBUS_INTROSPECTION_DIRECTORY");
  if (introspection_directory == NULL)
      introspection_directory = ATSPI_DBUS_INTROSPECTION_DIRECTORY;

  filename = g_build_filename(introspection_directory, interface, NULL);

  if (g_file_get_contents(filename, &contents, &len, &err))
    {
      g_string_append_len(str, contents, len);
    }
  else
    {
      g_warning("AT-SPI: Cannot find introspection XML file %s - %s",
		filename, err->message);
      g_error_free(err);
    }

  g_string_append(str, "\n");
  g_free(filename);
  g_free(contents);
}
