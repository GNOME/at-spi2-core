/* ATK -  Accessibility Toolkit
 * Copyright 2001 Sun Microsystems Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "atkimage.h"

GType
atk_image_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo =
    {
      sizeof (AtkImageIface),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL
    };

    type = g_type_register_static (G_TYPE_INTERFACE, "AtkImage", &tinfo, 0);
  }

  return type;
}

/**
 * atk_image_get_image_description:
 * @image: a #GObject instance that implements AtkImageIface
 *
 * Get a textual description of this image.
 *
 * Returns: a string representing the image description
 **/
G_CONST_RETURN gchar*
atk_image_get_image_description (AtkImage *image)
{
  AtkImageIface *iface;

  g_return_val_if_fail (ATK_IS_IMAGE (image), NULL);

  iface = ATK_IMAGE_GET_IFACE (image);

  if (iface->get_image_description)
    {
      return (iface->get_image_description) (image);
    }
  else
    {
      return NULL;
    }
}

/**
 * atk_image_get_image_size:
 * @image: a #GObject instance that implements AtkImageIface
 * @width: filled with the image width
 * @height: filled with the image height
 *
 * Get the width and height in pixels for the specified image.
 * The values of @width and @height are returned as -1 if the
 * values cannot be obtained.
 **/
void
atk_image_get_image_size (AtkImage *image, 
                          int      *width,
                          int      *height)
{
  AtkImageIface *iface;
  gint local_width, local_height;
  gint *real_width, *real_height;

  g_return_if_fail (ATK_IS_IMAGE (image));

  if (width)
    real_width = width;
  else
    real_width = &local_width;
  if (height)
    real_height = height;
  else
    real_height = &local_height;
  
  iface = ATK_IMAGE_GET_IFACE (image);

  if (iface->get_image_size)
  {
    iface->get_image_size (image, real_width, real_height);
  }
  else
  {
    *width = -1;
    *height = -1;
  }
}

/**
 * atk_image_set_image_description:
 * @image: a #GObject instance that implements AtkImageIface
 * @description: a string description to set for @image
 *
 * Sets the textual description for this image.
 *
 * Returns: boolean TRUE, or FALSE if operation could
 * not be completed.
 **/
gboolean
atk_image_set_image_description (AtkImage        *image,
                                 const gchar     *description)
{
  AtkImageIface *iface;

  g_return_val_if_fail (ATK_IS_IMAGE (image), FALSE);

  iface = ATK_IMAGE_GET_IFACE (image);

  if (iface->set_image_description)
    {
      return (iface->set_image_description) (image, description);
    }
  else
    {
      return FALSE;
    }
}

/**
 * atk_image_get_image_position:
 * @image: a #GObject instance that implements AtkImageIface
 * @x: address of #gint to put x coordinate position
 * @y: address of #gint to put y coordinate position
 * @coord_type: specifies whether the coordinates are relative to the screen
 * or to the components top level window
 * 
 * Gets the position of the image in the form of a point specifying the
 * images top-left corner.  The values of @x and @y are returned as -1
 * if the values cannot be obtained.
 **/
void     
atk_image_get_image_position (AtkImage *image,
                        gint *x,
		        gint *y,
    		        AtkCoordType coord_type)
{
  AtkImageIface *iface;
  gint local_x, local_y;
  gint *real_x, *real_y;

  g_return_if_fail (ATK_IS_IMAGE (image));

  if (x)
    real_x = x;
  else
    real_x = &local_x;
  if (y)
    real_y = y;
  else
    real_y = &local_y;
  
  iface = ATK_IMAGE_GET_IFACE (image);

  g_return_if_fail (ATK_IS_IMAGE (image));

  iface = ATK_IMAGE_GET_IFACE (image);

  if (iface->get_image_position)
  {
    (iface->get_image_position) (image, real_x, real_y, coord_type);
  }
  else
  {
    *x = -1;
    *y = -1;
  }
}
