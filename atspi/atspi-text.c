/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 * Copyright 2010, 2011 Novell, Inc.
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
 * atspi_range_copy:
 * @src: a pointer to the source #AtspiRange object that will be copied.
 *
 * Gets a copy of an #AtspiRange object.
 *
 * Returns: the #AtspiRange copy of an #AtspiRange object.
 **/
AtspiRange *
atspi_range_copy (AtspiRange *src)
{
  AtspiRange *dst = g_new (AtspiRange, 1);

  dst->start_offset = src->start_offset;
  dst->end_offset = src->end_offset;
  return dst;
}

G_DEFINE_BOXED_TYPE (AtspiRange, atspi_range, atspi_range_copy, g_free)

static AtspiTextRange *
atspi_text_range_copy (AtspiTextRange *src)
{
  AtspiTextRange *dst = g_new (AtspiTextRange, 1);

  dst->content = g_strdup (src->content);
  dst->start_offset = src->start_offset;
  dst->end_offset = src->end_offset;
  return dst;
}

static void
atspi_text_range_free (AtspiTextRange *range)
{
  g_free (range->content);
  g_free (range);
}

G_DEFINE_BOXED_TYPE (AtspiTextRange, atspi_text_range, atspi_text_range_copy,
                     atspi_text_range_free)

/**
 * atspi_text_get_character_count:
 * @obj: a pointer to the #AtspiText object to query.
 *
 * Gets the character count of an #AccessibleText object.
 *
 * Returns: a #gint indicating the total number of
 *              characters in the #AccessibleText object.
 **/
gint
atspi_text_get_character_count (AtspiText *obj, GError **error)
{
  dbus_int32_t retval = 0;

  g_return_val_if_fail (obj != NULL, -1);

  _atspi_dbus_get_property (obj, atspi_interface_text, "CharacterCount", error, "i", &retval);

  return retval;
}

/**
 * atspi_text_get_text:
 * @obj: a pointer to the #AtspiText object to query.
 * @start_offset: a #gint indicating the start of the desired text range.
 * @end_offset: a #gint indicating the first character past the desired range.
 *
 * Gets a range of text from an #AtspiText object.  The number of bytes
 *          in the returned string may exceed either end_offset or start_offset, since
 *          UTF-8 is a variable-width encoding.
 *
 * Returns: a text string containing characters from @start_offset
 *          to @end_offset-1, inclusive, encoded as UTF-8.
 **/
gchar *
atspi_text_get_text (AtspiText *obj,
                        gint start_offset,
                        gint end_offset,
                        GError **error)
{
  gchar *retval = NULL;
  dbus_int32_t d_start_offset = start_offset, d_end_offset = end_offset;

  g_return_val_if_fail (obj != NULL, g_strdup (""));

  _atspi_dbus_call (obj, atspi_interface_text, "GetText", error, "ii=>s", d_start_offset, d_end_offset, &retval);

  if (!retval)
    retval = g_strdup ("");

  return retval;
}

/**
 * atspi_text_get_caret_offset:
 * @obj: a pointer to the #AtspiText object to query.
 *
 * Gets the current offset of the text caret in an #AtspiText object.
 *
 * Returns: a #gint indicating the current position of the text caret.
 **/
gint
atspi_text_get_caret_offset (AtspiText *obj, GError **error)
{
  dbus_int32_t retval = -1;

  g_return_val_if_fail (obj != NULL, -1);

  _atspi_dbus_get_property (obj, atspi_interface_text, "CaretOffset", error, "i", &retval);

  return retval;
}

/**
 * atspi_text_get_attributes: (rename-to atspi_text_get_text_attributes)
 * @obj: a pointer to the #AtspiText object to query.
 * @offset: a #gint indicating the offset from which the attribute
 *        search is based.
 * @start_offset: (out): a #gint pointer indicating the start of the desired text
 *                range.
 * @end_offset: (out): a #gint pointer indicating the first character past the desired
 *              range.
 *
 * Gets the attributes applied to a range of text from an #AtspiText
 * object. The text attributes correspond to CSS attributes
 * where possible.
 * <em>DEPRECATED</em>
 *
 * Returns: (element-type gchar* gchar*) (transfer full): a #GHashTable
 * describing the attributes at the given character offset.
 *
 * Deprecated: 2.10: Use atspi_text_get_text_attributes instead.
 **/
