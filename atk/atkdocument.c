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

#include "atkdocument.h"

GType
atk_document_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo =
    {
      sizeof (AtkDocumentIface),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,

    };

    type = g_type_register_static (G_TYPE_INTERFACE, "AtkDocument", &tinfo, 0);
  }

  return type;
}

/**
 * atk_document_get_document_type:
 * @document: a #GObject instance that implements AtkDocumentIface
 *
 * Gets a string indicating the document type.
 *
 * Returns: a string indicating the document type
 **/
G_CONST_RETURN gchar*
atk_document_get_document_type (AtkDocument *document)
{
  AtkDocumentIface *iface;

  g_return_val_if_fail (ATK_IS_DOCUMENT (document), NULL);

  iface = ATK_DOCUMENT_GET_IFACE (document);

  if (iface->get_document_type)
    {
      return (iface->get_document_type) (document);
    }
  else
    {
      return NULL;
    }
}

/**
 * atk_document_get_document:
 * @document: a #GObject instance that implements AtkDocumentIface
 *
 * Gets a %gpointer that points to an instance of the DOM.  It is
 * up to the caller to check atk_document_get_type to determine
 * how to cast this pointer.
 *
 * Returns: a %gpointer that points to an instance of the DOM.
 **/
gpointer 
atk_document_get_document (AtkDocument *document)
{
  AtkDocumentIface *iface;

  g_return_val_if_fail (ATK_IS_DOCUMENT (document), NULL);

  iface = ATK_DOCUMENT_GET_IFACE (document);

  if (iface->get_document)
    {
      return (iface->get_document) (document);
    }
  else
    {
      return NULL;
    }
}

