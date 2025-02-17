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

#include "bridge.h"
#include "introspection.h"
#include <atk/atk.h>
#include <droute/droute.h>

#include "spi-dbus.h"

static DBusMessage *
impl_SetTextContents (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkEditableText *editable = (AtkEditableText *) user_data;
  const char *newContents;
  dbus_bool_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_EDITABLE_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_STRING, &newContents, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
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
impl_InsertText (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkEditableText *editable = (AtkEditableText *) user_data;
  dbus_int32_t position, length;
  char *text;
  dbus_bool_t rv;
  DBusMessage *reply;
  gint ip;

  g_return_val_if_fail (ATK_IS_EDITABLE_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &position, DBUS_TYPE_STRING, &text,
                              DBUS_TYPE_INT32, &length, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
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
impl_CopyText (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkEditableText *editable = (AtkEditableText *) user_data;
  dbus_int32_t startPos, endPos;

  g_return_val_if_fail (ATK_IS_EDITABLE_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &startPos, DBUS_TYPE_INT32, &endPos,
                              DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  atk_editable_text_copy_text (editable, startPos, endPos);
  return dbus_message_new_method_return (message);
}

static DBusMessage *
impl_CutText (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkEditableText *editable = (AtkEditableText *) user_data;
  dbus_int32_t startPos, endPos;
  dbus_bool_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_EDITABLE_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &startPos, DBUS_TYPE_INT32, &endPos,
                              DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
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
impl_DeleteText (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkEditableText *editable = (AtkEditableText *) user_data;
  dbus_int32_t startPos, endPos;
  dbus_bool_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_EDITABLE_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &startPos, DBUS_TYPE_INT32, &endPos,
                              DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
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
impl_PasteText (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkEditableText *editable = (AtkEditableText *) user_data;
  dbus_int32_t position;
  dbus_bool_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_EDITABLE_TEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &position, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
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

static dbus_bool_t
impl_get_Version (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_uint32 (iter, SPI_DBUS_EDITABLE_TEXT_VERSION);
}

static DRouteMethod methods[] = {
  { impl_SetTextContents, "SetTextContents" },
  { impl_InsertText, "InsertText" },
  { impl_CopyText, "CopyText" },
  { impl_CutText, "CutText" },
  { impl_DeleteText, "DeleteText" },
  { impl_PasteText, "PasteText" },
  { NULL, NULL }
};

static DRouteProperty properties[] = {
  { impl_get_Version, NULL, "version" },
  { NULL, NULL, NULL },
};

void
spi_initialize_editabletext (DRoutePath *path)
{
  spi_atk_add_interface (path,
                         ATSPI_DBUS_INTERFACE_EDITABLE_TEXT, spi_org_a11y_atspi_EditableText,
                         methods, properties);
};
