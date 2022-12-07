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
 * AtspiEditabletext:
 *
 * An interface that provides methods for modifying textual content
 * of components which support editing.
 *
 * Derived from atspi-text, the atspi-editabletext interface
 * provides methods for modifying textual content of components
 * which support editing. EditableText also interacts with the
 * system clipboard via copy, cut, and paste methods.
 */

#if 0
/* TODO: implement */
/**
 * atspi_editable_text_set_attributes:
 * @obj: a pointer to the #AtspiEditableText object to modify.
 * @attributes: a string indicating the attributes to apply to the range,
 *        delimited by ':'.
 * @startOffset: a #gint indicating the start of the desired text range.
 * @endOffset: a #gint indicating the first character past the desired range.
 *
 * Sets the attributes applied to a range of text from an #AtspiEditableText
 *          object, and the bounds of the range.
 *
 * Returns: #TRUE if the operation was successful, otherwise #FALSE.
 **/
gboolean
atspi_editable_text_set_attributes (AtspiEditableText *obj,
				    const char *attributes,
				    gint start_pos,
				    gint end_pos,
				    GError **error
{
  dbus_int32_t d_start_pos = start_pos, d_end_pos = end_pos;
  dbus_bool_t retval = FALSE;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_editable_text, "SetAttributes", error, "sii=>b", attributes, d_start_pos, d_end_pos, &retval);

  return retval;
}
#endif

/**
 * atspi_editable_text_set_text_contents:
 * @obj: a pointer to the #AtspiEditableText object to modify.
 * @new_contents: a character string, encoded in UTF-8, which is to
 *      become the new text contents of the #AtspiEditableText object.
 *
 * Replace the entire text contents of an #AtspiEditableText object.
 *
 * Returns: #TRUE if the operation was successful, otherwise #FALSE.
 **/
gboolean
atspi_editable_text_set_text_contents (AtspiEditableText *obj,
                                       const gchar *new_contents,
                                       GError **error)
{
  dbus_bool_t retval = FALSE;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_editable_text, "SetTextContents", error, "s=>b", new_contents, &retval);

  return retval;
}

/**
 * atspi_editable_text_insert_text:
 * @obj: a pointer to the #AtspiEditableText object to modify.
 * @position: a #gint indicating the character offset at which to insert
 *       the new text.
 * @text: a string representing the text to insert, in UTF-8 encoding.
 * @length:  the number of characters of text to insert, in bytes. If the
 * byte count of text is less than or equal to length, the entire contents
 * of text will be inserted.
 *
 * Inserts text into an #AtspiEditableText object.
 * As with all character offsets, the specified @position may not be the
 * same as the resulting byte offset, since the text is in a
 * variable-width encoding.
 *
 * Returns: #TRUE if the operation was successful, otherwise #FALSE.
 **/
gboolean
atspi_editable_text_insert_text (AtspiEditableText *obj,
                                 gint position,
                                 const gchar *text,
                                 gint length,
                                 GError **error)
{
  dbus_int32_t d_position = position, d_length = length;
  dbus_bool_t retval = FALSE;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_editable_text, "InsertText", error, "isi=>b", d_position, text, d_length, &retval);

  return retval;
}

/**
 * atspi_editable_text_copy_text:
 * @obj: a pointer to the #AtspiEditableText object to modify.
 * @start_pos: a #gint indicating the starting character offset
 *       of the text to copy.
 * @end_pos: a #gint indicating the offset of the first character
 *       past the end of the text section to be copied.
 *
 * Copies text from an #AtspiEditableText object into the system clipboard.
 *
 * see: #atspi_editable_text_paste_text
 *
 * Returns: #TRUE if the operation was successful, otherwise #FALSE.
 **/
gboolean
atspi_editable_text_copy_text (AtspiEditableText *obj,
                               gint start_pos,
                               gint end_pos,
                               GError **error)
{
  dbus_int32_t d_start_pos = start_pos, d_end_pos = end_pos;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_editable_text, "CopyText", error, "ii", d_start_pos, d_end_pos);

  return TRUE;
}

/**
 * atspi_editable_text_cut_text:
 * @obj: a pointer to the #AtspiEditableText object to modify.
 * @start_pos: a #gint indicating the starting character offset
 *       of the text to cut.
 * @end_pos: a #gint indicating the offset of the first character
 *       past the end of the text section to be cut.
 *
 * Deletes text from an #AtspiEditableText object, copying the
 *       excised portion into the system clipboard.
 *
 * see: #atspi_editable_text_paste_text
 *
 * Returns: #TRUE if operation was successful, #FALSE otherwise.
 **/
gboolean
atspi_editable_text_cut_text (AtspiEditableText *obj,
                              gint start_pos,
                              gint end_pos,
                              GError **error)
{
  dbus_int32_t d_start_pos = start_pos, d_end_pos = end_pos;
  dbus_bool_t retval = FALSE;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_editable_text, "CutText", error, "ii=>b", d_start_pos, d_end_pos, &retval);

  return retval;
}

/**
 * atspi_editable_text_delete_text:
 * @obj: a pointer to the #AtspiEditableText object to modify.
 * @start_pos: a #gint indicating the starting character offset
 *       of the text to delete.
 * @end_pos: a #gint indicating the offset of the first character
 *       past the end of the text section to be deleted.
 *
 * Deletes text from an #AtspiEditableText object, without copying the
 *       excised portion into the system clipboard.
 *
 * see: #atspi_editable_text_cut_text
 *
 * Returns: #TRUE if the operation was successful, otherwise #FALSE.
 **/
gboolean
atspi_editable_text_delete_text (AtspiEditableText *obj,
                                 gint start_pos,
                                 gint end_pos,
                                 GError **error)
{
  dbus_int32_t d_start_pos = start_pos, d_end_pos = end_pos;
  dbus_bool_t retval = FALSE;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_editable_text, "DeleteText", error, "ii=>b", d_start_pos, d_end_pos, &retval);

  return retval;
}

/**
 * atspi_editable_text_paste_text:
 * @obj: a pointer to the #AtspiEditableText object to modify.
 * @position: a #gint indicating the character offset at which to insert
 *       the new text.
 *
 * Inserts text from the system clipboard into an #AtspiEditableText object.
 * As with all character offsets, the specified @position may not be the
 *       same as the resulting byte offset, since the text is in a
 *       variable-width encoding.
 *
 * Returns: #TRUE if the operation was successful, otherwise #FALSE.
 **/
gboolean
atspi_editable_text_paste_text (AtspiEditableText *obj,
                                gint position,
                                GError **error)
{
  dbus_int32_t d_position = position;
  dbus_bool_t retval = FALSE;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_editable_text, "PasteText", error, "i=>b", d_position, &retval);

  return retval;
}

static void
atspi_editable_text_base_init (AtspiEditableText *klass)
{
}

GType
atspi_editable_text_get_type (void)
{
  static GType type = 0;

  if (!type)
    {
      static const GTypeInfo tinfo = {
        sizeof (AtspiEditableText),
        (GBaseInitFunc) atspi_editable_text_base_init,
        (GBaseFinalizeFunc) NULL,
      };

      type = g_type_register_static (G_TYPE_INTERFACE, "AtspiEditableText", &tinfo, 0);
    }
  return type;
}
