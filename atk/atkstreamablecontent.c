/* ATK -  Accessibility Toolkit
 * Copyright 2001 Sun Microsystems Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "atkstreamablecontent.h"

GType
atk_streamable_content_get_type (void)
{
  static GType type = 0;

  if (!type) {
    GTypeInfo tinfo =
    {
      sizeof (AtkStreamableContentIface),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,

    };

    type = g_type_register_static (G_TYPE_INTERFACE, "AtkStreamableContent", &tinfo, 0);
  }

  return type;
}

/**
 * atk_streamable_content_get_n_mime_types:
 * @streamable: a GObject instance that implements AtkStreamableContentIface
 *
 * Gets the number of mime types supported by this object.
 *
 * Returns: a gint which is the number of mime types supported by the object.
 **/
gint
atk_streamable_content_get_n_mime_types (AtkStreamableContent *streamable)
{
  AtkStreamableContentIface *iface;

  g_return_val_if_fail (ATK_IS_STREAMABLE_CONTENT (streamable), 0);

  iface = ATK_STREAMABLE_CONTENT_GET_IFACE (streamable);

  if (iface->get_n_mime_types)
    return (iface->get_n_mime_types) (streamable);
  else
    return 0;
}

/**
 * atk_streamable_content_get_mime_type:
 * @streamable: a GObject instance that implements AtkStreamableContent
 * @i: a gint representing the position of the mime type starting from 0
 *
 * Gets the character string of the specified mime type. The first mime
 * type is at position 0, the second at position 1, and so on.
 *
 * Returns : a gchar* representing the specified mime type; the caller
 * should not free the character string.
 **/
G_CONST_RETURN gchar*
atk_streamable_content_get_mime_type (AtkStreamableContent *streamable,
                                      gint                 i)
{
  AtkStreamableContentIface *iface;

  g_return_val_if_fail (i >= 0, NULL);
  g_return_val_if_fail (ATK_IS_STREAMABLE_CONTENT (streamable), NULL);

  iface = ATK_STREAMABLE_CONTENT_GET_IFACE (streamable);

  if (iface->get_mime_type)
    return (iface->get_mime_type) (streamable, i);
  else
    return NULL;
}

/**
 * atk_streamable_content_get_stream:
 * @streamable: a GObject instance that implements AtkStreamableContentIface
 * @mime_type: a gchar* representing the mime type
 *
 * Gets the content in the specified mime type.
 *
 * Returns: A #GIOChannel which contains the content in the specified mime
 * type.
 **/
GIOChannel*
atk_streamable_content_get_stream (AtkStreamableContent *streamable,
                                   const gchar          *mime_type)
{
  AtkStreamableContentIface *iface;

  g_return_val_if_fail (mime_type != NULL, NULL);
  g_return_val_if_fail (ATK_IS_STREAMABLE_CONTENT (streamable), NULL);

  iface = ATK_STREAMABLE_CONTENT_GET_IFACE (streamable);

  if (iface->get_stream)
    return (iface->get_stream) (streamable, mime_type);
  else
    return NULL;
}
