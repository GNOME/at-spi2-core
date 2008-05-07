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

static AtkImage *
get_image (DBusMessage * message)
{
  AtkObject *obj = spi_dbus_get_object (dbus_message_get_path (message));
  if (!obj)
    return NULL;
  return ATK_IMAGE (obj);
}

static AtkImage *
get_image_from_path (const char *path, void *user_data)
{
  AtkObject *obj = spi_dbus_get_object (path);
  if (!obj || !ATK_IS_IMAGE(obj))
    return NULL;
  return ATK_IMAGE (obj);
}

static dbus_bool_t
impl_get_imageDescription (const char *path, DBusMessageIter * iter,
			   void *user_data)
{
  AtkImage *image = get_image_from_path (path, user_data);
  if (!image)
    return FALSE;
  return droute_return_v_string (iter,
				 atk_image_get_image_description (image));
}

static dbus_bool_t
impl_get_imageLocale (const char *path, DBusMessageIter * iter,
		      void *user_data)
{
  AtkImage *image = get_image_from_path (path, user_data);
  if (!image)
    return FALSE;
  return droute_return_v_string (iter, atk_image_get_image_locale (image));
}

static DBusMessage *
impl_getImageExtents (DBusConnection * bus, DBusMessage * message,
		      void *user_data)
{
  AtkImage *image = get_image (message);
  DBusError error;
  dbus_uint32_t coordType;
  gint ix, iy, iwidth, iheight;

  if (!image)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_UINT32, &coordType, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  atk_image_get_image_size (image, &iwidth, &iheight);
  atk_image_get_image_position (image, &ix, &iy, (AtkCoordType) coordType);
  return spi_dbus_return_rect (message, ix, iy, iwidth, iheight);
}

static DBusMessage *
impl_getImagePosition (DBusConnection * bus, DBusMessage * message,
		       void *user_data)
{
  AtkImage *image = get_image (message);
  DBusError error;
  dbus_uint32_t coord_type;
  gint ix = 0, iy = 0;
  dbus_int32_t x, y;
  DBusMessage *reply;

  if (!image)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_UINT32, &coord_type, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
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
impl_getImageSize (DBusConnection * bus, DBusMessage * message,
		   void *user_data)
{
  AtkImage *image = get_image (message);
  gint iwidth = 0, iheight = 0;
  dbus_int32_t width, height;
  DBusMessage *reply;

  if (!image)
    return spi_dbus_general_error (message);
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
  {impl_getImageExtents, "getImageExtents"},
  {impl_getImagePosition, "getImagePosition"},
  {impl_getImageSize, "getImageSize"},
  {NULL, NULL}
};

static DRouteProperty properties[] = {
  {impl_get_imageDescription, NULL, "imageDescription"},
  {impl_get_imageLocale, NULL, "imageLocale"},
  {NULL, NULL, NULL}
};

void
spi_initialize_image (DRouteData * data)
{
  droute_add_interface (data, "org.freedesktop.atspi.Image", methods,
			properties,
			(DRouteGetDatumFunction) get_image_from_path, NULL);
};
