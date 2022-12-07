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

#include "my-atk-object.h"

#include "my-atk-component.h"

typedef struct _MyAtkComponentInfo MyAtkComponentInfo;

static void atk_component_interface_init (AtkComponentIface *iface);

G_DEFINE_TYPE_WITH_CODE (MyAtkComponent,
                         my_atk_component,
                         MY_TYPE_ATK_OBJECT,
                         G_IMPLEMENT_INTERFACE (ATK_TYPE_COMPONENT,
                                                atk_component_interface_init));

void
my_atk_component_set_layer (AtkComponent *component,
                            AtkLayer layer)
{
  g_return_if_fail (MY_IS_ATK_COMPONENT (component));

  MyAtkComponent *self = MY_ATK_COMPONENT (component);
  self->layer = layer;
}

void
my_atk_component_set_mdi_zorder (AtkComponent *component,
                                 gint mdi_zorder)
{
  g_return_if_fail (MY_IS_ATK_COMPONENT (component));

  MyAtkComponent *self = MY_ATK_COMPONENT (component);
  self->zorder = mdi_zorder;
}

void
my_atk_component_set_alpha (AtkComponent *component,
                            gdouble alpha)
{

  g_return_if_fail (MY_IS_ATK_COMPONENT (component));

  MyAtkComponent *self = MY_ATK_COMPONENT (component);
  self->alpha = alpha;
}

static void
my_atk_component_get_extents (AtkComponent *component,
                              gint *width,
                              gint *height,
                              gint *x,
                              gint *y,
                              AtkCoordType coord_type)
{
  g_return_if_fail (MY_IS_ATK_COMPONENT (component));

  MyAtkComponent *self = MY_ATK_COMPONENT (component);
  *width = self->extent.width;
  *height = self->extent.height;
  *x = self->extent.x;
  *y = self->extent.y;
}

static gboolean
my_atk_component_set_extents (AtkComponent *component,
                              gint x,
                              gint y,
                              gint width,
                              gint height,
                              AtkCoordType coord_type)
{
  g_return_val_if_fail (MY_IS_ATK_COMPONENT (component), FALSE);

  MyAtkComponent *self = MY_ATK_COMPONENT (component);

  if (self->extent_may_change)
    {
      self->extent.width = width;
      self->extent.height = height;
      self->extent.x = x;
      self->extent.y = y;
      return TRUE;
    }
  return FALSE;
}

static gboolean
my_atk_component_contains (AtkComponent *component,
                           gint c_x,
                           gint c_y,
                           AtkCoordType coord_type)
{
  g_return_val_if_fail (MY_IS_ATK_COMPONENT (component), FALSE);

  gint x, y, w, h;
  my_atk_component_get_extents (component, &x, &y, &w, &h, coord_type);

  if ((c_x >= x) && (c_y >= y) && (c_x < x + w) && (c_y < y + h))
    return TRUE;
  else
    return FALSE;
}

static AtkObject *
my_atk_component_ref_accessible_at_point (AtkComponent *component,
                                          gint x,
                                          gint y,
                                          AtkCoordType coord_type)
{
  g_return_val_if_fail (MY_IS_ATK_COMPONENT (component), NULL);

  gint count, i;
  count = atk_object_get_n_accessible_children (ATK_OBJECT (component));

  for (i = 0; i < count; i++)
    {
      AtkObject *obj;
      obj = atk_object_ref_accessible_child (ATK_OBJECT (component), i);

      if (obj != NULL)
        {
          if (atk_component_contains (ATK_COMPONENT (obj), x, y, coord_type))
            return obj;
          else
            g_object_unref (obj);
        }
    }
  return NULL;
}

static gboolean
my_atk_component_grab_focus (AtkComponent *component)
{
  return TRUE;
}

static AtkLayer
my_atk_component_get_layer (AtkComponent *component)
{
  g_return_val_if_fail (MY_IS_ATK_COMPONENT (component), -1);

  return MY_ATK_COMPONENT (component)->layer;
}

static gint
my_atk_component_get_mdi_zorder (AtkComponent *component)
{
  g_return_val_if_fail (MY_IS_ATK_COMPONENT (component), -1);

  return MY_ATK_COMPONENT (component)->zorder;
}

static gdouble
my_atk_component_get_alpha (AtkComponent *component)
{
  g_return_val_if_fail (MY_IS_ATK_COMPONENT (component), -1);

  return MY_ATK_COMPONENT (component)->alpha;
}

static void
atk_component_interface_init (AtkComponentIface *iface)
{
  g_return_if_fail (iface);

  iface->add_focus_handler = NULL;
  iface->contains = my_atk_component_contains;
  iface->ref_accessible_at_point = my_atk_component_ref_accessible_at_point;
  iface->get_extents = my_atk_component_get_extents;
  iface->get_position = NULL;
  iface->get_size = NULL;
  iface->grab_focus = my_atk_component_grab_focus;
  iface->remove_focus_handler = NULL;
  iface->set_extents = my_atk_component_set_extents;
  iface->set_position = NULL;
  iface->set_size = NULL;
  iface->get_layer = my_atk_component_get_layer;
  iface->get_mdi_zorder = my_atk_component_get_mdi_zorder;
  iface->bounds_changed = NULL;
  iface->get_alpha = my_atk_component_get_alpha;
}

static void
my_atk_component_initialize (AtkObject *obj, gpointer data)
{
}

static void
my_atk_component_finalize (GObject *object)
{
}

static void
my_atk_component_init (MyAtkComponent *obj)
{
  obj->extent.x = 0;
  obj->extent.y = 0;
  obj->extent.width = 0;
  obj->extent.height = 0;
  obj->extent_may_change = TRUE;
  obj->layer = ATK_LAYER_BACKGROUND;
  obj->zorder = -1;
  obj->alpha = 1.0;
}

static void
my_atk_component_class_init (MyAtkComponentClass *my_class)
{
  AtkObjectClass *atk_class = ATK_OBJECT_CLASS (my_class);
  GObjectClass *gobject_class = G_OBJECT_CLASS (my_class);

  gobject_class->finalize = my_atk_component_finalize;

  atk_class->initialize = my_atk_component_initialize;
}
