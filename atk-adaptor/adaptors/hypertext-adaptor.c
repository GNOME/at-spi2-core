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
#include <atk/atk.h>
#include <droute/droute.h>

#include "spi-dbus.h"

#include "introspection.h"
#include "object.h"

static DBusMessage *
impl_GetNLinks (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkHypertext *hypertext = (AtkHypertext *) user_data;
  gint rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_HYPERTEXT (user_data),
                        droute_not_yet_handled_error (message));
  rv = atk_hypertext_get_n_links (hypertext);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &rv,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetLink (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkHypertext *hypertext = (AtkHypertext *) user_data;
  dbus_int32_t linkIndex;
  AtkHyperlink *link;

  g_return_val_if_fail (ATK_IS_HYPERTEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &linkIndex, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  link = atk_hypertext_get_link (hypertext, linkIndex);
  /*The above line doesn't ref the link, and the next call is going to unref*/
  if (link)
    g_object_ref (link);
  return spi_hyperlink_return_reference (message, link);
}

static DBusMessage *
impl_GetLinkIndex (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkHypertext *hypertext = (AtkHypertext *) user_data;
  dbus_int32_t characterIndex;
  dbus_int32_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_HYPERTEXT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &characterIndex, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  rv = atk_hypertext_get_link_index (hypertext, characterIndex);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &rv,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static dbus_bool_t
impl_get_Version (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_uint32 (iter, SPI_DBUS_HYPERTEXT_VERSION);
}

static DRouteMethod methods[] = {
  { impl_GetNLinks, "GetNLinks" },
  { impl_GetLink, "GetLink" },
  { impl_GetLinkIndex, "GetLinkIndex" },
  { NULL, NULL }
};

static DRouteProperty properties[] = {
  { impl_get_Version, NULL, "version" },
  { NULL, NULL, NULL },
};

void
spi_initialize_hypertext (DRoutePath *path)
{
  spi_atk_add_interface (path,
                         ATSPI_DBUS_INTERFACE_HYPERTEXT,
                         spi_org_a11y_atspi_Hypertext,
                         methods, properties);
};
