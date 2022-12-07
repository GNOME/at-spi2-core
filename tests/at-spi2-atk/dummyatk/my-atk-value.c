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

#include <atk/atk.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>

#include "my-atk-object.h"
#include "my-atk-value.h"

typedef struct _MyAtkValueInfo MyAtkValueInfo;

static void atk_value_interface_init (AtkValueIface *iface);

G_DEFINE_TYPE_WITH_CODE (MyAtkValue,
                         my_atk_value,
                         MY_TYPE_ATK_OBJECT,
                         G_IMPLEMENT_INTERFACE (ATK_TYPE_VALUE,
                                                atk_value_interface_init));

guint
my_atk_set_value (AtkValue *obj, gdouble min, gdouble cur, gdouble max, gdouble step)
{
  MyAtkValue *self = MY_ATK_VALUE (obj);
  g_return_val_if_fail (MY_IS_ATK_VALUE (obj), -1);

  self->min = min;
  self->cur = cur;
  self->max = max;
  self->step = step;

  return 0;
}

static void
my_atk_value_get_value_and_text (AtkValue *obj, gdouble *value, gchar **text)
{
  MyAtkValue *self = MY_ATK_VALUE (obj);
  g_return_if_fail (MY_IS_ATK_VALUE (obj));

  *value = self->cur;
  *text = g_strdup_printf ("%f", self->cur);
}

static AtkRange *
my_atk_value_get_range (AtkValue *obj)
{
  MyAtkValue *self = MY_ATK_VALUE (obj);
  g_return_val_if_fail (MY_IS_ATK_VALUE (obj), NULL);

  return atk_range_new (self->min, self->max, NULL);
}

static gdouble
my_atk_value_get_increment (AtkValue *obj)
{
  MyAtkValue *self = MY_ATK_VALUE (obj);
  g_return_val_if_fail (MY_IS_ATK_VALUE (obj), 0);
  return self->step;
}

static GSList *
my_atk_value_get_sub_ranges (AtkValue *obj)
{
  g_return_val_if_fail (MY_IS_ATK_VALUE (obj), NULL);

  return NULL;
}

static void
my_atk_value_set_value (AtkValue *obj, const gdouble val)
{
  MyAtkValue *self = MY_ATK_VALUE (obj);
  g_return_if_fail (MY_IS_ATK_VALUE (obj));

  if (self->min < val && val < self->max)
    self->cur = val;
  return;
}

static void
atk_value_interface_init (AtkValueIface *iface)
{
  if (!iface)
    return;
  iface->get_value_and_text = my_atk_value_get_value_and_text;
  iface->get_range = my_atk_value_get_range;
  iface->get_increment = my_atk_value_get_increment;
  iface->get_sub_ranges = my_atk_value_get_sub_ranges;
  iface->set_value = my_atk_value_set_value;
}

static void
my_atk_value_init (MyAtkValue *self)
{
  self->min = 0;
  self->cur = 0;
  self->max = 0;
  self->step = 0;
}

static void
my_atk_value_class_initialize (AtkObject *obj, gpointer data)
{
}

static void
my_atk_value_class_finalize (GObject *obj)
{
}

static void
my_atk_value_class_init (MyAtkValueClass *my_class)
{
  AtkObjectClass *atk_class = ATK_OBJECT_CLASS (my_class);
  GObjectClass *gobject_class = G_OBJECT_CLASS (my_class);

  gobject_class->finalize = my_atk_value_class_finalize;

  atk_class->initialize = my_atk_value_class_initialize;
}
