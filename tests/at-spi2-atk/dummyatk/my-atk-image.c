/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; https://wiki.gnome.org/Accessibility)
 *
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <atk/atk.h>
#include <stdio.h>
#include <string.h>

#include "my-atk-image.h"
#include "my-atk-object.h"

typedef struct _MyAtkImageInfo MyAtkImageInfo;

static void atk_image_interface_init (AtkImageIface *iface);

G_DEFINE_TYPE_WITH_CODE (MyAtkImage,
                         my_atk_image,
                         MY_TYPE_ATK_OBJECT,
                         G_IMPLEMENT_INTERFACE (ATK_TYPE_IMAGE,
                                                atk_image_interface_init));

guint
my_atk_set_image (AtkImage *image,
                  const gchar *desc,
                  const gint x,
                  const gint y,
                  const gint width,
                  const gint height,
                  const gchar *locale)
{
  g_return_val_if_fail (MY_IS_ATK_IMAGE (image), FALSE);

  MyAtkImage *self = MY_ATK_IMAGE (image);

  self->description = g_strdup (desc);
  self->x = x;
  self->y = y;
  self->width = width;
  self->height = height;
  self->locale = g_strdup (locale);

  return 0;
}

static void
my_atk_image_init (MyAtkImage *obj)
{
  MyAtkImage *self = MY_ATK_IMAGE (obj);
  self->description = NULL;
  self->x = -1;
  self->y = -1;
  self->width = -1;
  self->height = -1;
  self->locale = NULL;
}

void
my_atk_image_get_image_position (AtkImage *obj, gint *x, gint *y, AtkCoordType coord_type)
{
  g_return_if_fail (MY_IS_ATK_IMAGE (obj));

  MyAtkImage *self = MY_ATK_IMAGE (obj);
  *x = self->x;
  *y = self->y;
}

const gchar *
my_atk_image_get_image_description (AtkImage *obj)
{
  g_return_val_if_fail (MY_IS_ATK_IMAGE (obj), NULL);

  MyAtkImage *self = MY_ATK_IMAGE (obj);

  return g_strdup (self->description);
}

void
my_atk_image_get_image_size (AtkImage *obj, gint *width, gint *height)
{
  g_return_if_fail (MY_IS_ATK_IMAGE (obj));

  MyAtkImage *self = MY_ATK_IMAGE (obj);
  *width = self->width;
  *height = self->height;
}

gboolean
my_atk_image_set_image_description (AtkImage *obj, const gchar *desc)
{
  g_return_val_if_fail (MY_IS_ATK_IMAGE (obj), FALSE);

  MyAtkImage *self = MY_ATK_IMAGE (obj);

  g_free (self->description);
  self->description = g_strdup (desc);

  return TRUE;
}

const gchar *
my_atk_image_get_image_locale (AtkImage *obj)
{
  g_return_val_if_fail (MY_IS_ATK_IMAGE (obj), NULL);

  MyAtkImage *self = MY_ATK_IMAGE (obj);

  return self->locale;
}

static void
atk_image_interface_init (AtkImageIface *iface)
{
  if (!iface)
    return;
  iface->get_image_position = my_atk_image_get_image_position;
  iface->set_image_description = my_atk_image_set_image_description;
  iface->get_image_description = my_atk_image_get_image_description;
  iface->get_image_size = my_atk_image_get_image_size;
  iface->get_image_locale = my_atk_image_get_image_locale;
}

static void
my_atk_image_initialize (AtkObject *obj, gpointer data)
{
}

static void
my_atk_image_finalize (GObject *object)
{
}

static void
my_atk_image_class_init (MyAtkImageClass *my_class)
{
  AtkObjectClass *atk_class = ATK_OBJECT_CLASS (my_class);
  GObjectClass *gobject_class = G_OBJECT_CLASS (my_class);

  gobject_class->finalize = my_atk_image_finalize;

  atk_class->initialize = my_atk_image_initialize;
}
