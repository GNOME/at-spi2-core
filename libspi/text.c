/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
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

#include "accessible.h"
#include <string.h>

static AtkText *
get_text (DBusMessage * message)
{
  AtkObject *obj = spi_dbus_get_object (dbus_message_get_path (message));
  if (!obj)
    return NULL;
  return ATK_TEXT (obj);
}

static AtkText *
get_text_from_path (const char *path, void *user_data)
{
  AtkObject *obj = spi_dbus_get_object (path);
  if (!obj || !ATK_IS_TEXT(obj))
    return NULL;
  return ATK_TEXT (obj);
}

static dbus_bool_t
impl_get_characterCount (const char *path, DBusMessageIter * iter,
			 void *user_data)
{
  AtkText *text = get_text_from_path (path, user_data);
  if (!text)
    return FALSE;
  return droute_return_v_int32 (iter, atk_text_get_character_count (text));
}

static char *
impl_get_characterCount_str (void *datum)
{
  g_assert (ATK_IS_TEXT (datum));

  return g_strdup_printf ("%d",
			  atk_text_get_character_count ((AtkText *) datum));
}

static dbus_bool_t
impl_get_caretOffset (const char *path, DBusMessageIter * iter,
		      void *user_data)
{
  AtkText *text = get_text_from_path (path, user_data);
  if (!text)
    return FALSE;
  return droute_return_v_int32 (iter, atk_text_get_caret_offset (text));
}

static char *
impl_get_caretOffset_str (void *datum)
{
  g_assert (ATK_IS_TEXT (datum));

  return g_strdup_printf ("%d",
			  atk_text_get_caret_offset ((AtkText *) datum));
}

static DBusMessage *
impl_getText (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkText *text = get_text (message);
  dbus_int32_t startOffset, endOffset;
  gchar *txt;
  DBusError error;
  DBusMessage *reply;

  if (!text)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &startOffset, DBUS_TYPE_INT32,
       &endOffset, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  txt = atk_text_get_text (text, startOffset, endOffset);
  if (!txt)
    txt = g_strdup ("");
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
impl_setCaretOffset (DBusConnection * bus, DBusMessage * message,
		     void *user_data)
{
  AtkText *text = get_text (message);
  dbus_int32_t offset;
  dbus_bool_t rv;
  DBusError error;
  DBusMessage *reply;

  if (!text)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &offset, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
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
impl_getTextBeforeOffset (DBusConnection * bus, DBusMessage * message,
			  void *user_data)
{
  AtkText *text = get_text (message);
  dbus_int32_t offset;
  dbus_uint32_t type;
  gchar *txt;
  dbus_int32_t startOffset, endOffset;
  gint intstart_offset = 0, intend_offset = 0;
  DBusError error;
  DBusMessage *reply;

  if (!text)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &offset, DBUS_TYPE_UINT32, &type,
       DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  txt =
    atk_text_get_text_before_offset (text, offset, (AtkTextBoundary) type,
				     &intstart_offset, &intend_offset);
  startOffset = intstart_offset;
  endOffset = intend_offset;
  if (!txt)
    txt = g_strdup ("");
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &startOffset,
				DBUS_TYPE_INT32, &endOffset, DBUS_TYPE_STRING,
				&txt, DBUS_TYPE_INVALID);
    }
  g_free (txt);
  return reply;
}

static DBusMessage *
impl_getTextAtOffset (DBusConnection * bus, DBusMessage * message,
		      void *user_data)
{
  AtkText *text = get_text (message);
  dbus_int32_t offset, type;
  gchar *txt;
  dbus_int32_t startOffset, endOffset;
  gint intstart_offset = 0, intend_offset = 0;
  DBusError error;
  DBusMessage *reply;

  if (!text)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &offset, DBUS_TYPE_UINT32, &type,
       DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  txt =
    atk_text_get_text_at_offset (text, offset, (AtkTextBoundary) type,
				 &intstart_offset, &intend_offset);
  startOffset = intstart_offset;
  endOffset = intend_offset;
  if (!txt)
    txt = g_strdup ("");
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &startOffset,
				DBUS_TYPE_INT32, &endOffset, DBUS_TYPE_STRING,
				&txt, DBUS_TYPE_INVALID);
    }
  g_free (txt);
  return reply;
}

