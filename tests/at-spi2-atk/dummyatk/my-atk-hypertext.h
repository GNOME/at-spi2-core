/*
 * Copyright 2008 Codethink Ltd.
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
#define MY_TYPE_ATK_HYPERTEXT (my_atk_hypertext_get_type ())
#define MY_ATK_HYPERTEXT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ATK_HYPERTEXT, MyAtkHypertext))
#define MY_ATK_HYPERTEXT_CLASS(vtable) (G_TYPE_CHECK_CLASS_CAST ((vtable), MY_TYPE_ATK_HYPERTEXT, MyAtkHypertextClass))
#define MY_IS_ATK_HYPERTEXT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ATK_HYPERTEXT))
#define MY_IS_ATK_HYPERTEXT_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), MY_TYPE_ATK_HYPERTEXT))
#define MY_ATK_HYPERTEXT_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS ((inst), MY_TYPE_ATK_HYPERTEXT, MyAtkHypertextClass))

typedef struct _MyAtkHypertext MyAtkHypertext;
typedef struct _MyAtkHypertextClass MyAtkHypertextClass;

struct _MyAtkHypertext
{
  MyAtkObject parent;
  GPtrArray *array;
};

struct _MyAtkHypertextClass
{
  MyAtkObjectClass parent;
};

GType my_atk_hypertext_get_type (void);

gint my_atk_set_hypertext (AtkHypertext *obj, const gchar *text);
