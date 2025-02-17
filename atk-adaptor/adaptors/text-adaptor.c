/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
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

#include <string.h>

#define ATK_DISABLE_DEPRECATION_WARNINGS
#include "bridge.h"
#include <atk/atk.h>
#include <droute/droute.h>

#include "spi-dbus.h"

#include "introspection.h"
#include "object.h"

static dbus_bool_t
impl_get_Version (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_uint32 (iter, SPI_DBUS_TEXT_VERSION);
}

static dbus_bool_t
impl_get_CharacterCount (DBusMessageIter *iter, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  g_return_val_if_fail (ATK_IS_TEXT (user_data), FALSE);
  return droute_return_v_int32 (iter, atk_text_get_character_count (text));
}

static dbus_bool_t
impl_get_CaretOffset (DBusMessageIter *iter, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  g_return_val_if_fail (ATK_IS_TEXT (user_data), FALSE);
  return droute_return_v_int32 (iter, atk_text_get_caret_offset (text));
}

static gchar *
validate_allocated_string (gchar *str)
{
  if (!str)
    return g_strdup ("");
  if (!g_utf8_validate (str, -1, NULL))
    {
      g_warning ("atk-bridge: received bad UTF-8 string from a get_text function");
      g_free (str);
      return g_strdup ("");
    }
  return str;
}

static DBusMessage *
impl_GetText (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t startOffset, endOffset;
  gchar *txt;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &startOffset, DBUS_TYPE_INT32,
                              &endOffset, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  txt = atk_text_get_text (text, startOffset, endOffset);
  txt = validate_allocated_string (txt);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &txt,
                                DBUS_TYPE_INVALID);
    }
  g_free (txt);
  return reply;
}

static DBusMessage *
impl_SetCaretOffset (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t offset;
  dbus_bool_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &offset, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  rv = atk_text_set_caret_offset (text, offset);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetTextBeforeOffset (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t offset;
  dbus_uint32_t type;
  gchar *txt;
  dbus_int32_t startOffset, endOffset;
  gint intstart_offset = 0, intend_offset = 0;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &offset, DBUS_TYPE_UINT32, &type,
                              DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  txt =
      atk_text_get_text_before_offset (text, offset, (AtkTextBoundary) type,
                                       &intstart_offset, &intend_offset);
  startOffset = intstart_offset;
  endOffset = intend_offset;
  txt = validate_allocated_string (txt);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &txt,
                                DBUS_TYPE_INT32, &startOffset,
                                DBUS_TYPE_INT32, &endOffset,
                                DBUS_TYPE_INVALID);
    }
  g_free (txt);
  return reply;
}

static DBusMessage *
impl_GetTextAtOffset (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t offset, type;
  gchar *txt;
  dbus_int32_t startOffset, endOffset;
  gint intstart_offset = 0, intend_offset = 0;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &offset, DBUS_TYPE_UINT32, &type,
                              DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  txt =
      atk_text_get_text_at_offset (text, offset, (AtkTextBoundary) type,
                                   &intstart_offset, &intend_offset);
  startOffset = intstart_offset;
  endOffset = intend_offset;
  txt = validate_allocated_string (txt);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &txt,
                                DBUS_TYPE_INT32, &startOffset,
                                DBUS_TYPE_INT32, &endOffset,
                                DBUS_TYPE_INVALID);
    }
  g_free (txt);
  return reply;
}

static DBusMessage *
impl_GetTextAfterOffset (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t offset;
  dbus_uint32_t type;
  gchar *txt;
  dbus_int32_t startOffset, endOffset;
  gint intstart_offset = 0, intend_offset = 0;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &offset, DBUS_TYPE_UINT32, &type,
                              DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  txt =
      atk_text_get_text_after_offset (text, offset, (AtkTextBoundary) type,
                                      &intstart_offset, &intend_offset);
  startOffset = intstart_offset;
  endOffset = intend_offset;
  txt = validate_allocated_string (txt);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &txt,
                                DBUS_TYPE_INT32, &startOffset,
                                DBUS_TYPE_INT32, &endOffset,
                                DBUS_TYPE_INVALID);
    }
  g_free (txt);
  return reply;
}

