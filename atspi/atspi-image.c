/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 * Copyright 2010, 2011 Novell, Inc.
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

#include "atspi-private.h"

/**
 * atspi_image_get_image_description:
 * @obj: a pointer to the #AtspiImage implementor on which to operate.
 *
 * Gets the description of the image displayed in an #AtspiImage object.
 *
 * Returns: a UTF-8 string describing the image.
 **/
gchar *
atspi_image_get_image_description (AtspiImage *obj, GError **error)
{
  char *retval = NULL;

  g_return_val_if_fail (obj != NULL, NULL);

  _atspi_dbus_get_property (obj, atspi_interface_image, "ImageDescription", error, "s", &retval);

  return retval;
}

/**
 * atspi_image_get_image_size:
 * @obj: a pointer to the #AtspiImage to query.
 *
 * Gets the size of the image displayed in a specified #AtspiImage object.
 * The returned values are meaningful only if the Image has both
 * STATE_VISIBLE and STATE_SHOWING.
 *
 * Returns: a pointer to an #AtspiPoint where x corresponds to
 * the image's width and y corresponds to the image's height.
 *
 **/
AtspiPoint *
atspi_image_get_image_size (AtspiImage *obj, GError **error)
{
  dbus_int32_t d_w, d_h;
  AtspiPoint ret;

  ret.x = ret.y = -1;
  if (!obj)
    return atspi_point_copy (&ret);

  _atspi_dbus_call (obj, atspi_interface_image, "GetImageSize", error, "=>ii", &d_w, &d_h);
  ret.x = d_w;
  ret.y = d_h;
  return atspi_point_copy (&ret);
}

/**
 * atspi_image_get_image_position:
 * @obj: a pointer to the #AtspiImage implementor to query.
 * @ctype: the desired coordinate system into which to return the results,
 *         (e.g. ATSPI_COORD_TYPE_WINDOW, ATSPI_COORD_TYPE_SCREEN).
 *
 * Gets the minimum x and y coordinates of the image displayed in a
 *         specified #AtspiImage implementor.
 * The returned values are meaningful only if the Image has both
 * STATE_VISIBLE and STATE_SHOWING.
 *
 * Returns: a pointer to an #AtspiPoint where x and y correspond to the
 * minimum coordinates of the displayed image.
 *
 **/
AtspiPoint *
atspi_image_get_image_position (AtspiImage *obj,
                                AtspiCoordType ctype,
                                GError **error)
{
  dbus_int32_t d_x, d_y;
  dbus_uint32_t d_ctype = ctype;
  AtspiPoint ret;

  ret.x = ret.y = 0;

  if (!obj)
    return atspi_point_copy (&ret);

  _atspi_dbus_call (obj, atspi_interface_image, "GetImagePosition", error, "u=>ii", d_ctype, &d_x, &d_y);

  ret.x = d_x;
  ret.y = d_y;
  return atspi_point_copy (&ret);
}

/**
 * atspi_image_get_image_extents:
 * @obj: a pointer to the #AtspiImage implementor to query.
 * @ctype: the desired coordinate system into which to return the results,
 *         (e.g. ATSPI_COORD_TYPE_WINDOW, ATSPI_COORD_TYPE_SCREEN).
 *
 * Gets the bounding box of the image displayed in a
 *         specified #AtspiImage implementor.
 * The returned values are meaningful only if the Image has both
 * STATE_VISIBLE and STATE_SHOWING.
 *
 * Returns: a pointer to an #AtspiRect corresponding to the image's bounding box. The minimum x and y coordinates,
 * width, and height are specified.
 **/
AtspiRect *
atspi_image_get_image_extents (AtspiImage *obj,
                               AtspiCoordType ctype,
                               GError **error)
{
  dbus_uint32_t d_ctype = ctype;
  AtspiRect bbox;

  bbox.x = bbox.y = bbox.width = bbox.height = -1;
  g_return_val_if_fail (obj != NULL, atspi_rect_copy (&bbox));

  _atspi_dbus_call (obj, atspi_interface_image, "GetImageExtents", error, "u=>(iiii)", d_ctype, &bbox);

  return atspi_rect_copy (&bbox);
}

/**
 * atspi_image_get_image_locale:
 * @obj: a pointer to the #AtspiImage to query.
 *
 * Gets the locale associated with an image and its textual representation.
 *
 * Returns: A POSIX LC_MESSAGES-style locale value for image description and text.
 **/
gchar *
atspi_image_get_image_locale (AtspiImage *obj, GError **error)
{
  gchar *retval = NULL;

  g_return_val_if_fail (obj != NULL, g_strdup ("C"));

  _atspi_dbus_get_property (obj, atspi_interface_image, "ImageLocale", error, "s", &retval);

  return retval;
}

static void
atspi_image_base_init (AtspiImage *klass)
{
}

GType
atspi_image_get_type (void)
{
  static GType type = 0;

  if (!type)
    {
      static const GTypeInfo tinfo = {
        sizeof (AtspiImage),
        (GBaseInitFunc) atspi_image_base_init,
        (GBaseFinalizeFunc) NULL,
      };

      type = g_type_register_static (G_TYPE_INTERFACE, "AtspiImage", &tinfo, 0);
    }
  return type;
}
