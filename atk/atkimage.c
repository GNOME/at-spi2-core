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

/* Superseded by atkimage.c */
#include "atkimage.h"

GType
atk_image_get_type ()
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo =
    {
      sizeof (AtkImageIface),
      NULL,
      NULL,

    };

    type = g_type_register_static (G_TYPE_INTERFACE, "AtkImage", &tinfo, 0);
  }

  return type;
}

/**
 * atk_image_get_storage_type:
 * @value: a GObject instance that implements AtkImageIface
 * @return: a AtkStorageType representing the image storage type, or 0
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkImageIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_image() convenience method.
 **/
AtkImageType
atk_image_get_storage_type (AtkImage *obj)
{
  AtkImageIface *iface;

  g_return_val_if_fail (obj != NULL, 0);
  g_return_val_if_fail (ATK_IS_IMAGE (obj), 0);

  iface = ATK_IMAGE_GET_IFACE (obj);

  if (iface->get_storage_type)
    return (iface->get_storage_type) (obj);
  else
    return 0;
}

/**
 * atk_image_get_image_description:
 * @value: a GObject instance that implements AtkImageIface
 * @return: a gchar* representing the image description, or NULL
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkImageIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_image() convenience method.
 **/
G_CONST_RETURN gchar*
atk_image_get_image_description (AtkImage *obj)
{
  AtkImageIface *iface;

  g_return_val_if_fail (obj != NULL, NULL);
  g_return_val_if_fail (ATK_IS_IMAGE (obj), NULL);

  iface = ATK_IMAGE_GET_IFACE (obj);

  if (iface->get_image_description)
    return (iface->get_image_description) (obj);
  else
    return NULL;
}

/**
 * atk_image_get_image_height:
 * @value: a GObject instance that implements AtkImageIface
 * @return: a gint representing the image height, or 0
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkImageIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_image() convenience method.
 **/
gint
atk_image_get_image_height (AtkImage *obj)
{
  AtkImageIface *iface;

  g_return_val_if_fail (obj != NULL, 0);
  g_return_val_if_fail (ATK_IS_IMAGE (obj), 0);

  iface = ATK_IMAGE_GET_IFACE (obj);

  if (iface->get_image_height)
    return (iface->get_image_height) (obj);
  else
    return 0;
}

/**
 * atk_image_get_image_width:
 * @value: a GObject instance that implements AtkImageIface
 * @return: a gint representing the image width, or 0
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkImageIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_image() convenience method.
 **/
gint
atk_image_get_image_width (AtkImage *obj)
{
  AtkImageIface *iface;

  g_return_val_if_fail (obj != NULL, 0);
  g_return_val_if_fail (ATK_IS_IMAGE (obj), 0);

  iface = ATK_IMAGE_GET_IFACE (obj);

  if (iface->get_image_width)
    return (iface->get_image_width) (obj);
  else
    return 0;
}

/**
 * atk_image_set_image_description:
 * @value: a GObject instance that implements AtkImageIface
 * @return: void
 **/
void
atk_image_set_image_description (AtkImage        *obj,
                              const gchar     *description)
{
  AtkImageIface *iface;

  g_return_if_fail (obj != NULL);
  g_return_if_fail (ATK_IS_IMAGE (obj));

  iface = ATK_IMAGE_GET_IFACE (obj);

  if (iface->set_image_description)
    (iface->set_image_description) (obj, description);
}