GHashTable *
atspi_text_get_attributes (AtspiText *obj,
			   gint offset,
			   gint *start_offset,
			   gint *end_offset,
			   GError **error)
{
  return atspi_text_get_text_attributes (obj, offset, start_offset, end_offset, error);
}

/**
 * atspi_text_get_text_attributes:
 * @obj: a pointer to the #AtspiText object to query.
 * @offset: a #gint indicating the offset from which the attribute
 *        search is based.
 * @start_offset: (out): a #gint pointer indicating the start of the desired text
 *                range.
 * @end_offset: (out): a #gint pointer indicating the first character past the desired
 *              range.
 *
 * Gets the attributes applied to a range of text from an #AtspiText
 * object. The text attributes correspond to CSS attributes
 * where possible.
 * <em>DEPRECATED</em>
 *
 * Returns: (element-type gchar* gchar*) (transfer full): a #GHashTable
 * describing the attributes at the given character offset.
 **/
GHashTable *
atspi_text_get_text_attributes (AtspiText *obj,
			   gint offset,
			   gint *start_offset,
			   gint *end_offset,
			   GError **error)
{
  dbus_int32_t d_offset = offset;
  dbus_int32_t d_start_offset, d_end_offset;
  DBusMessage *reply;
  DBusMessageIter iter;
  GHashTable *ret = NULL;

  if (obj == NULL)
   return NULL;

  reply = _atspi_dbus_call_partial (obj, atspi_interface_text, "GetAttributes", error, "i", d_offset);
  _ATSPI_DBUS_CHECK_SIG (reply, "a{ss}ii", error, ret)

  dbus_message_iter_init (reply, &iter);
  ret = _atspi_dbus_hash_from_iter (&iter);
  dbus_message_iter_next (&iter);

  dbus_message_iter_get_basic (&iter, &d_start_offset);
  if (start_offset)
    *start_offset = d_start_offset;
  dbus_message_iter_next (&iter);
  dbus_message_iter_get_basic (&iter, &d_end_offset);
  if (end_offset)
    *end_offset = d_end_offset;

  dbus_message_unref (reply);
  return ret;
}

/**
 * atspi_text_get_attribute_run:
 * @obj: a pointer to the #AtspiText object to query.
 * @offset: a #gint indicating the offset from which the attribute
 *        search is based.
 * @include_defaults: a #bool that, when set as #FALSE, indicates the call
 * should only return those attributes which are explicitly set on the current
 * attribute run, omitting any attributes which are inherited from the 
 * default values.
 * @start_offset: (out): a #gint pointer indicating the start of the desired text
 *                range.
 * @end_offset: (out): a #gint pointer indicating the first character past the desired
 *              range.
 *
 * Gets a set of attributes applied to a range of text from an #AtspiText object, optionally
 * including its 'default' attributes.
 *
 * Returns: (element-type gchar* gchar*) (transfer full): a #GHashTable with attributes
 *          defined at the indicated offset, optionally including the 'default' ones.
 **/
GHashTable *
atspi_text_get_attribute_run (AtspiText *obj,
			      gint offset,
			      gboolean include_defaults,
			      gint *start_offset,
			      gint *end_offset,
			      GError **error)
{
  dbus_int32_t d_offset = offset;
  dbus_int32_t d_start_offset, d_end_offset;
  DBusMessage *reply;
  DBusMessageIter iter;
  GHashTable *ret = NULL;

  if (obj == NULL)
   return NULL;

  reply = _atspi_dbus_call_partial (obj, atspi_interface_text,
                                    "GetAttributeRun", error, "ib", d_offset,
                                    include_defaults);
  _ATSPI_DBUS_CHECK_SIG (reply, "a{ss}ii", error, ret)

  dbus_message_iter_init (reply, &iter);
  ret = _atspi_dbus_hash_from_iter (&iter);
  dbus_message_iter_next (&iter);

  dbus_message_iter_get_basic (&iter, &d_start_offset);
  if (start_offset)
    *start_offset = d_start_offset;
  dbus_message_iter_next (&iter);
  dbus_message_iter_get_basic (&iter, &d_end_offset);
  if (end_offset)
    *end_offset = d_end_offset;

  dbus_message_unref (reply);
  return ret;
}