static DBusMessage *
impl_GetCharacterAtOffset (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t offset;
  dbus_int32_t ch;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &offset, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  ch = atk_text_get_character_at_offset (text, offset);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &ch,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static gchar *
get_text_for_legacy_implementations (AtkText *text,
                                     gint offset,
                                     AtkTextGranularity granularity,
                                     gint *start_offset,
                                     gint *end_offset)
{
  gchar *txt = 0;
  AtkTextBoundary boundary = 0;
  switch (granularity)
    {
    case ATK_TEXT_GRANULARITY_CHAR:
      boundary = ATK_TEXT_BOUNDARY_CHAR;
      break;

    case ATK_TEXT_GRANULARITY_WORD:
      boundary = ATK_TEXT_BOUNDARY_WORD_START;
      break;

    case ATK_TEXT_GRANULARITY_SENTENCE:
      boundary = ATK_TEXT_BOUNDARY_SENTENCE_START;
      break;

    case ATK_TEXT_GRANULARITY_LINE:
      boundary = ATK_TEXT_BOUNDARY_LINE_START;
      break;

    case ATK_TEXT_GRANULARITY_PARAGRAPH:
      /* This is not implemented in previous versions of ATK */
      txt = g_strdup ("");
      break;

    default:
      g_assert_not_reached ();
    }

  if (!txt)
    {
      txt =
          atk_text_get_text_at_offset (text, offset, boundary,
                                       start_offset, end_offset);
    }

  return txt;
}

static DBusMessage *
impl_GetStringAtOffset (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t offset;
  dbus_uint32_t granularity;
  gchar *txt = 0;
  dbus_int32_t startOffset, endOffset;
  gint intstart_offset = 0, intend_offset = 0;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &offset, DBUS_TYPE_UINT32, &granularity,
                              DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }

  txt =
      atk_text_get_string_at_offset (text, offset, (AtkTextGranularity) granularity,
                                     &intstart_offset, &intend_offset);

  /* Accessibility layers implementing an older version of ATK (even if
   * a new enough version of libatk is installed) might return NULL due
   * not to provide an implementation for get_string_at_offset(), so we
   * try with the legacy implementation if that's the case. */
  if (!txt)
    txt = get_text_for_legacy_implementations (text, offset,
                                               (AtkTextGranularity) granularity,
                                               &intstart_offset, &intend_offset);

  startOffset = intstart_offset;
  endOffset = intend_offset;
  txt = validate_allocated_string (txt);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &txt,
                                DBUS_TYPE_INT32, &startOffset,
                                DBUS_TYPE_INT32, &endOffset,
                                DBUS_TYPE_INVALID);
    }
  g_free (txt);
  return reply;
}

static DBusMessage *
impl_GetAttributeValue (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t offset;
  char *attributeName;
  gint intstart_offset = 0, intend_offset = 0;
  char *rv = NULL;
  DBusMessage *reply;
  AtkAttributeSet *set;
  GSList *cur_attr;
  AtkAttribute *at;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &offset, DBUS_TYPE_STRING,
                              &attributeName, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }

  set = atk_text_get_run_attributes (text, offset,
                                     &intstart_offset, &intend_offset);
  cur_attr = (GSList *) set;
  while (cur_attr)
    {
      at = (AtkAttribute *) cur_attr->data;
      if (!strcmp (at->name, attributeName))
        {
          rv = at->value;
          break;
        }
      cur_attr = cur_attr->next;
    }
  if (!rv)
    rv = "";
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &rv, DBUS_TYPE_INVALID);
    }
  atk_attribute_set_free (set);
  return reply;
}

static DBusMessage *
impl_GetAttributes (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t offset;
  dbus_int32_t startOffset, endOffset;
  gint intstart_offset, intend_offset;
  DBusMessage *reply;
  AtkAttributeSet *set;
  DBusMessageIter iter;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &offset, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }

  set = atk_text_get_run_attributes (text, offset,
                                     &intstart_offset, &intend_offset);

  startOffset = intstart_offset;
  endOffset = intend_offset;
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_iter_init_append (reply, &iter);
      spi_object_append_attribute_set (&iter, set);
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &startOffset,
                                DBUS_TYPE_INT32, &endOffset,
                                DBUS_TYPE_INVALID);
    }
  atk_attribute_set_free (set);
  return reply;
}

static DBusMessage *
impl_GetDefaultAttributes (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  DBusMessage *reply;
  AtkAttributeSet *set;
  DBusMessageIter iter;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));

  set = atk_text_get_default_attributes (text);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_iter_init_append (reply, &iter);
      spi_object_append_attribute_set (&iter, set);
    }
  atk_attribute_set_free (set);
  return reply;
}