static DBusMessage *
impl_getTextAfterOffset (DBusConnection * bus, DBusMessage * message,
			 void *user_data)
{
  AtkText *text = get_text (message);
  dbus_int32_t offset;
  dbus_uint32_t type;
  gchar *txt;
  dbus_int32_t startOffset, endOffset;
  gint intstart_offset = 0, intend_offset = 0;
  DBusError error;
  DBusMessage *reply;

  if (!text)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &offset, DBUS_TYPE_UINT32, &type,
       DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  txt =
    atk_text_get_text_after_offset (text, offset, (AtkTextBoundary) type,
				    &intstart_offset, &intend_offset);
  startOffset = intstart_offset;
  endOffset = intend_offset;
  if (!txt)
    txt = g_strdup ("");
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &startOffset,
				DBUS_TYPE_INT32, &endOffset, DBUS_TYPE_STRING,
				&txt, DBUS_TYPE_INVALID);
    }
  g_free (txt);
  return reply;
}

static DBusMessage *
impl_getAttributeValue (DBusConnection * bus, DBusMessage * message,
			void *user_data)
{
  AtkText *text = get_text (message);
  dbus_int32_t offset;
  char *attributeName;
  dbus_int32_t startOffset, endOffset;
  dbus_bool_t defined;
  gint intstart_offset = 0, intend_offset = 0;
  char *rv;
  DBusError error;
  DBusMessage *reply;
  AtkAttributeSet *set;
  GSList *cur_attr;
  AtkAttribute *at;

  if (!text)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &offset, DBUS_TYPE_STRING,
       &attributeName, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }

  set = atk_text_get_run_attributes (text, offset,
				     &intstart_offset, &intend_offset);
  startOffset = intstart_offset;
  endOffset = intend_offset;
  defined = FALSE;
  cur_attr = (GSList *) set;
  while (cur_attr)
    {
      at = (AtkAttribute *) cur_attr->data;
      if (!strcmp (at->name, attributeName))
	{
	  rv = at->value;
	  defined = TRUE;
	  break;
	}
      cur_attr = cur_attr->next;
    }
  if (!rv)
    rv = "";
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &startOffset,
				DBUS_TYPE_INT32, &endOffset,
				DBUS_TYPE_BOOLEAN, &defined, DBUS_TYPE_STRING,
				&rv, DBUS_TYPE_INVALID);
    }
  atk_attribute_set_free (set);
  return reply;
}

static char *
_string_from_attribute_set (AtkAttributeSet * set)
{
  gchar *attributes, *tmp, *tmp2;
  GSList *cur_attr;
  AtkAttribute *at;

  attributes = g_strdup ("");
  cur_attr = (GSList *) set;
  while (cur_attr)
    {
      at = (AtkAttribute *) cur_attr->data;
      tmp = g_strdup_printf ("%s%s:%s%s",
			     ((GSList *) (set) == cur_attr) ? "" : " ",
			     at->name, at->value,
			     (cur_attr->next) ? ";" : "");
      tmp2 = g_strconcat (attributes, tmp, NULL);
      g_free (tmp);
      g_free (attributes);
      attributes = tmp2;
      cur_attr = cur_attr->next;
    }
  return attributes;
}

static DBusMessage *
impl_getAttributes (DBusConnection * bus, DBusMessage * message,
		    void *user_data)
{
  AtkText *text = get_text (message);
  dbus_int32_t offset;
  dbus_int32_t startOffset, endOffset;
  gint intstart_offset, intend_offset;
  char *rv;
  DBusError error;
  DBusMessage *reply;
  AtkAttributeSet *set;

  if (!text)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &offset, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }

  set = atk_text_get_run_attributes (text, offset,
				     &intstart_offset, &intend_offset);
  startOffset = intstart_offset;
  endOffset = intend_offset;
  rv = _string_from_attribute_set (set);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &startOffset,
				DBUS_TYPE_INT32, &endOffset, DBUS_TYPE_STRING,
				&rv, DBUS_TYPE_INVALID);
    }
  g_free (rv);
  atk_attribute_set_free (set);
  return reply;
}