/**
 * atspi_text_get_attribute_value: (rename-to atspi_text_get_text_attribute_value)
 * @obj: a pointer to the #AtspiText object to query.
 * @offset: The character offset at which to query the attribute.
 * @attribute_name: The attribute to query.
 *
 * Gets the value of a named attribute at a given offset.
 *
 * Returns: (nullable): the value of a given attribute at the given
 * offset, or %NULL if not present.
 *
 * Deprecated: 2.10: Use atspi_text_get_text_attribute_value instead.
 **/
gchar *
atspi_text_get_attribute_value (AtspiText *obj,
                                gint offset,
                                gchar *attribute_value,
                                GError **error)
{
  return atspi_text_get_text_attribute_value (obj, offset, attribute_value,
                                              error);
}

/**
 * atspi_text_get_text_attribute_value:
 * @obj: a pointer to the #AtspiText object to query.
 * @offset: The character offset at which to query the attribute.
 * @attribute_name: The attribute to query.
 *
 * Gets the value of a named attribute at a given offset.
 *
 * Returns: (nullable): the value of a given attribute at the given offset, or %NULL if
 * not present.
 **/
gchar *
atspi_text_get_text_attribute_value (AtspiText *obj,
                                     gint offset,
                                     gchar *attribute_value,
                                     GError **error)
{
  gchar *retval = NULL;
  dbus_int32_t d_i = offset;

  g_return_val_if_fail (obj != NULL, NULL);

  _atspi_dbus_call (obj, atspi_interface_text, "GetAttributeValue", error, "is=>s", d_i, (const gchar *)attribute_value, &retval);

  if (!retval)
    retval = g_strdup ("");

  return retval;
}

/**
 * atspi_text_get_default_attributes:
 * @obj: a pointer to the #AtspiText object to query.
 *
 * Gets the default attributes applied to an #AtspiText
 * object. The text attributes correspond to CSS attributes 
 * where possible. The combination of this attribute set and
 * the attributes reported by #atspi_text_get_attributes
 * describes the entire set of text attributes over a range.
 *
 * Returns: (element-type gchar* gchar*) (transfer full): a #GHashTable
 *          containing the default attributes applied to a text object,
 *          (exclusive of explicitly-set attributes), encoded as UTF-8.
 **/
GHashTable *
atspi_text_get_default_attributes (AtspiText *obj, GError **error)
{
  DBusMessage *reply;

    g_return_val_if_fail (obj != NULL, NULL);

  reply = _atspi_dbus_call_partial (obj, atspi_interface_text, "GetDefaultAttributes", error, "");
  return _atspi_dbus_return_hash_from_message (reply);
}


/**
 * atspi_text_set_caret_offset:
 * @obj: a pointer to the #AtspiText object on which to operate.
 * @new_offset: the offset to which the text caret is to be moved.
 *
 * Moves the text caret to a given position.
 *
 * Returns: #TRUE if successful, #FALSE otherwise.
 **/
gboolean
atspi_text_set_caret_offset (AtspiText *obj,
                               gint new_offset,
                               GError **error)
{
  dbus_int32_t d_new_offset = new_offset;
  dbus_bool_t retval = FALSE;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_text, "SetCaretOffset", error, "i=>b", d_new_offset, &retval);

  return retval;
}

/**
 * atspi_text_get_text_before_offset:
 * @obj: a pointer to the #AtspiText object on which to operate.
 * @offset: a #gint indicating the offset from which the delimiter
 *        search is based.
 * @type: an #AtspiTextBoundaryType indicating whether the desired
 *       text string is a word, sentence, line, or attribute run.
 *
 * Gets delimited text from an #AtspiText object which precedes a given
 *          text offset.
 *
 * Returns: an #AtspiTextRange containing a UTF-8 string representing the
 *          delimited text, both of whose delimiting boundaries are before the
 *          current offset, or an empty string if no such text exists.
 **/
