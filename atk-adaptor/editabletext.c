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

static AtkEditableText *
get_editable (DBusMessage * message)
{
  AtkObject *obj = spi_dbus_get_object (dbus_message_get_path (message));
  if (!obj)
    return NULL;
  return ATK_EDITABLE_TEXT (obj);
}

static AtkEditableText *
get_editable_from_path (const char *path, void *user_data)
{
  AtkObject *obj = spi_dbus_get_object (path);
  if (!obj || !ATK_IS_EDITABLE_TEXT(obj))
    return NULL;
  return ATK_EDITABLE_TEXT (obj);
}

static DBusMessage *
impl_setTextContents (DBusConnection * bus, DBusMessage * message,
		      void *user_data)
{
  AtkEditableText *editable = get_editable (message);
  const char *newContents;
  dbus_bool_t rv;
  DBusError error;
  DBusMessage *reply;

  if (!editable)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_STRING, &newContents, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  atk_editable_text_set_text_contents (editable, newContents);
  rv = TRUE;
  // TODO decide if we really need this return value
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_insertText (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkEditableText *editable = get_editable (message);
  dbus_int32_t position, length;
  char *text;
  dbus_bool_t rv;
  DBusError error;
  DBusMessage *reply;
  gint ip;

  if (!editable)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &position, DBUS_TYPE_STRING, &text,
       DBUS_TYPE_INT32, &length, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  ip = position;
  atk_editable_text_insert_text (editable, text, length, &ip);
  rv = TRUE;
  // TODO decide if we really need this return value
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_setAttributes (DBusConnection * bus, DBusMessage * message,
		    void *user_data)
{
  AtkEditableText *editable = get_editable (message);
  const char *attributes;
  dbus_int32_t startPos, endPos;
  dbus_bool_t rv;
  DBusError error;
  DBusMessage *reply;

  if (!editable)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_STRING, &attributes, DBUS_TYPE_INT32,
       &startPos, DBUS_TYPE_INT32, &endPos, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  // TODO implement
  rv = FALSE;
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_copyText (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkEditableText *editable = get_editable (message);
  dbus_int32_t startPos, endPos;
  DBusError error;

  if (!editable)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &startPos, DBUS_TYPE_INT32, &endPos,
       DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  atk_editable_text_copy_text (editable, startPos, endPos);
  return NULL;
}

static DBusMessage *
impl_cutText (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkEditableText *editable = get_editable (message);
  dbus_int32_t startPos, endPos;
  DBusError error;
  dbus_bool_t rv;
  DBusMessage *reply;

  if (!editable)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &startPos, DBUS_TYPE_INT32, &endPos,
       DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  atk_editable_text_cut_text (editable, startPos, endPos);
  rv = TRUE;
  // TODO decide if we really need this return value
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_deleteText (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkEditableText *editable = get_editable (message);
  dbus_int32_t startPos, endPos;
  DBusError error;
  dbus_bool_t rv;
  DBusMessage *reply;

  if (!editable)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &startPos, DBUS_TYPE_INT32, &endPos,
       DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  atk_editable_text_delete_text (editable, startPos, endPos);
  rv = TRUE;
  // TODO decide if we really need this return value
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_pasteText (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkEditableText *editable = get_editable (message);
  dbus_int32_t position;
  DBusError error;
  dbus_bool_t rv;
  DBusMessage *reply;

  if (!editable)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &position, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  atk_editable_text_paste_text (editable, position);
  rv = TRUE;
  // TODO decide if we really need this return value
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DRouteMethod methods[] = {
  {impl_setTextContents, "setTextContents"},
  {impl_insertText, "insertText"},
  {impl_setAttributes, "setAttributes"},
  {impl_copyText, "copyText"},
  {impl_cutText, "cutText"},
  {impl_deleteText, "deleteText"},
  {impl_pasteText, "pasteText"},
  {NULL, NULL}
};

void
spi_initialize_editabletext (DRouteData * data)
{
  droute_add_interface (data, SPI_DBUS_INTERFACE_EDITABLE_TEXT,
			methods, NULL,
			(DRouteGetDatumFunction) get_editable_from_path,
			NULL);
};
