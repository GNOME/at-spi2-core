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

#include <atk/atk.h>
#include <droute/droute.h>
#include "bridge.h"

#include "spi-dbus.h"
#include "object.h"
#include "introspection.h"

static dbus_bool_t
impl_get_CurrentPageNumber (DBusMessageIter * iter, void *user_data)
{
  AtkDocument *document = (AtkDocument *) user_data;
  g_return_val_if_fail (ATK_IS_DOCUMENT (user_data), FALSE);
  return droute_return_v_int32 (iter, atk_document_get_current_page_number (document));
}

static dbus_bool_t
impl_get_PageCount (DBusMessageIter * iter, void *user_data)
{
  AtkDocument *document = (AtkDocument *) user_data;
  g_return_val_if_fail (ATK_IS_DOCUMENT (user_data), FALSE);
  return droute_return_v_int32 (iter, atk_document_get_page_count (document));
}

static DBusMessage *
impl_GetLocale (DBusConnection * bus, DBusMessage * message, void *user_data)
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
impl_GetAttributeValue (DBusConnection * bus, DBusMessage * message,
                        void *user_data)
{
  AtkDocument *document = (AtkDocument *) user_data;
  gchar *attributename;
  const gchar *atr;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_DOCUMENT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args
      (message, NULL, DBUS_TYPE_STRING, &attributename, DBUS_TYPE_INVALID))
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
impl_GetAttributes (DBusConnection * bus, DBusMessage * message,
                    void *user_data)
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

static DRouteMethod methods[] = {
  {impl_GetLocale, "GetLocale"},
  {impl_GetAttributeValue, "GetAttributeValue"},
  {impl_GetAttributes, "GetAttributes"},
  {NULL, NULL}
};

static DRouteProperty properties[] = {
  {impl_get_CurrentPageNumber, NULL, "CurrentPageNumber"},
  {impl_get_PageCount, NULL, "PageCount"},
  {NULL, NULL, NULL}
};

void
spi_initialize_document (DRoutePath * path)
{
  droute_path_add_interface (path,
                             ATSPI_DBUS_INTERFACE_DOCUMENT, spi_org_a11y_atspi_Document, methods, properties);
};