AtspiTextRange *
atspi_text_get_text_before_offset (AtspiText *obj,
                                    gint offset,
                                    AtspiTextBoundaryType type,
                                    GError **error)
{
  dbus_int32_t d_offset = offset;
  dbus_uint32_t d_type = type;
  dbus_int32_t d_start_offset = -1, d_end_offset = -1;
  AtspiTextRange *range = g_new0 (AtspiTextRange, 1);

  range->start_offset = range->end_offset = -1;
  if (!obj)
    return range;

  _atspi_dbus_call (obj, atspi_interface_text, "GetTextBeforeOffset", error,
                    "iu=>sii", d_offset, d_type, &range->content,
                    &d_start_offset, &d_end_offset);

  range->start_offset = d_start_offset;
  range->end_offset = d_end_offset;
  if (!range->content)
    range->content = g_strdup ("");

  return range;
}

/**
 * atspi_text_get_string_at_offset:
 * @obj: an #AtspiText
 * @offset: position
 * @granularity: An #AtspiTextGranularity
 *
 * Gets a portion of the text exposed through an #AtspiText according to a given @offset
 * and a specific @granularity, along with the start and end offsets defining the
 * boundaries of such a portion of text.
 *
 * If @granularity is ATSPI_TEXT_GRANULARITY_CHAR the character at the
 * offset is returned.
 *
 * If @granularity is ATSPI_TEXT_GRANULARITY_WORD the returned string
 * is from the word start at or before the offset to the word start after
 * the offset.
 *
 * The returned string will contain the word at the offset if the offset
 * is inside a word and will contain the word before the offset if the
 * offset is not inside a word.
 *
 * If @granularity is ATSPI_TEXT_GRANULARITY_SENTENCE the returned string
 * is from the sentence start at or before the offset to the sentence
 * start after the offset.
 *
 * The returned string will contain the sentence at the offset if the offset
 * is inside a sentence and will contain the sentence before the offset
 * if the offset is not inside a sentence.
 *
 * If @granularity is ATSPI_TEXT_GRANULARITY_LINE the returned string
 * is from the line start at or before the offset to the line
 * start after the offset.
 *
 * If @granularity is ATSPI_TEXT_GRANULARITY_PARAGRAPH the returned string
 * is from the start of the paragraph at or before the offset to the start
 * of the following paragraph after the offset.
 *
 * Since: 2.9.90
 *
 * Returns: a newly allocated string containing the text at the @offset bounded
 *   by the specified @granularity. Use g_free() to free the returned string.
 *   Returns %NULL if the offset is invalid or no implementation is available.
 **/
AtspiTextRange *
atspi_text_get_string_at_offset (AtspiText *obj,
                                 gint offset,
                                 AtspiTextGranularity granularity,
                                 GError **error)
{
  dbus_int32_t d_offset = offset;
  dbus_uint32_t d_granularity = granularity;
  dbus_int32_t d_start_offset = -1, d_end_offset = -1;
  AtspiTextRange *range = g_new0 (AtspiTextRange, 1);

  range->start_offset = range->end_offset = -1;
  if (!obj)
    return range;

  _atspi_dbus_call (obj, atspi_interface_text, "GetStringAtOffset", error,
                    "iu=>sii", d_offset, d_granularity, &range->content,
                    &d_start_offset, &d_end_offset);

  range->start_offset = d_start_offset;
  range->end_offset = d_end_offset;
  if (!range->content)
    range->content = g_strdup ("");

  return range;
}

/**
 * atspi_text_get_text_at_offset:
 * @obj: a pointer to the #AtspiText object on which to operate.
 * @offset: a #gint indicating the offset from which the delimiter
 *        search is based.
 * @type: an #AtspiTextBoundaryType indicating whether the desired
 *       text string is a word, sentence, line, or attribute run.
 *
 * Gets delimited text from an #AtspiText object which includes a given
 *          text offset.
 *
 * Returns: an #AtspiTextRange containing a UTF-8 string representing the
 *          delimited text, whose delimiting boundaries bracket the
 *          current offset, or an empty string if no such text exists.
 *
 * Deprecated: 2.10: Use atspi_text_get_string_at_offset.
 **/
AtspiTextRange *
atspi_text_get_text_at_offset (AtspiText *obj,
                                    gint offset,
                                    AtspiTextBoundaryType type,
                                    GError **error)
{
  dbus_int32_t d_offset = offset;
  dbus_uint32_t d_type = type;
  dbus_int32_t d_start_offset = -1, d_end_offset = -1;
  AtspiTextRange *range = g_new0 (AtspiTextRange, 1);

  range->start_offset = range->end_offset = -1;
  if (!obj)
    return range;

  _atspi_dbus_call (obj, atspi_interface_text, "GetTextAtOffset", error,
                    "iu=>sii", d_offset, d_type, &range->content,
                    &d_start_offset, &d_end_offset);

  range->start_offset = d_start_offset;
  range->end_offset = d_end_offset;
  if (!range->content)
    range->content = g_strdup ("");

  return range;
}

