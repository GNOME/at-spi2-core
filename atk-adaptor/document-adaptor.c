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

#include "spi-common/spi-dbus.h"

static DBusMessage *
impl_getLocale (DBusConnection *bus,
                DBusMessage *message,
                void *user_data)
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
impl_getAttributeValue (DBusConnection * bus, DBusMessage * message,
                        void *user_data)
{
  AtkDocument *document = (AtkDocument *) user_data;
  DBusError error;
  gchar *attributename;
  const gchar *atr;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_DOCUMENT (user_data),
                        droute_not_yet_handled_error (message));
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_STRING, &attributename, DBUS_TYPE_INVALID))
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
impl_getAttributes (DBusConnection * bus, DBusMessage * message,
                    void *user_data)
{
  AtkDocument *document = (AtkDocument *) user_data;
  DBusMessage *reply;
  AtkAttributeSet *attributes;
  AtkAttribute *attr = NULL;
  char **retval;
  gint n_attributes = 0;
  gint i;

  g_return_val_if_fail (ATK_IS_DOCUMENT (user_data),
                        droute_not_yet_handled_error (message));

  attributes = atk_document_get_attributes (document);
  if (attributes)
    n_attributes = g_slist_length (attributes);

  retval = (char **) g_malloc (n_attributes * sizeof (char *));

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
  {impl_getLocale, "getLocale"},
  {impl_getAttributeValue, "getAttributeValue"},
  {impl_getAttributes, "getAttributes"},
  {NULL, NULL}
};

void
spi_initialize_document (DRoutePath *path)
{
  droute_path_add_interface (path,
                             SPI_DBUS_INTERFACE_DOCUMENT,
                             methods,
                             NULL);
};
