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

#pragma once

#include <atk/atk.h>
#include <glib-object.h>
#include <glib.h>

#include "my-atk-object.h"

#define MY_TYPE_ATK_IMAGE (my_atk_image_get_type ())
#define MY_ATK_IMAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ATK_IMAGE, MyAtkImage))
#define MY_ATK_IMAGE_CLASS(vtable) (G_TYPE_CHECK_CLASS_CAST ((vtable), MY_TYPE_ATK_IMAGE, MyAtkImageClass))
#define MY_IS_ATK_IMAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ATK_IMAGE))
#define MY_IS_ATK_IMAGE_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), MY_TYPE_ATK_IMAGE))
#define MY_ATK_IMAGE_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS ((inst), MY_TYPE_ATK_IMAGE, MyAtkImageClass))

typedef struct _MyAtkImage MyAtkImage;
typedef struct _MyAtkImageClass MyAtkImageClass;

typedef void (*MyAtkImageFunc) (MyAtkImage *image);

struct _MyAtkImage
{
  MyAtkObject parent;
  gchar *description;
  gint x;
  gint y;
  gint width;
  gint height;
  gchar *locale;
};

struct _MyAtkImageClass
{
  MyAtkObjectClass parent;
};

GType my_atk_image_get_type (void);

guint my_atk_set_image (AtkImage *image,
                        const gchar *desc,
                        const gint x,
                        const gint y,
                        const gint width,
                        const gint height,
                        const gchar *locale);