/**
 * atspi_text_get_text_after_offset:
 * @obj: a pointer to the #AtspiText object on which to operate.
 * @offset: a #gint indicating the offset from which the delimiter
 *        search is based.
 * @type: an #AtspiTextBoundaryType indicating whether the desired
 *       text string is a word, sentence, line, or attribute run.
 *
 * Gets delimited text from an #AtspiText object which follows a given
 *          text offset.
 *
 * Returns: an #AtspiTextRange containing a UTF-8 string representing the
 *          delimited text, both of whose delimiting boundaries are after or
 *          inclusive of the current offset, or an empty string if no such
 *          text exists.
 **/
AtspiTextRange *
atspi_text_get_text_after_offset (AtspiText *obj,
                                    gint offset,
                                    AtspiTextBoundaryType type,
                                    GError **error)
{
  dbus_int32_t d_offset = offset;
  dbus_uint32_t d_type = type;
  dbus_int32_t d_start_offset = -1, d_end_offset = -1;
  AtspiTextRange *range = g_new0 (AtspiTextRange, 1);

  range->start_offset = range->end_offset = -1;
  if (!obj)
    return range;

  _atspi_dbus_call (obj, atspi_interface_text, "GetTextAfterOffset", error,
                    "iu=>sii", d_offset, d_type, &range->content,
                    &d_start_offset, &d_end_offset);

  range->start_offset = d_start_offset;
  range->end_offset = d_end_offset;
  if (!range->content)
    range->content = g_strdup ("");

  return range;
}

/**
 * atspi_text_get_character_at_offset:
 * @obj: a pointer to the #AtspiText object on which to operate.
 * @offset: a #gint indicating the text offset where the desired
 *          character is located.
 *
 * Gets the character at a given offset for an #AtspiText object.
 *
 * Returns: a #guint  representing the
 *        UCS-4 unicode code point of the given character, or
 *        0xFFFFFFFF if the character in question cannot be represented
 *        in the UCS-4 encoding.
 **/
guint
atspi_text_get_character_at_offset (AtspiText *obj,
                                     gint offset,
                                     GError **error)
{
  dbus_int32_t d_offset = offset;
  dbus_int32_t retval = -1;

  g_return_val_if_fail (obj != NULL, -1);

  _atspi_dbus_call (obj, atspi_interface_text, "GetCharacterAtOffset", error, "i=>i", d_offset, &retval);

  return retval;
}

/**
 * atspi_text_get_character_extents:
 * @obj: a pointer to the #AtspiText object on which to operate.
 * @offset: a #gint indicating the offset of the text character for
 *        whom boundary information is requested.
 * @type: an #AccessibleCoordType indicating the coordinate system to use
 *        for the returned values.
 *
 * Gets a bounding box containing the glyph representing
 *        the character at a particular text offset.
 *
 * Returns: An #AtspiRect specifying the position and size of the character.
 *
 **/
AtspiRect *
atspi_text_get_character_extents (AtspiText *obj,
                                    gint offset,
				    AtspiCoordType type,
				    GError **error)
{
  dbus_int32_t d_offset = offset;
  dbus_uint32_t d_type = type;
  dbus_int32_t d_x, d_y, d_width, d_height;
  AtspiRect ret;

  ret.x = ret.y = ret.width = ret.height = -1;

  if (obj == NULL)
    return atspi_rect_copy (&ret);

  _atspi_dbus_call (obj, atspi_interface_text, "GetCharacterExtents", error, "iu=>iiii", d_offset, d_type, &d_x, &d_y, &d_width, &d_height);

  ret.x = d_x;
  ret.y = d_y;
  ret.width = d_width;
  ret.height = d_height;
  return atspi_rect_copy (&ret);
}

