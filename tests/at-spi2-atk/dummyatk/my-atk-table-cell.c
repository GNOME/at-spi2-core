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
#include <string.h>

#include "my-atk-object.h"
#include "my-atk-table-cell.h"
#include "my-atk-table.h"

typedef struct _MyAtkTableCellInfo MyAtkTableCellInfo;

static void atk_tablecell_interface_init (AtkTableCellIface *iface);

G_DEFINE_TYPE_WITH_CODE (MyAtkTableCell,
                         my_atk_tablecell,
                         MY_TYPE_ATK_OBJECT,
                         G_IMPLEMENT_INTERFACE (ATK_TYPE_TABLE_CELL,
                                                atk_tablecell_interface_init));

gboolean
my_atk_set_table_cell (AtkTableCell *cell, gint x, gint y, gint row_span, gint column_span)
{
  g_return_val_if_fail (MY_IS_ATK_TABLE_CELL (cell), FALSE);
  MyAtkTableCell *self = MY_ATK_TABLE_CELL (cell);

  self->x = x;
  self->y = y;
  self->row_span = row_span;
  self->column_span = column_span;

  /* Default value for span is 1, so that condition is needed */
  if (row_span == 0)
    self->row_span = 1;
  if (column_span == 0)
    self->column_span = 1;

  return TRUE;
}
gboolean
my_atk_set_tablecell (MyAtkTableCell *self, gpointer value, const gchar *row_desc, MyAtkObject *parent_table, gboolean selected, gint *xy)
{
  self->value = value;
  self->row_desc = g_strdup (row_desc);
  self->parent_table = parent_table;
  self->selected = selected;

  memcpy (self->xy, xy, sizeof (self->xy));
  return TRUE;
}

static gint
my_atk_tablecell_get_column_span (AtkTableCell *obj)
{
  g_return_val_if_fail (MY_IS_ATK_TABLE_CELL (obj), -1);
  MyAtkTableCell *self = MY_ATK_TABLE_CELL (obj);
  return self->column_span;
}

static gboolean
my_atk_tablecell_get_row_column_span (AtkTableCell *obj, gint *row, gint *col, gint *row_span, gint *col_span)
{
  g_return_val_if_fail (MY_IS_ATK_TABLE_CELL (obj), FALSE);
  MyAtkTableCell *self = MY_ATK_TABLE_CELL (obj);
  *col = self->x;
  *row = self->y;
  *row_span = self->row_span;
  *col_span = self->column_span;
  return TRUE;
}

static GPtrArray *
my_atk_tablecell_get_column_header_cells (AtkTableCell *obj)
{
  g_return_val_if_fail (MY_IS_ATK_TABLE_CELL (obj), FALSE);
  MyAtkTable *tab = MY_ATK_TABLE (atk_object_get_parent (ATK_OBJECT (obj)));

  gint i, all_child;
  all_child = MY_ATK_OBJECT (tab)->children->len;
  AtkObject *child = NULL;
  GPtrArray *ret = g_ptr_array_new_full (atk_table_get_n_columns ATK_TABLE (tab), g_object_unref);

  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (tab), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_COLUMN_HEADER)
        {
          g_ptr_array_add (ret, child);
        }
    }

  return ret;
}

static AtkObject *
my_atk_tablecell_get_table (AtkTableCell *obj)
{
  g_return_val_if_fail (MY_IS_ATK_TABLE_CELL (obj), NULL);

  return atk_object_get_parent (ATK_OBJECT (obj));
}

static gint
my_atk_tablecell_get_row_span (AtkTableCell *obj)
{
  g_return_val_if_fail (MY_IS_ATK_TABLE_CELL (obj), -1);
  MyAtkTableCell *self = MY_ATK_TABLE_CELL (obj);
  return self->row_span;
}

static gboolean
my_atk_tablecell_get_position (AtkTableCell *obj, gint *row, gint *column)
{
  MyAtkTableCell *self = MY_ATK_TABLE_CELL (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE_CELL (obj), FALSE);

  *row = self->xy[0];
  *column = self->xy[1];

  return TRUE;
}

static GPtrArray *
my_atk_tablecell_get_row_header_cells (AtkTableCell *obj)
{
  g_return_val_if_fail (MY_IS_ATK_TABLE_CELL (obj), FALSE);
  MyAtkTable *tab = MY_ATK_TABLE (atk_object_get_parent (ATK_OBJECT (obj)));

  gint i, all_child;
  all_child = MY_ATK_OBJECT (tab)->children->len;
  AtkObject *child = NULL;
  GPtrArray *ret = g_ptr_array_new_full (atk_table_get_n_columns ATK_TABLE (tab), g_object_unref);

  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (tab), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_ROW_HEADER)
        {
          g_ptr_array_add (ret, child);
        }
    }

  return ret;
}

static void
atk_tablecell_interface_init (AtkTableCellIface *iface)
{
  if (!iface)
    return;
  iface->get_column_span = my_atk_tablecell_get_column_span;
  iface->get_column_header_cells = my_atk_tablecell_get_column_header_cells;
  iface->get_position = my_atk_tablecell_get_position;
  iface->get_row_span = my_atk_tablecell_get_row_span;
  iface->get_row_header_cells = my_atk_tablecell_get_row_header_cells;
  iface->get_row_column_span = my_atk_tablecell_get_row_column_span;
  iface->get_table = my_atk_tablecell_get_table;
}

static void
my_atk_tablecell_init (MyAtkTableCell *self)
{
  self->value = NULL;
  self->parent_table = NULL;
  self->row_desc = NULL;
  self->selected = FALSE;
  memset (self->xy, -1, sizeof (self->xy));
  self->column_span = 1;
  self->row_span = 1;
  self->x = -1;
  self->y = -1;
  self->column_index = -1;
}

static void
my_atk_tablecell_class_initialize (AtkObject *obj, gpointer data)
{
}

static void
my_atk_tablecell_class_finalize (GObject *obj)
{
}

static void
my_atk_tablecell_class_init (MyAtkTableCellClass *my_class)
{
  AtkObjectClass *atk_class = ATK_OBJECT_CLASS (my_class);
  GObjectClass *gobject_class = G_OBJECT_CLASS (my_class);

  gobject_class->finalize = my_atk_tablecell_class_finalize;

  atk_class->initialize = my_atk_tablecell_class_initialize;
}
