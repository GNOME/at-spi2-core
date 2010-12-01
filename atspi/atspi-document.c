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

#include "atspi-private.h"

/**
 * atspi_document_get_locale:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Gets the locale associated with the document's content.
 * e.g. the locale for LOCALE_TYPE_MESSAGES.
 *
 * Returns: a string compliant with the POSIX standard for locale description.
 **/
gchar *
atspi_document_get_locale (AtspiDocument *obj, GError **error)
{
  gchar *retval = NULL;

  g_return_val_if_fail (obj != NULL, g_strdup ("C"));

  _atspi_dbus_call (obj, atspi_interface_document, "GetLocale", error, "=>s", &retval);

  return retval;
}

/**
 * atspi_document_get_attribute_value:
 * @obj: a pointer to the #Accessible object on which to operate.
 * @attribute: a string indicating the name of a specific attribute 
 *
 * Gets the value of a single attribute, if specified for the document as a whole.
 *
 * (name-value pair) being queried.
 * 
 * Returns a string corresponding to the value of the specified attribute, or
 * an empty string if the attribute is unspecified for the object.
 **/
gchar *
atspi_document_get_attribute_value (AtspiDocument *obj,
				      gchar *attribute,
				      GError **error)
{
  gchar *retval = NULL;

  g_return_val_if_fail (obj != NULL, NULL);

  _atspi_dbus_call (obj, atspi_interface_document, "GetAttributevaluee", error, "s=>s", attribute, &retval);

  if (!retval)
    retval = g_strdup ("");

  return retval;
}
				      

/**
 * atspi_document_get_attributes:
 * @obj: a pointer to the #Accessible object on which to operate.
 * 
 * Gets all attributes specified for a document as a whole.  
 *
 * For attributes which change within 
 * the document content, see atspi_text_get_attribute_run instead.
 * 
 * Returns: (element-type gchar* gchar*) (transfer full): an ::AttributeSet
 *          containing the attributes of the document, as name-value pairs.
 **/
GHashTable *
atspi_document_get_attributes (AtspiDocument *obj, GError **error)
{
  DBusMessage *message;
  GHashTable *ret;

    g_return_val_if_fail (obj != NULL, NULL);

  message = _atspi_dbus_call_partial (obj, atspi_interface_document, "GetAttributes", error, "");
  ret = _atspi_dbus_hash_from_message (message);
  dbus_message_unref (message);
  return ret;
}

static void
atspi_document_base_init (AtspiDocument *klass)
{
}

GType
atspi_document_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo =
    {
      sizeof (AtspiDocument),
      (GBaseInitFunc) atspi_document_base_init,
      (GBaseFinalizeFunc) NULL,
    };

    type = g_type_register_static (G_TYPE_INTERFACE, "AtspiDocument", &tinfo, 0);

  }
  return type;
}