/**
 * atspi_text_get_offset_at_point:
 * @obj: a pointer to the #AtspiText object on which to operate.
 * @x: the x coordinate of the point to be queried.
 * @y: the y coordinate of the point to be queried.
 * @type: an #AtspiCoordType indicating the coordinate system in which
 *       the values should be returned.
 *
 * Gets the character offset into the text at a given point.
 *
 * Returns: the offset (as a #gint) at the point (@x, @y)
 *       in the specified coordinate system.
 *
 **/
gint
atspi_text_get_offset_at_point (AtspiText *obj,
                                 gint x,
                                 gint y,
				 AtspiCoordType type,
				 GError **error)
{
  dbus_int32_t d_x = x, d_y = y;
  dbus_uint32_t d_type = type;
  dbus_int32_t retval = -1;

  g_return_val_if_fail (obj != NULL, -1);

  _atspi_dbus_call (obj, atspi_interface_text, "GetOffsetAtPoint", error, "iiu=>i", d_x, d_y, d_type, &retval);

  return retval;
}

/**
 * atspi_text_get_range_extents:
 * @obj: a pointer to the #AtspiText object on which to operate.
 * @start_offset: a #gint indicating the offset of the first text character for
 *        whom boundary information is requested.
 * @end_offset: a #gint indicating the offset of the text character
 *        after the last character for whom boundary information is requested.
 * @type: an #AtspiCoordType indicating the coordinate system to use
 *        for the returned values.
 *
 * Gets the bounding box for text within a range in an  #AtspiText object.
 *
 * Returns: An #AtspiRect giving the position and size of the specified range
 *          of text.
 *
 **/
AtspiRect *
atspi_text_get_range_extents (AtspiText *obj,
				gint start_offset,
				gint end_offset,
				AtspiCoordType type,
				GError **error)
{
  dbus_int32_t d_start_offset = start_offset, d_end_offset = end_offset;
  dbus_uint32_t d_type = type;
  dbus_int32_t d_x, d_y, d_width, d_height;
  AtspiRect ret;

  ret.x = ret.y = ret.width = ret.height = -1;

  if (obj == NULL)
    return atspi_rect_copy (&ret);

  _atspi_dbus_call (obj, atspi_interface_text, "GetRangeExtents", error, "iiu=>iiii", d_start_offset, d_end_offset, d_type, &d_x, &d_y, &d_width, &d_height);

  ret.x = d_x;
  ret.y = d_y;
  ret.width = d_width;
  ret.height = d_height;
  return atspi_rect_copy (&ret);
}

/**
 * atspi_text_get_bounded_ranges:
 * @obj: a pointer to the #AtspiText object on which to operate.
 * @x: the 'starting' x coordinate of the bounding box.
 * @y: the 'starting' y coordinate of the bounding box.
 * @width: the x extent of the bounding box.
 * @height: the y extent of the bounding box.
 * @type: an #AccessibleCoordType indicating the coordinate system to use
 *        for the returned values.
 * @clipTypeX: an #AtspiTextClipType indicating how to treat characters that
 *        intersect the bounding box's x extents.
 * @clipTypeY: an #AtspiTextClipType indicating how to treat characters that
 *        intersect the bounding box's y extents.
 *
 * Gets the ranges of text from an #AtspiText object which lie within the
 *          bounds defined by (@x, @y) and (@x+@width, @y+@height).
 *
 * Returns: (transfer full) (element-type AtspiTextRange*): a null-terminated list of
 *          pointers to #AtspiTextRange structs detailing the bounded text.
 **/
GArray *
atspi_text_get_bounded_ranges (AtspiText *obj,
				 gint x,
				 gint y,
				 gint width,
				 gint height,
				 AtspiCoordType type,
				 AtspiTextClipType clipTypeX,
				 AtspiTextClipType clipTypeY,
				 GError **error)
{
  dbus_int32_t d_x = x, d_y = y, d_width = width, d_height = height;
  dbus_uint32_t d_type = type;
  dbus_uint32_t d_clipTypeX = clipTypeX, d_clipTypeY = clipTypeY;
  GArray *range_seq = NULL;

  g_return_val_if_fail (obj != NULL, NULL);

  _atspi_dbus_call (obj, atspi_interface_text, "GetBoundedRanges", error, "iiiiuuu=>a(iisv)", d_x, d_y, d_width, d_height, d_type, d_clipTypeX, d_clipTypeY, &range_seq);

  return range_seq;
}

