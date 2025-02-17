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
  return droute_return_v_uint32 (iter, SPI_DBUS_DOCUMENT_VERSION);
}

static dbus_bool_t
impl_get_CurrentPageNumber (DBusMessageIter *iter, void *user_data)
{
  AtkDocument *document = (AtkDocument *) user_data;
  g_return_val_if_fail (ATK_IS_DOCUMENT (user_data), FALSE);
  return droute_return_v_int32 (iter, atk_document_get_current_page_number (document));
}

static dbus_bool_t
impl_get_PageCount (DBusMessageIter *iter, void *user_data)
{
  AtkDocument *document = (AtkDocument *) user_data;
  g_return_val_if_fail (ATK_IS_DOCUMENT (user_data), FALSE);
  return droute_return_v_int32 (iter, atk_document_get_page_count (document));
}

static DBusMessage *
impl_GetLocale (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkDocument *document = (AtkDocument *) user_data;
  const gchar *lc;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_DOCUMENT (user_data),
                        droute_not_yet_handled_error (message));
  lc = atk_document_get_locale (document);
  if (!lc)
    lc = "";
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &lc,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetAttributeValue (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkDocument *document = (AtkDocument *) user_data;
  gchar *attributename;
  const gchar *atr;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_DOCUMENT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_STRING, &attributename, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  atr = atk_document_get_attribute_value (document, attributename);
  if (!atr)
    atr = "";
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &atr,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetAttributes (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkDocument *document = (AtkDocument *) user_data;
  DBusMessage *reply;
  AtkAttributeSet *attributes;
  DBusMessageIter iter;

  g_return_val_if_fail (ATK_IS_DOCUMENT (user_data),
                        droute_not_yet_handled_error (message));

  attributes = atk_document_get_attributes (document);

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
impl_GetTextSelections (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkDocument *document = (AtkDocument *) user_data;
  GArray *selections;
  gint i, count;
  DBusMessageIter iter, iter_array, iter_struct;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_DOCUMENT (user_data),
                        droute_not_yet_handled_error (message));

  selections = atk_document_get_text_selections (document);
  count = (selections ? selections->len : 0);

  reply = dbus_message_new_method_return (message);
  if (!reply)
    goto done;
  dbus_message_iter_init_append (reply, &iter);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "((so)i(so)ib)", &iter_array);
  for (i = 0; i < count; i++)
    {
      dbus_message_iter_open_container (&iter_array, DBUS_TYPE_STRUCT, NULL, &iter_struct);
      AtkTextSelection *item = &g_array_index (selections, AtkTextSelection, i);
      spi_object_append_reference (&iter_struct, item->start_object);
      dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, &item->start_offset);
      spi_object_append_reference (&iter_struct, item->end_object);
      dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, &item->end_offset);
      dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_BOOLEAN, &item->start_is_active);
      dbus_message_iter_close_container (&iter_array, &iter_struct);
    }
  dbus_message_iter_close_container (&iter, &iter_array);

done:
  if (selections)
    g_array_free (selections, TRUE);
  return reply;
}

static DBusMessage *
impl_SetTextSelections (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkDocument *document = (AtkDocument *) user_data;
  GArray *selections;
  DBusMessageIter iter, iter_array, iter_struct;
  gboolean ret;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_DOCUMENT (user_data),
                        droute_not_yet_handled_error (message));

  if (strcmp (dbus_message_get_signature (message), "a((so)i(so)ib)") != 0)
    {
      return droute_invalid_arguments_error (message);
    }

  selections = g_array_new (FALSE, TRUE, sizeof (AtkTextSelection));
  dbus_message_iter_init (message, &iter);
  dbus_message_iter_recurse (&iter, &iter_array);

  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
    {
      AtkTextSelection selection;
      dbus_message_iter_recurse (&iter_array, &iter_struct);
      selection.start_object = ATK_OBJECT (spi_dbus_get_object_from_iter (&iter_struct));
      dbus_message_iter_get_basic (&iter_struct, &selection.start_offset);
      dbus_message_iter_next (&iter_struct);
      selection.end_object = ATK_OBJECT (spi_dbus_get_object_from_iter (&iter_struct));
      dbus_message_iter_get_basic (&iter_struct, &selection.end_offset);
      dbus_message_iter_next (&iter_struct);
      dbus_message_iter_get_basic (&iter_struct, &selection.start_is_active);
      g_array_append_val (selections, selection);
      dbus_message_iter_next (&iter_array);
    }

  ret = atk_document_set_text_selections (document, selections);
  g_array_free (selections, TRUE);

  reply = dbus_message_new_method_return (message);
  if (reply)
    dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret, DBUS_TYPE_INVALID);
  return reply;
}

static DRouteMethod methods[] = {
  { impl_GetLocale, "GetLocale" },
  { impl_GetAttributeValue, "GetAttributeValue" },
  { impl_GetAttributes, "GetAttributes" },
  { impl_GetTextSelections, "GetTextSelections" },
  { impl_SetTextSelections, "SetTextSelections" },
  { NULL, NULL }
};

static DRouteProperty properties[] = {
  { impl_get_CurrentPageNumber, NULL, "CurrentPageNumber" },
  { impl_get_PageCount, NULL, "PageCount" },
  { impl_get_Version, NULL, "version" },
  { NULL, NULL, NULL }
};

void
spi_initialize_document (DRoutePath *path)
{
  droute_path_add_interface (path,
                             ATSPI_DBUS_INTERFACE_DOCUMENT, spi_org_a11y_atspi_Document, methods, properties);
};
