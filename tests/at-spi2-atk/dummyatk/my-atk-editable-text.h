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

#define MY_TYPE_ATK_EDITABLE_TEXT (my_atk_editable_text_get_type ())
#define MY_ATK_EDITABLE_TEXT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ATK_EDITABLE_TEXT, MyAtkEditableText))
#define MY_ATK_EDITABLE_TEXT_CLASS(vtable) (G_TYPE_CHECK_CLASS_CAST ((vtable), MY_TYPE_ATK_EDITABLE_TEXT, MyAtkEditableTextClass))
#define MY_IS_ATK_EDITABLE_TEXT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ATK_EDITABLE_TEXT))
#define MY_IS_ATK_EDITABLE_TEXT_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), MY_TYPE_ATK_EDITABLE_TEXT))
#define MY_ATK_EDITABLE_TEXT_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS ((inst), MY_TYPE_ATK_EDITABLE_TEXT, MyAtkEditableTextClass))

typedef struct _MyAtkEditableText MyAtkEditableText;
typedef struct _MyAtkEditableTextClass MyAtkEditableTextClass;

typedef void (*MyAtkEditableTextFunc) (MyAtkEditableText *editable_text);

struct _MyAtkEditableText
{
  MyAtkObject parent;
  gchar *text;
};

struct _MyAtkEditableTextClass
{
  MyAtkObjectClass parent;
};

GType my_atk_editable_text_get_type (void);

guint my_atk_set_editable_text (AtkEditableText *editable_text, const gchar *text);
