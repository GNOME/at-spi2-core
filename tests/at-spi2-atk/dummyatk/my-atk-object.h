/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; https://wiki.gnome.org/Accessibility)
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#define MY_TYPE_ATK_OBJECT (my_atk_object_get_type ())
#define MY_ATK_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ATK_OBJECT, MyAtkObject))
#define MY_ATK_OBJECT_CLASS(vtable) (G_TYPE_CHECK_CLASS_CAST ((vtable), MY_TYPE_ATK_OBJECT, MyAtkObjectClass))
#define MY_IS_ATK_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ATK_OBJECT))
#define MY_IS_ATK_OBJECT_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), MY_TYPE_ATK_OBJECT))
#define MY_ATK_OBJECT_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS ((inst), MY_TYPE_ATK_OBJECT, MyAtkObjectClass))

typedef struct _MyAtkObject MyAtkObject;
typedef struct _MyAtkObjectClass MyAtkObjectClass;

struct _MyAtkObject
{
  AtkObject parent;
  AtkStateSet *state_set;
  AtkRelationSet *relation_set;
  GPtrArray *children;
  gint id;
  gboolean selected;
};

struct _MyAtkObjectClass
{
  AtkObjectClass parent;
};

GType my_atk_object_get_type ();

void my_atk_object_add_child (MyAtkObject *parent,
                              MyAtkObject *child);

void my_atk_object_remove_child (MyAtkObject *parent,
                                 MyAtkObject *child);
