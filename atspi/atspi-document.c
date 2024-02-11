/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#include "atspi-private.h"

/**
 * atspi_document_get_locale:
 * @obj: a pointer to the #AtspiDocument object on which to operate.
 *
 * Gets the locale associated with the document's content,
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
 * atspi_document_get_attribute_value: (rename-to atspi_document_get_document_attribute_value)
 * @obj: a pointer to the #AtspiDocument object on which to operate.
 * @attribute: a string indicating the name of a specific attribute.
 *
 * Gets the value of a single attribute, if specified for the document as a whole.
 *
 * Returns: a string corresponding to the value of the specified attribute, or
 * an empty string if the attribute is unspecified for the object.
 *
 * Deprecated: 2.10: Use atspi_document_get_document_attribute_value instead.
 **/
gchar *
atspi_document_get_attribute_value (AtspiDocument *obj,
                                    const gchar *attribute,
                                    GError **error)
{
  return atspi_document_get_document_attribute_value (obj, attribute, error);
}

/**
 * atspi_document_get_document_attribute_value:
 * @obj: a pointer to the #AtspiDocument object on which to operate.
 * @attribute: a string indicating the name of a specific attribute.
 *
 * Gets the value of a single attribute, if specified for the document as a whole.
 *
 * Returns: a string corresponding to the value of the specified attribute, or
 * an empty string if the attribute is unspecified for the object.
 **/
gchar *
atspi_document_get_document_attribute_value (AtspiDocument *obj,
                                             const gchar *attribute,
                                             GError **error)
{
  gchar *retval = NULL;

  g_return_val_if_fail (obj != NULL, NULL);

  _atspi_dbus_call (obj, atspi_interface_document, "GetAttributeValue", error, "s=>s", attribute, &retval);

  if (!retval)
    retval = g_strdup ("");

  return retval;
}

/**
 * atspi_document_get_attributes: (rename-to atspi_document_get_document_attributes)
 * @obj: a pointer to the #AtspiDocument object on which to operate.
 *
 * Gets all constant attributes for the document as a whole. For attributes
 * that change within the document content, see @atspi_text_get_attribute_run instead.
 *
 * Returns: (element-type gchar* gchar*) (transfer full): a #GHashTable
 *          containing the constant attributes of the document, as name-value pairs.
 *
 * Deprecated: 2.10: Use atspi_document_get_document_attributes instead.
 **/
GHashTable *
atspi_document_get_attributes (AtspiDocument *obj, GError **error)
{
  return atspi_document_get_document_attributes (obj, error);
}

/**
 * atspi_document_get_document_attributes:
 * @obj: a pointer to the #AtspiDocument object on which to operate.
 *
 * Gets all constant attributes for the document as a whole. For attributes
 * that change within the document content, see @atspi_text_get_attribute_run instead.
 *
 * Returns: (element-type gchar* gchar*) (transfer full): a #GHashTable
 *          containing the constant attributes of the document, as name-value pairs.
 **/
GHashTable *
atspi_document_get_document_attributes (AtspiDocument *obj, GError **error)
{
  DBusMessage *message;

  g_return_val_if_fail (obj != NULL, NULL);

  message = _atspi_dbus_call_partial (obj, atspi_interface_document, "GetAttributes", error, "");
  return _atspi_dbus_return_hash_from_message (message);
}

/**
 * atspi_document_get_page_count:
 * @obj: a pointer to the #AtspiDocument object to query.
 *
 * Gets the page count of an #AccessibleDocument object.
 *
 * Returns: a #gint indicating the page count of an
 * #AccessibleDocument object.
 **/
gint
atspi_document_get_page_count (AtspiDocument *obj, GError **error)
{
  dbus_int32_t retval = 0;

  g_return_val_if_fail (obj != NULL, -1);

  _atspi_dbus_get_property (obj, atspi_interface_document, "PageCount", error, "i", &retval);

  return retval;
}

/**
 * atspi_document_get_current_page_number:
 * @obj: a pointer to the #AtspiDocument object to query.
 *
 * Gets the current page number of an #AccessibleDocument object.
 *
 * Returns: a #gint indicating the current page number in the
 * #AccessibleDocument object.
 **/
gint
atspi_document_get_current_page_number (AtspiDocument *obj, GError **error)
{
  dbus_int32_t retval = 0;

  g_return_val_if_fail (obj != NULL, -1);

  _atspi_dbus_get_property (obj, atspi_interface_document, "CurrentPageNumber", error, "i", &retval);

  return retval;
}

static void
clear_text_selection (void *data)
{
  AtspiTextSelection *selection = data;

  g_clear_object (&selection->start_object);
  g_clear_object (&selection->end_object);
}