/**
 * atspi_text_get_n_selections:
 * @obj: a pointer to the #AtspiText object on which to operate.
 *
 * Gets the number of active non-contiguous selections for an
 *          #AtspiText object.
 *
 * Returns: a #gint indicating the current
 *          number of non-contiguous text selections active
 *          within an #AtspiText object.
 **/
gint
atspi_text_get_n_selections (AtspiText *obj, GError **error)
{
  dbus_int32_t retval = 0;

  g_return_val_if_fail (obj != NULL, -1);

  _atspi_dbus_call (obj, atspi_interface_text, "GetNSelections", error, "=>i", &retval);

  return retval;
}

/**
 * atspi_text_get_selection:
 * @obj: a pointer to the #AtspiText object on which to operate.
 * @selection_num: a #gint indicating which selection to query.
 *
 * Gets the bounds of the @selection_num-th active text selection for an
 *         #AtspiText object.
 **/
AtspiRange *
atspi_text_get_selection (AtspiText *obj,
			     gint selection_num,
			     GError **error)
{
  dbus_int32_t d_selection_num = selection_num;
  dbus_int32_t d_start_offset, d_end_offset;
  AtspiRange *ret = g_new (AtspiRange, 1);

  ret->start_offset = ret->end_offset = -1;

  if (!obj)
    return ret;

  _atspi_dbus_call (obj, atspi_interface_text, "GetSelection", error, "i=>ii", d_selection_num, &d_start_offset, &d_end_offset);

  ret->start_offset = d_start_offset;
  ret->end_offset = d_end_offset;
  return ret;
}

/**
 * atspi_text_add_selection:
 * @obj: a pointer to the #AtspiText object on which to operate.
 * @start_offset: the starting offset of the desired new selection.
 * @end_offset: the offset of the first character after the new selection.
 *
 * Selects some text (adds a text selection) in an #AtspiText object.
 *
 * Returns: #TRUE if successful, #FALSE otherwise.
 **/
gboolean
atspi_text_add_selection (AtspiText *obj,
			     gint start_offset, gint end_offset,
			     GError **error)
{
  dbus_int32_t d_start_offset = start_offset, d_end_offset = end_offset;
  dbus_bool_t retval = FALSE;

  _atspi_dbus_call (obj, atspi_interface_text, "AddSelection", error, "ii=>b", d_start_offset, d_end_offset, &retval);

  return retval;
}

/**
 * atspi_text_remove_selection:
 * @obj: a pointer to the #AtspiText object on which to operate.
 * @selection_num: a #gint indicating which text selection to remove.
 *
 * De-selects a text selection.
 *
 * Returns: #TRUE if successful, #FALSE otherwise.
 **/
gboolean
atspi_text_remove_selection (AtspiText *obj,
				gint selection_num,
				GError **error)
{
  dbus_int32_t d_selection_num = selection_num;
  dbus_bool_t retval = FALSE;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_text, "RemoveSelection", error, "i=>b", d_selection_num, &retval);

  return retval;
}

/**
 * atspi_text_set_selection:
 * @obj: a pointer to the #AtspiText object on which to operate.
 * @selection_num: a zero-offset index indicating which text selection to modify.
 * @start_offset: a #gint indicating the new starting offset for the selection.
 * @end_offset: a #gint indicating the desired new offset of the first character
 *             after the selection.
 *
 * Changes the bounds of an existing #AtspiText text selection.
 *
 * Returns: #TRUE if successful, #FALSE otherwise.
 **/
gboolean
atspi_text_set_selection (AtspiText *obj,
			     gint selection_num,
			     gint start_offset,
			     gint end_offset,
			     GError **error)
{
  dbus_int32_t d_selection_num = selection_num, d_start_offset = start_offset, d_end_offset = end_offset;
  dbus_bool_t retval = FALSE;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_text, "SetSelection", error, "iii=>b", d_selection_num, d_start_offset, d_end_offset, &retval);

  return retval;
}

static void
atspi_text_base_init (AtspiText *klass)
{
}

GType
atspi_text_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo =
    {
      sizeof (AtspiText),
      (GBaseInitFunc) atspi_text_base_init,
      (GBaseFinalizeFunc) NULL,
    };

    type = g_type_register_static (G_TYPE_INTERFACE, "AtspiText", &tinfo, 0);

  }
  return type;
}
