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

static AtkDocument *
get_document (DBusMessage * message)
{
  AtkObject *obj = spi_dbus_get_object (dbus_message_get_path (message));
  if (!obj)
    return NULL;
  return ATK_DOCUMENT (obj);
}

static AtkDocument *
get_document_from_path (const char *path, void *user_data)
{
  AtkObject *obj = spi_dbus_get_object (path);
  if (!obj)
    return NULL;
  return ATK_DOCUMENT (obj);
}

static DBusMessage *
impl_getLocale (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkDocument *document = get_document (message);
  const gchar *lc;
  DBusMessage *reply;

  if (!document)
    return spi_dbus_general_error (message);
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
  AtkDocument *document = get_document (message);
  DBusError error;
  gchar *attributename;
  const gchar *atr;
  DBusMessage *reply;

  if (!document)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_STRING, &attributename, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
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
  AtkDocument *document = get_document (message);
  DBusMessage *reply;
  AtkAttributeSet *attributes;
  AtkAttribute *attr = NULL;
  char **retval;
  gint n_attributes = 0;
  gint i;

  if (!document)
    return spi_dbus_general_error (message);

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
  {DROUTE_METHOD, impl_getLocale, "getLocale", "s,,o"},
  {DROUTE_METHOD, impl_getAttributeValue, "getAttributeValue",
   "s,attributename,i:s,,o"},
  {DROUTE_METHOD, impl_getAttributes, "getAttributes", "as,,o"},
  {0, NULL, NULL, NULL}
};

void
spi_initialize_document (DRouteData * data)
{
  droute_add_interface (data, "org.freedesktop.atspi.Document",
			methods, NULL,
			(DRouteGetDatumFunction) get_document_from_path,
			NULL);
};