/**
 * atspi_document_get_text_selections:
 * @document: an #AtspiDocument
 *
 * Returns an array of AtspiTextSelections within this document.
 *
 * Returns: (element-type AtspiTextSelection) (transfer full): a GArray of
 * AtspiTextSelection structures representing the selection.
 *
 * Since: 2.52
 */
GArray *
atspi_document_get_text_selections (AtspiDocument *obj, GError **error)
{
  DBusMessage *message;
  GArray *selections;
  DBusMessageIter iter, iter_array, iter_struct;

  g_return_val_if_fail (obj != NULL, NULL);

  message = _atspi_dbus_call_partial (obj, atspi_interface_document, "GetTextSelections", error, "");
  if (!message)
    return NULL;

  if (strcmp (dbus_message_get_signature (message), "a((so)i(so)ib)") != 0)
    {
      dbus_message_unref (message);
      return NULL;
    }

  selections = g_array_new (FALSE, TRUE, sizeof (AtspiTextSelection));
  g_array_set_clear_func (selections, clear_text_selection);
  dbus_message_iter_init (message, &iter);
  dbus_message_iter_recurse (&iter, &iter_array);

  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
    {
      AtspiTextSelection selection;
      dbus_message_iter_recurse (&iter_array, &iter_struct);
      selection.start_object = _atspi_dbus_consume_accessible (&iter_struct);
      dbus_message_iter_get_basic (&iter_struct, &selection.start_offset);
      dbus_message_iter_next (&iter_struct);
      selection.end_object = _atspi_dbus_consume_accessible (&iter_struct);
      dbus_message_iter_get_basic (&iter_struct, &selection.end_offset);
      dbus_message_iter_next (&iter_struct);
      dbus_message_iter_get_basic (&iter_struct, &selection.start_is_active);
      g_array_append_val (selections, selection);
      dbus_message_iter_next (&iter_array);
    }

  dbus_message_unref (message);
  return selections;
}

static void
append_accessible_to_iter (DBusMessageIter *iter, AtspiAccessible *accessible)
{
  DBusMessageIter iter_struct;
  dbus_message_iter_open_container (iter, DBUS_TYPE_STRUCT, NULL, &iter_struct);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &accessible->parent.app->bus_name);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &accessible->parent.path);
  dbus_message_iter_close_container (iter, &iter_struct);
}

/**
 * atspi_document_set_text_selections:
 * @document: an #AtspiDocument.
 * @selections: (element-type AtspiTextSelection): a GArray of AtspiTextSelections
 *              to be selected.
 *
 * Makes 1 or more selections within this document denoted by the given
 * array of AtspiTextSelections. Any existing physical selection (inside or
 * outside this document) is replaced by the new selections. All objects within
 * the given selection ranges must be descendants of this document. Otherwise
 * FALSE will be returned.
 *
 * Returns: TRUE if the selection was made successfully; FALSE otherwise.
 *
 * Since: 2.52
 */
gboolean
atspi_document_set_text_selections (AtspiDocument *obj,
                                    GArray *selections,
                                    GError **error)
{
  DBusMessage *message, *reply;
  AtspiAccessible *accessible;
  DBusMessageIter iter, iter_struct, iter_array;
  gint i, count;
  dbus_bool_t ret = FALSE;

  g_return_val_if_fail (obj != NULL, FALSE);
  accessible = ATSPI_ACCESSIBLE (obj);

  message = dbus_message_new_method_call (accessible->parent.app->bus_name,
                                          accessible->parent.path,
                                          atspi_interface_document,
                                          "SetTextSelections");
  count = (selections ? selections->len : 0);

  dbus_message_iter_init_append (message, &iter);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "((so)i(so)ib)", &iter_array);
  for (i = 0; i < count; i++)
    {
      dbus_message_iter_open_container (&iter_array, DBUS_TYPE_STRUCT, NULL, &iter_struct);
      AtspiTextSelection *item = &g_array_index (selections, AtspiTextSelection, i);
      append_accessible_to_iter (&iter_struct, item->start_object);
      dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, &item->start_offset);
      append_accessible_to_iter (&iter_struct, item->end_object);
      dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, &item->end_offset);
      dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_BOOLEAN, &item->start_is_active);
      dbus_message_iter_close_container (&iter_array, &iter_struct);
    }
  dbus_message_iter_close_container (&iter, &iter_array);

  reply = _atspi_dbus_send_with_reply_and_block (message, error);
  if (reply)
    {
      dbus_message_get_args (reply, NULL, DBUS_TYPE_BOOLEAN, &ret, DBUS_TYPE_INVALID);
      dbus_message_unref (reply);
    }
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

  if (!type)
    {
      static const GTypeInfo tinfo = {
        sizeof (AtspiDocument),
        (GBaseInitFunc) atspi_document_base_init,
        (GBaseFinalizeFunc) NULL,
      };

      type = g_type_register_static (G_TYPE_INTERFACE, "AtspiDocument", &tinfo, 0);
    }
  return type;
}
