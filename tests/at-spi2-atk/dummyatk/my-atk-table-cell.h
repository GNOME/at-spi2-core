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
#define MY_TYPE_ATK_TABLE_CELL (my_atk_tablecell_get_type ())
#define MY_ATK_TABLE_CELL(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ATK_TABLE_CELL, MyAtkTableCell))
#define MY_ATK_TABLE_CELL_CLASS (vtablecell) (G_TYPE_CHECK_CLASS_CAST ((vtablecell), MY_TYPE_ATK_TABLE_CELL, MyAtkTableCellClass))
#define MY_IS_ATK_TABLE_CELL(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ATK_TABLE_CELL))
#define MY_IS_ATK_TABLE_CELL_CLASS (vtablecell) (G_TYPE_CHECK_CLASS_TYPE ((vtablecell), MY_TYPE_ATK_TABLE_CELL))
#define MY_ATK_TABLE_CELL_GET_CLASS (inst) (G_TYPE_INSTANCE_GET_CLASS ((inst), MY_TYPE_ATK_TABLE_CELL, MyAtkTableCellClass))

typedef struct _MyAtkTableCell MyAtkTableCell;
typedef struct _MyAtkTableCellClass MyAtkTableCellClass;

struct _MyAtkTableCell
{
  MyAtkObject parent;
  MyAtkObject *parent_table;
  gpointer value;
  gint x;
  gint y;
  gint column_span;
  gint row_span;
  gint column_index;
  gboolean selected;
  gchar *row_desc;
  gint xy[2];
};

struct _MyAtkTableCellClass
{
  MyAtkObjectClass parent;
};

GType my_atk_tablecell_get_type (void);

gboolean my_atk_set_table_cell (AtkTableCell *self, gint x, gint y, gint row, gint column);
gboolean my_atk_set_tablecell (MyAtkTableCell *self, gpointer value, const gchar *row_desc, MyAtkObject *parent_table, gboolean selected, gint *xy);
