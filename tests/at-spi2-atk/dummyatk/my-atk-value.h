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

#ifndef MY_ATK_VALUE_H_
#define MY_ATK_VALUE_H_

#include <glib.h>
#include <glib-object.h>
#include <atk/atk.h>

#include "my-atk-object.h"
#define MY_TYPE_ATK_VALUE             (my_atk_value_get_type ())
#define MY_ATK_VALUE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ATK_VALUE, MyAtkValue))
#define MY_ATK_VALUE_CLASS(vtable)    (G_TYPE_CHECK_CLASS_CAST ((vtable), MY_TYPE_ATK_VALUE, MyAtkValueClass))
#define MY_IS_ATK_VALUE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ATK_VALUE))
#define MY_IS_ATK_VALUE_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), MY_TYPE_ATK_VALUE))
#define MY_ATK_VALUE_GET_CLASS(inst)  (G_TYPE_INSTANCE_GET_CLASS ((inst), MY_TYPE_ATK_VALUE, MyAtkValueClass))

typedef struct _MyAtkValue MyAtkValue;
typedef struct _MyAtkValueClass MyAtkValueClass;

struct _MyAtkValue {
  MyAtkObject parent;
  gdouble min;
  gdouble cur;
  gdouble max;
  gdouble step;
};

struct _MyAtkValueClass {
  MyAtkObjectClass parent;
};

GType my_atk_value_get_type (void);

guint my_atk_set_value (AtkValue *obj, gdouble min, gdouble cur, gdouble max, gdouble step);

#endif /* MY_ATK_VALUE_H_ */
