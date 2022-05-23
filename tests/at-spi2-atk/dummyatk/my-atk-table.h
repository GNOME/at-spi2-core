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

#ifndef MY_ATK_TABLE_H_
#define MY_ATK_TABLE_H_

#include <glib.h>
#include <glib-object.h>
#include <atk/atk.h>

#include "my-atk-object.h"
#define MY_TYPE_ATK_TABLE             (my_atk_table_get_type ())
#define MY_ATK_TABLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ATK_TABLE, MyAtkTable))
#define MY_ATK_TABLE_CLASS(vtable)    (G_TYPE_CHECK_CLASS_CAST ((vtable), MY_TYPE_ATK_TABLE, MyAtkTableClass))
#define MY_IS_ATK_TABLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ATK_TABLE))
#define MY_IS_ATK_TABLE_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), MY_TYPE_ATK_TABLE))
#define MY_ATK_TABLE_GET_CLASS(inst)  (G_TYPE_INSTANCE_GET_CLASS ((inst), MY_TYPE_ATK_TABLE, MyAtkTableClass))

typedef struct _MyAtkTable MyAtkTable;
typedef struct _MyAtkTableClass MyAtkTableClass;

struct _MyAtkTable {
  MyAtkObject parent;
  MyAtkTable *parent_table;
  GPtrArray *table;
  GPtrArray *column_headers;
  GPtrArray *row_headers;
  GPtrArray *row_header;
  gchar *col_desc;
  gboolean selected;
};

struct _MyAtkTableClass {
  MyAtkObjectClass parent;
};

GType my_atk_table_get_type (void);

guint my_atk_set_table (AtkTable *obj,
                        gchar *col_header,
                        gchar *row_header,
                        gchar *col_desc,
                        gchar* row_desc,
                        gchar *value,
                        gboolean selected,
                        gboolean selected_col);

AtkObject*
test_get_cell_from_table (AtkTable *obj, gint row);

#endif /* MY_ATK_TABLE_H_ */