static DBusMessage *
impl_getDefaultAttributes (DBusConnection * bus, DBusMessage * message,
			   void *user_data)
{
  AtkText *text = get_text (message);
  char *rv;
  DBusError error;
  DBusMessage *reply;
  AtkAttributeSet *set;

  if (!text)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);

  set = atk_text_get_default_attributes (text);
  rv = _string_from_attribute_set (set);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &rv,
				DBUS_TYPE_INVALID);
    }
  g_free (rv);
  atk_attribute_set_free (set);
  return reply;
}

static DBusMessage *
impl_getCharacterExtents (DBusConnection * bus, DBusMessage * message,
			  void *user_data)
{
  AtkText *text = get_text (message);
  dbus_int32_t offset;
  dbus_uint32_t coordType;
  dbus_int32_t x, y, width, height;
  gint ix = 0, iy = 0, iw = 0, ih = 0;
  DBusError error;
  DBusMessage *reply;

  if (!text)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &offset, DBUS_TYPE_INT32, &coordType,
       DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
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
impl_getOffsetAtPoint (DBusConnection * bus, DBusMessage * message,
		       void *user_data)
{
  AtkText *text = get_text (message);
  dbus_int32_t x, y;
  dbus_uint32_t coordType;
  dbus_int32_t rv;
  DBusError error;
  DBusMessage *reply;

  if (!text)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32, &y,
       DBUS_TYPE_UINT32, &coordType, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
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
impl_getNSelections (DBusConnection * bus, DBusMessage * message,
		     void *user_data)
{
  AtkText *text = get_text (message);
  dbus_int32_t rv;
  DBusMessage *reply;

  if (!text)
    return spi_dbus_general_error (message);
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
impl_getSelection (DBusConnection * bus, DBusMessage * message,
		   void *user_data)
{
  AtkText *text = get_text (message);
  dbus_int32_t selectionNum;
  dbus_int32_t startOffset, endOffset;
  gint intstart_offset = 0, intend_offset = 0;
  DBusError error;
  DBusMessage *reply;

  if (!text)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &selectionNum, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  /* atk_text_get_selection returns gchar * which we discard */
  g_free (atk_text_get_selection
	  (text, selectionNum, &intstart_offset, &intend_offset));
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
impl_addSelection (DBusConnection * bus, DBusMessage * message,
		   void *user_data)
{
  AtkText *text = get_text (message);
  dbus_int32_t startOffset, endOffset;
  dbus_bool_t rv;
  DBusError error;
  DBusMessage *reply;

  if (!text)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &startOffset, DBUS_TYPE_INT32,
       &endOffset, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
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
impl_removeSelection (DBusConnection * bus, DBusMessage * message,
		      void *user_data)
{
  AtkText *text = get_text (message);
  dbus_int32_t selectionNum;
  dbus_bool_t rv;
  DBusError error;
  DBusMessage *reply;

  if (!text)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &selectionNum, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
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
impl_setSelection (DBusConnection * bus, DBusMessage * message,
		   void *user_data)
{
  AtkText *text = get_text (message);
  dbus_int32_t selectionNum, startOffset, endOffset;
  dbus_bool_t rv;
  DBusError error;
  DBusMessage *reply;

  if (!text)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &selectionNum, DBUS_TYPE_INT32,
       &startOffset, DBUS_TYPE_INT32, &endOffset, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
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
impl_getRangeExtents (DBusConnection * bus, DBusMessage * message,
		      void *user_data)
{
  AtkText *text = get_text (message);
  dbus_int32_t startOffset, endOffset;
  dbus_uint32_t coordType;
  AtkTextRectangle rect;
  dbus_int32_t x, y, width, height;
  DBusError error;
  DBusMessage *reply;

  if (!text)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &startOffset, DBUS_TYPE_INT32,
       &endOffset, DBUS_TYPE_UINT32, &coordType, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
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
impl_getBoundedRanges (DBusConnection * bus, DBusMessage * message,
		       void *user_data)
{
  AtkText *text = get_text (message);
  dbus_int32_t x, y, width, height;
  dbus_uint32_t coordType, xClipType, yClipType;
  DBusError error;
  AtkTextRange **range_list = NULL;
  AtkTextRectangle rect;
  DBusMessage *reply;
  DBusMessageIter iter, array, struc, variant;

  if (!text)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32, &y,
       DBUS_TYPE_INT32, &height, DBUS_TYPE_INT32, &width, DBUS_TYPE_INT32,
       &coordType, DBUS_TYPE_INT32, &xClipType, DBUS_TYPE_INT32, &yClipType,
       DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
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
  if (dbus_message_iter_open_container
      (&iter, DBUS_TYPE_ARRAY, "(iisv)", &array))
    {
      int len;
      for (len = 0; len < MAXRANGELEN && range_list[len]; ++len)
	{
	  if (dbus_message_iter_open_container
	      (&array, DBUS_TYPE_STRUCT, NULL, &struc))
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
	      if (dbus_message_iter_open_container
		  (&array, DBUS_TYPE_VARIANT, "i", &variant))
		{
		  dbus_uint32_t dummy = 0;
		  dbus_message_iter_append_basic (&variant, DBUS_TYPE_INT32,
						  &dummy);
		  dbus_message_iter_close_container (&struc, &variant);
		}
	      dbus_message_iter_close_container (&array, &struc);
	    }
	}
      dbus_message_iter_close_container (&iter, &array);
    }
  return reply;
}

static DBusMessage *
impl_getAttributeRun (DBusConnection * bus, DBusMessage * message,
		      void *user_data)
{
  DBusError error;
  AtkText *text = get_text (message);
  dbus_int32_t offset;
  dbus_bool_t includeDefaults;
  dbus_int32_t startOffset, endOffset;
  gint intstart_offset = 0, intend_offset = 0;
  DBusMessage *reply;
  AtkAttributeSet *attributes, *default_attributes = NULL;
  AtkAttribute *attr = NULL;
  char **retval;
  gint n_attributes = 0, total_attributes = 0, n_default_attributes = 0;
  gint i, j;

  if (!text)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &offset, DBUS_TYPE_BOOLEAN,
       &includeDefaults, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }

  attributes =
    atk_text_get_run_attributes (text, offset, &intstart_offset,
				 &intend_offset);

  if (attributes)
    total_attributes = n_attributes = g_slist_length (attributes);

  if (includeDefaults)
    {
      default_attributes = atk_text_get_default_attributes (text);
      if (default_attributes)
	n_default_attributes = g_slist_length (default_attributes);
      total_attributes += n_default_attributes;
    }

  startOffset = intstart_offset;
  endOffset = intend_offset;

  retval = (char **) g_malloc (total_attributes * sizeof (char *));

  if (total_attributes)
    {
      for (i = 0; i < n_attributes; ++i)
	{
	  attr = g_slist_nth_data (attributes, i);
	  retval[i] = g_strconcat (attr->name, ":", attr->value, NULL);
	}

      for (j = 0; j < n_default_attributes; ++i, ++j)
	{
	  attr = g_slist_nth_data (default_attributes, j);
	  retval[i] = g_strconcat (attr->name, ":", attr->value, NULL);
	}

      atk_attribute_set_free (attributes);
      if (default_attributes)
	atk_attribute_set_free (default_attributes);
    }
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &startOffset,
				DBUS_TYPE_INT32, &endOffset, DBUS_TYPE_ARRAY,
				DBUS_TYPE_STRING, &retval, total_attributes,
				DBUS_TYPE_INVALID);
    }
  for (i = 0; i < total_attributes; i++)
    g_free (retval[i]);
  g_free (retval);
  return reply;
}

static DBusMessage *
impl_getDefaultAttributeSet (DBusConnection * bus, DBusMessage * message,
			     void *user_data)
{
  AtkText *text = get_text (message);
  DBusMessage *reply;
  AtkAttributeSet *attributes;
  AtkAttribute *attr = NULL;
  char **retval;
  gint n_attributes = 0;
  gint i;

  if (!text)
    return spi_dbus_general_error (message);

  attributes = atk_text_get_default_attributes (text);
  if (attributes)
    n_attributes = g_slist_length (attributes);

  retval = (char **) malloc (n_attributes * sizeof (char *));

  for (i = 0; i < n_attributes; ++i)
    {
      attr = g_slist_nth_data (attributes, i);
      retval[i] = g_strconcat (attr->name, ":", attr->value, NULL);
    }
  if (attributes)
    atk_attribute_set_free (attributes);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING,
				&retval, n_attributes, DBUS_TYPE_INVALID);
    }
  for (i = 0; i < n_attributes; i++)
    g_free (retval[i]);
  g_free (retval);
  return reply;
}

static DRouteMethod methods[] = {
  {DROUTE_METHOD, impl_getText, "getText",
   "i,startOffset,i:i,endOffset,i:s,,o"},
  {DROUTE_METHOD, impl_setCaretOffset, "setCaretOffset", "i,offset,i:b,,o"},
  {DROUTE_METHOD, impl_getTextBeforeOffset, "getTextBeforeOffset",
   "i,offset,i:u,type,i:i,startOffset,o:i,endOffset,o:s,,o"},
  {DROUTE_METHOD, impl_getTextAtOffset, "getTextAtOffset",
   "i,offset,i:u,type,i:i,startOffset,o:i,endOffset,o:s,,o"},
  {DROUTE_METHOD, impl_getTextAfterOffset, "getTextAfterOffset",
   "i,offset,i:u,type,i:i,startOffset,o:i,endOffset,o:s,,o"},
  {DROUTE_METHOD, impl_getAttributeValue, "getAttributeValue",
   "i,offset,i:s,attributeName,i:i,startOffset,o:i,endOffset,o:b,defined,o:s,,o"},
  {DROUTE_METHOD, impl_getAttributes, "getAttributes",
   "i,offset,i:i,startOffset,o:i,endOffset,o:s,,o"},
  {DROUTE_METHOD, impl_getDefaultAttributes, "getDefaultAttributes", "s,,o"},
  {DROUTE_METHOD, impl_getCharacterExtents, "getCharacterExtents",
   "i,offset,i:i,x,o:i,y,o:i,width,o:i,height,o:u,coordType,i"},
  {DROUTE_METHOD, impl_getOffsetAtPoint, "getOffsetAtPoint",
   "i,x,i:i,y,i:u,coordType,i:i,,o"},
  {DROUTE_METHOD, impl_getNSelections, "getNSelections", "i,,o"},
  {DROUTE_METHOD, impl_getSelection, "getSelection",
   "i,selectionNum,i:i,startOffset,o:i,endOffset,o"},
  {DROUTE_METHOD, impl_addSelection, "addSelection",
   "i,startOffset,i:i,endOffset,i:b,,o"},
  {DROUTE_METHOD, impl_removeSelection, "removeSelection",
   "i,selectionNum,i:b,,o"},
  {DROUTE_METHOD, impl_setSelection, "setSelection",
   "i,selectionNum,i:i,startOffset,i:i,endOffset,i:b,,o"},
  {DROUTE_METHOD, impl_getRangeExtents, "getRangeExtents",
   "i,startOffset,i:i,endOffset,i:i,x,o:i,y,o:i,width,o:i,height,o:u,coordType,i"},
  {DROUTE_METHOD, impl_getBoundedRanges, "getBoundedRanges",
   "i,x,i:i,y,i:i,width,i:i,height,i:u,coordType,i:u,xClipType,i:u,yClipType,i:a(iisv),,o"},
  {DROUTE_METHOD, impl_getAttributeRun, "getAttributeRun",
   "i,offset,i:i,startOffset,o:i,endOffset,o:b,includeDefaults,i:as,,o"},
  {DROUTE_METHOD, impl_getDefaultAttributeSet, "getDefaultAttributeSet",
   "as,,o"},
  {0, NULL, NULL, NULL}
};

static DRouteProperty properties[] = {
  {impl_get_characterCount, impl_get_characterCount_str, NULL, NULL,
   "characterCount", "i"},
  {impl_get_caretOffset, impl_get_caretOffset_str, NULL, NULL, "caretOffset",
   "i"},
  {NULL, NULL, NULL, NULL, NULL, NULL}
};

void
spi_initialize_text (DRouteData * data)
{
  droute_add_interface (data, "org.freedesktop.atspi.Text", methods,
			properties,
			(DRouteGetDatumFunction) get_text_from_path, NULL);
};