static DBusMessage *
impl_GetCharacterExtents (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t offset;
  dbus_uint32_t coordType;
  dbus_int32_t x, y, width, height;
  gint ix = 0, iy = 0, iw = 0, ih = 0;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &offset, DBUS_TYPE_UINT32,
                              &coordType, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  atk_text_get_character_extents (text, offset, &ix, &iy, &iw, &ih,
                                  (AtkCoordType) coordType);
  x = ix;
  y = iy;
  width = iw;
  height = ih;
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32,
                                &y, DBUS_TYPE_INT32, &width, DBUS_TYPE_INT32,
                                &height, DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetOffsetAtPoint (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t x, y;
  dbus_uint32_t coordType;
  dbus_int32_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32, &y,
                              DBUS_TYPE_UINT32, &coordType, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  rv = atk_text_get_offset_at_point (text, x, y, coordType);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &rv,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetNSelections (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  rv = atk_text_get_n_selections (text);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &rv,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetSelection (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t selectionNum;
  dbus_int32_t startOffset, endOffset;
  gint intstart_offset = 0, intend_offset = 0;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &selectionNum, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  /* atk_text_get_selection returns gchar * which we discard */
  g_free (atk_text_get_selection (text, selectionNum, &intstart_offset, &intend_offset));
  startOffset = intstart_offset;
  endOffset = intend_offset;
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &startOffset,
                                DBUS_TYPE_INT32, &endOffset,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_AddSelection (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t startOffset, endOffset;
  dbus_bool_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &startOffset, DBUS_TYPE_INT32,
                              &endOffset, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  rv = atk_text_add_selection (text, startOffset, endOffset);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_RemoveSelection (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t selectionNum;
  dbus_bool_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &selectionNum, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  rv = atk_text_remove_selection (text, selectionNum);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_SetSelection (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t selectionNum, startOffset, endOffset;
  dbus_bool_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &selectionNum, DBUS_TYPE_INT32,
                              &startOffset, DBUS_TYPE_INT32, &endOffset, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  rv = atk_text_set_selection (text, selectionNum, startOffset, endOffset);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetRangeExtents (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t startOffset, endOffset;
  dbus_uint32_t coordType;
  AtkTextRectangle rect;
  dbus_int32_t x, y, width, height;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &startOffset, DBUS_TYPE_INT32,
                              &endOffset, DBUS_TYPE_UINT32, &coordType, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  memset (&rect, 0, sizeof (rect));
  atk_text_get_range_extents (text, startOffset, endOffset,
                              (AtkCoordType) coordType, &rect);
  x = rect.x;
  y = rect.y;
  width = rect.width;
  height = rect.height;
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32,
                                &y, DBUS_TYPE_INT32, &width, DBUS_TYPE_INT32,
                                &height, DBUS_TYPE_INVALID);
    }
  return reply;
}

#define MAXRANGELEN 512

static DBusMessage *
impl_GetBoundedRanges (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t x, y, width, height;
  dbus_uint32_t coordType, xClipType, yClipType;
  AtkTextRange **range_list = NULL;
  AtkTextRectangle rect;
  DBusMessage *reply;
  DBusMessageIter iter, array, struc, variant;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32, &y,
                              DBUS_TYPE_INT32, &height, DBUS_TYPE_INT32, &width, DBUS_TYPE_UINT32,
                              &coordType, DBUS_TYPE_UINT32, &xClipType, DBUS_TYPE_UINT32, &yClipType,
                              DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  rect.x = x;
  rect.y = y;
  rect.width = width;
  rect.height = height;

  range_list =
      atk_text_get_bounded_ranges (text, &rect, (AtkCoordType) coordType,
                                   (AtkTextClipType) xClipType,
                                   (AtkTextClipType) yClipType);
  reply = dbus_message_new_method_return (message);
  if (!reply)
    return NULL;
  /* This isn't pleasant. */
  dbus_message_iter_init_append (reply, &iter);
  if (dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "(iisv)", &array))
    {
      int len;
      int count = (range_list ? MAXRANGELEN : 0);
      for (len = 0; len < count && range_list[len]; ++len)
        {
          if (dbus_message_iter_open_container (&array, DBUS_TYPE_STRUCT, NULL, &struc))
            {
              dbus_int32_t val;
              val = range_list[len]->start_offset;
              dbus_message_iter_append_basic (&struc, DBUS_TYPE_INT32, &val);
              val = range_list[len]->end_offset;
              dbus_message_iter_append_basic (&struc, DBUS_TYPE_INT32, &val);
              dbus_message_iter_append_basic (&struc, DBUS_TYPE_STRING,
                                              &range_list[len]->content);
              /* The variant is unimplemented in atk, but I don't want to
               * unilaterally muck with the spec and remove it, so I'll just
               * throw in a dummy value */
              if (dbus_message_iter_open_container (&struc, DBUS_TYPE_VARIANT, "i", &variant))
                {
                  dbus_uint32_t dummy = 0;
                  dbus_message_iter_append_basic (&variant, DBUS_TYPE_INT32,
                                                  &dummy);
                  dbus_message_iter_close_container (&struc, &variant);
                }
              dbus_message_iter_close_container (&array, &struc);
              g_free (range_list[len]->content);
              g_free (range_list[len]);
            }
        }
      dbus_message_iter_close_container (&iter, &array);
    }

  if (range_list)
    g_free (range_list);

  return reply;
}

static DBusMessage *
impl_GetAttributeRun (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t offset;
  dbus_bool_t includeDefaults;
  dbus_int32_t startOffset, endOffset;
  gint intstart_offset = 0, intend_offset = 0;
  DBusMessage *reply;
  AtkAttributeSet *attributes = NULL;
  DBusMessageIter iter;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &offset, DBUS_TYPE_BOOLEAN,
                              &includeDefaults, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }

  if (includeDefaults)
    {
      attributes = g_slist_concat (attributes,
                                   atk_text_get_default_attributes (text));
    }

  attributes = g_slist_concat (attributes,
                               atk_text_get_run_attributes (text, offset,
                                                            &intstart_offset,
                                                            &intend_offset));

  reply = dbus_message_new_method_return (message);
  if (!reply)
    return NULL;

  dbus_message_iter_init_append (reply, &iter);
  spi_object_append_attribute_set (&iter, attributes);

  startOffset = intstart_offset;
  endOffset = intend_offset;
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &startOffset);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &endOffset);

  atk_attribute_set_free (attributes);

  return reply;
}

static DBusMessage *
impl_GetDefaultAttributeSet (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  DBusMessage *reply;
  DBusMessageIter iter;
  AtkAttributeSet *attributes;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));

  attributes = atk_text_get_default_attributes (text);

  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_iter_init_append (reply, &iter);
      spi_object_append_attribute_set (&iter, attributes);
    }

  if (attributes)
    atk_attribute_set_free (attributes);

  return reply;
}

