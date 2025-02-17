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

static dbus_bool_t
impl_get_Version (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_uint32 (iter, SPI_DBUS_IMAGE_VERSION);
}

static dbus_bool_t
impl_get_ImageDescription (DBusMessageIter *iter, void *user_data)
{
  AtkImage *image = (AtkImage *) user_data;
  g_return_val_if_fail (ATK_IS_IMAGE (user_data), FALSE);
  return droute_return_v_string (iter,
                                 atk_image_get_image_description (image));
}

static dbus_bool_t
impl_get_ImageLocale (DBusMessageIter *iter, void *user_data)
{
  AtkImage *image = (AtkImage *) user_data;
  g_return_val_if_fail (ATK_IS_IMAGE (user_data), FALSE);
  return droute_return_v_string (iter, atk_image_get_image_locale (image));
}

static DBusMessage *
impl_GetImageExtents (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkImage *image = (AtkImage *) user_data;
  dbus_uint32_t coordType;
  gint ix, iy, iwidth, iheight;

  g_return_val_if_fail (ATK_IS_IMAGE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_UINT32, &coordType, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  atk_image_get_image_size (image, &iwidth, &iheight);
  atk_image_get_image_position (image, &ix, &iy, (AtkCoordType) coordType);
  return spi_dbus_return_rect (message, ix, iy, iwidth, iheight);
}

static DBusMessage *
impl_GetImagePosition (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkImage *image = (AtkImage *) user_data;
  dbus_uint32_t coord_type;
  gint ix = 0, iy = 0;
  dbus_int32_t x, y;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_IMAGE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_UINT32, &coord_type, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  atk_image_get_image_position (image, &ix, &iy, (AtkCoordType) coord_type);
  x = ix;
  y = iy;
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32,
                                &y, DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetImageSize (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkImage *image = (AtkImage *) user_data;
  gint iwidth = 0, iheight = 0;
  dbus_int32_t width, height;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_IMAGE (user_data),
                        droute_not_yet_handled_error (message));
  atk_image_get_image_size (image, &iwidth, &iheight);
  width = iwidth;
  height = iheight;
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &width,
                                DBUS_TYPE_INT32, &height, DBUS_TYPE_INVALID);
    }
  return reply;
}

static DRouteMethod methods[] = {
  { impl_GetImageExtents, "GetImageExtents" },
  { impl_GetImagePosition, "GetImagePosition" },
  { impl_GetImageSize, "GetImageSize" },
  { NULL, NULL }
};

static DRouteProperty properties[] = {
  { impl_get_ImageDescription, NULL, "ImageDescription" },
  { impl_get_ImageLocale, NULL, "ImageLocale" },
  { impl_get_Version, NULL, "version" },
  { NULL, NULL, NULL }
};

void
spi_initialize_image (DRoutePath *path)
{
  spi_atk_add_interface (path,
                         ATSPI_DBUS_INTERFACE_IMAGE,
                         spi_org_a11y_atspi_Image,
                         methods, properties);
};
