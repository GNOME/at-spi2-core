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

#define MY_TYPE_ATK_COMPONENT (my_atk_component_get_type ())
#define MY_ATK_COMPONENT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ATK_COMPONENT, MyAtkComponent))
#define MY_ATK_COMPONENT_CLASS(vtable) (G_TYPE_CHECK_CLASS_CAST ((vtable), MY_TYPE_ATK_COMPONENT, MyAtkComponentClass))
#define MY_IS_ATK_COMPONENT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ATK_COMPONENT))
#define MY_IS_ATK_COMPONENT_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), MY_TYPE_ATK_COMPONENT))
#define MY_ATK_COMPONENT_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS ((inst), MY_TYPE_ATK_COMPONENT, MyAtkComponentClass))

typedef struct _MyAtkComponent MyAtkComponent;
typedef struct _MyAtkComponentClass MyAtkComponentClass;

typedef void (*MyAtkComponentFunc) (MyAtkComponent *component);

struct _MyAtkComponent
{
  MyAtkObject parent;
  AtkRectangle extent;
  gboolean extent_may_change;
  gboolean is_manage_descendants;
  AtkLayer layer;
  gint zorder;
  gdouble alpha;
};

struct _MyAtkComponentClass
{
  MyAtkObjectClass parent;
};

GType my_atk_component_get_type ();

void my_atk_component_set_layer (AtkComponent *component, AtkLayer layer);
void my_atk_component_set_mdi_zorder (AtkComponent *component, gint mdi_zorder);
void my_atk_component_set_alpha (AtkComponent *component, gdouble alpha);