static DBusMessage *
impl_ScrollSubstringTo (DBusConnection *bus,
                        DBusMessage *message,
                        void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t startOffset, endOffset;
  dbus_uint32_t type;
  dbus_bool_t ret;
  DBusMessage *reply = NULL;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));

  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &startOffset,
                              DBUS_TYPE_INT32, &endOffset,
                              DBUS_TYPE_UINT32, &type,
                              DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }

  ret = atk_text_scroll_substring_to (text, startOffset, endOffset, type);

  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_ScrollSubstringToPoint (DBusConnection *bus,
                             DBusMessage *message,
                             void *user_data)
{
  AtkText *text = (AtkText *) user_data;
  dbus_int32_t startOffset, endOffset;
  dbus_uint32_t type;
  dbus_int32_t x, y;
  dbus_bool_t ret;
  DBusMessage *reply = NULL;

  g_return_val_if_fail (ATK_IS_TEXT (user_data),
                        droute_not_yet_handled_error (message));

  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &startOffset,
                              DBUS_TYPE_INT32, &endOffset,
                              DBUS_TYPE_UINT32, &type,
                              DBUS_TYPE_INT32, &x,
                              DBUS_TYPE_INT32, &y,
                              DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }

  ret = atk_text_scroll_substring_to_point (text, startOffset, endOffset, type, x, y);

  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DRouteMethod methods[] = {
  { impl_GetText, "GetText" },
  { impl_SetCaretOffset, "SetCaretOffset" },
  { impl_GetTextBeforeOffset, "GetTextBeforeOffset" },
  { impl_GetTextAtOffset, "GetTextAtOffset" },
  { impl_GetTextAfterOffset, "GetTextAfterOffset" },
  { impl_GetStringAtOffset, "GetStringAtOffset" },
  { impl_GetCharacterAtOffset, "GetCharacterAtOffset" },
  { impl_GetAttributeValue, "GetAttributeValue" },
  { impl_GetAttributes, "GetAttributes" },
  { impl_GetDefaultAttributes, "GetDefaultAttributes" },
  { impl_GetCharacterExtents, "GetCharacterExtents" },
  { impl_GetOffsetAtPoint, "GetOffsetAtPoint" },
  { impl_GetNSelections, "GetNSelections" },
  { impl_GetSelection, "GetSelection" },
  { impl_AddSelection, "AddSelection" },
  { impl_RemoveSelection, "RemoveSelection" },
  { impl_SetSelection, "SetSelection" },
  { impl_GetRangeExtents, "GetRangeExtents" },
  { impl_GetBoundedRanges, "GetBoundedRanges" },
  { impl_GetAttributeRun, "GetAttributeRun" },
  { impl_GetDefaultAttributeSet, "GetDefaultAttributeSet" },
  { impl_ScrollSubstringTo, "ScrollSubstringTo" },
  { impl_ScrollSubstringToPoint, "ScrollSubstringToPoint" },
  { NULL, NULL }
};

static DRouteProperty properties[] = {
  { impl_get_CharacterCount, NULL, "CharacterCount" },
  { impl_get_CaretOffset, NULL, "CaretOffset" },
  { impl_get_Version, NULL, "version" },
  { NULL, NULL, NULL }
};

void
spi_initialize_text (DRoutePath *path)
{
  spi_atk_add_interface (path,
                         ATSPI_DBUS_INTERFACE_TEXT,
                         spi_org_a11y_atspi_Text,
                         methods, properties);
};
