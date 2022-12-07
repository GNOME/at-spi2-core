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

typedef struct _MyAtkTableInfo MyAtkTableInfo;

static void GDestroyNotifyGPTRARRAYptrArray (gpointer data);
static void atk_table_interface_init (AtkTableIface *iface);

G_DEFINE_TYPE_WITH_CODE (MyAtkTable,
                         my_atk_table,
                         MY_TYPE_ATK_OBJECT,
                         G_IMPLEMENT_INTERFACE (ATK_TYPE_TABLE,
                                                atk_table_interface_init));

static gint
my_atk_table_get_index_at (AtkTable *obj, gint row, gint column)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), -1);
  gint i, all_child, index_first_cell = -1;
  gint ret = -1;

  all_child = MY_ATK_OBJECT (self)->children->len;
  AtkObject *child = NULL;

  MyAtkTableCell *cell = NULL;
  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_CELL)
        {
          if (index_first_cell == -1)
            {
              index_first_cell = i;
            }
          cell = MY_ATK_TABLE_CELL (child);
          if (cell->x == column && cell->y == row)
            {
              ret = i - index_first_cell;
            }
        }
    }
  return ret;
}

static gint
my_atk_table_get_column_at_index (AtkTable *obj, gint index)
{
  MyAtkTable *table = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), -1);
  gint columns = -1;
  gint rows = -1;
  gint i, j;

  columns = atk_table_get_n_columns (ATK_TABLE (table));
  rows = atk_table_get_n_rows (ATK_TABLE (table));

  for (i = 0; i < rows; i++)
    {
      for (j = 0; j < columns; j++)
        {
          if (index == my_atk_table_get_index_at (obj, i, j))
            return j;
        }
    }
  return -1;
}

static gint
my_atk_table_get_row_at_index (AtkTable *obj, gint index)
{
  MyAtkTable *table = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), -1);
  gint columns = -1;
  gint rows = -1;
  gint i, j;

  columns = atk_table_get_n_columns (ATK_TABLE (table));
  rows = atk_table_get_n_rows (ATK_TABLE (table));

  for (i = 0; i < rows; i++)
    {
      for (j = 0; j < columns; j++)
        {
          if (index == my_atk_table_get_index_at (obj, i, j))
            return i;
        }
    }

  return -1;
}

static gint
my_atk_table_get_n_columns (AtkTable *obj)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), -1);
  gint i, all_child, ret = 0;

  all_child = MY_ATK_OBJECT (self)->children->len;
  AtkObject *child = NULL;
  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_COLUMN_HEADER)
        ret++;
    }
  return ret;
}

static gint
my_atk_table_get_n_rows (AtkTable *obj)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), -1);
  gint i, all_child, ret = 0;

  all_child = MY_ATK_OBJECT (self)->children->len;
  AtkObject *child = NULL;
  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_ROW_HEADER)
        ret++;
    }
  return ret;
}

static AtkObject *
my_atk_table_get_caption (AtkTable *obj)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);

  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), NULL);
  gint i, all_child;
  AtkObject *caption = NULL;

  all_child = MY_ATK_OBJECT (self)->children->len;
  AtkObject *child = NULL;
  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_CAPTION)
        caption = child;
    }
  return caption ? caption : NULL;
}

static AtkObject *
my_atk_table_ref_at (AtkTable *obj, gint row, gint column)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), NULL);
  gint i, all_child;
  AtkObject *ret = NULL;

  all_child = MY_ATK_OBJECT (self)->children->len;
  AtkObject *child = NULL;

  MyAtkTableCell *cell = NULL;
  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_CELL)
        {
          cell = MY_ATK_TABLE_CELL (child);
          if (cell->x == column && cell->y == row)
            ret = ATK_OBJECT (cell);
        }
    }
  return ret ? ret : NULL;
}

static const gchar *
my_atk_table_get_row_description (AtkTable *obj, gint index)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), NULL);
  gint i, all_child;
  GPtrArray *ret = g_ptr_array_new_full (my_atk_table_get_n_rows (obj),
                                         g_object_unref);

  all_child = MY_ATK_OBJECT (self)->children->len;
  AtkObject *child = NULL;

  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_ROW_HEADER)
        g_ptr_array_add (ret, child);
    }
  child = g_ptr_array_index (ret, index);

  return g_strdup (atk_object_get_description (child));
}

static const gchar *
my_atk_table_get_column_description (AtkTable *obj, gint index)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), NULL);
  gint i, all_child;
  GPtrArray *ret = g_ptr_array_new_full (my_atk_table_get_n_columns (obj), g_object_unref);

  all_child = MY_ATK_OBJECT (self)->children->len;
  AtkObject *child = NULL;

  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_COLUMN_HEADER)
        g_ptr_array_add (ret, child);
    }
  child = g_ptr_array_index (ret, index);

  return g_strdup (atk_object_get_description (child));
}

static gint
my_atk_table_test_table_get_row_extent_at (AtkTable *obj, gint row, gint col)
{
  AtkObject *cell = my_atk_table_ref_at (obj, row, col);

  MyAtkTableCell *self = MY_ATK_TABLE_CELL (cell);

  return self->row_span;
}

static gint
my_atk_table_test_table_get_column_extent_at (AtkTable *obj, gint row, gint col)
{
  AtkObject *cell = my_atk_table_ref_at (obj, row, col);

  MyAtkTableCell *self = MY_ATK_TABLE_CELL (cell);

  return self->column_span;
}

static AtkObject *
my_atk_table_get_row_header (AtkTable *obj, gint row)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), NULL);
  gint i, all_child;
  GPtrArray *ret = g_ptr_array_new_full (my_atk_table_get_n_rows (obj), g_object_unref);

  all_child = MY_ATK_OBJECT (self)->children->len;
  AtkObject *child = NULL;
  AtkObject *c = NULL;

  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_ROW_HEADER)
        g_ptr_array_add (ret, child);
    }
  c = ATK_OBJECT (g_ptr_array_index (ret, row));

  return atk_object_ref_accessible_child (c, 0);
}

static AtkObject *
my_atk_table_get_column_header (AtkTable *obj, gint col)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), NULL);
  gint i, all_child;
  GPtrArray *ret = g_ptr_array_new_full (my_atk_table_get_n_rows (obj), g_object_unref);

  all_child = MY_ATK_OBJECT (self)->children->len;
  AtkObject *child = NULL;
  AtkObject *c = NULL;

  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_COLUMN_HEADER)
        g_ptr_array_add (ret, child);
    }
  c = g_ptr_array_index (ret, col);

  return atk_object_ref_accessible_child (c, 0);
}

gint
my_atk_table_get_selected_rows (AtkTable *obj, gint **selected)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), -1);
  gint i, all_child, row = 0, ret = 0;
  AtkObject *child = NULL;
  AtkStateSet *ss = NULL;

  GArray *array = g_array_new (FALSE, FALSE, sizeof (gint));

  all_child = MY_ATK_OBJECT (self)->children->len;
  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_ROW_HEADER)
        {
          ss = atk_object_ref_state_set (child);
          if (atk_state_set_contains_state (ss, ATK_STATE_SELECTED))
            {
              ret++;
              g_array_append_val (array, row);
            }
          row++;
        }
    }
  if (selected)
    *selected = (gint *) g_array_free (array, FALSE);
  return ret;
}

gint
my_atk_table_get_selected_columns (AtkTable *obj, gint **selected)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), -1);
  gint i, all_child, column = 0, ret = 0;
  AtkObject *child = NULL;
  AtkStateSet *ss = NULL;

  GArray *array = g_array_new (FALSE, FALSE, sizeof (gint));

  all_child = MY_ATK_OBJECT (self)->children->len;
  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_COLUMN_HEADER)
        {
          ss = atk_object_ref_state_set (child);
          if (atk_state_set_contains_state (ss, ATK_STATE_SELECTED))
            {
              ret++;
              g_array_append_val (array, column);
            }
          column++;
        }
    }
  if (selected)
    *selected = (gint *) g_array_free (array, FALSE);
  return ret;
}

static gboolean
my_atk_table_is_row_selected (AtkTable *obj, gint row)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), FALSE);
  gint i, all_child;
  AtkObject *child = NULL;
  AtkObject *c = NULL;
  AtkStateSet *ss = NULL;
  GPtrArray *ret = g_ptr_array_new_full (my_atk_table_get_n_rows (obj), g_object_unref);

  all_child = MY_ATK_OBJECT (self)->children->len;
  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_ROW_HEADER)
        g_ptr_array_add (ret, child);
    }
  c = g_ptr_array_index (ret, row);
  ss = atk_object_ref_state_set (c);
  if (atk_state_set_contains_state (ss, ATK_STATE_SELECTED))
    return TRUE;
  return FALSE;
}

static gboolean
my_atk_table_is_column_selected (AtkTable *obj, gint col)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), FALSE);
  gint i, all_child;
  AtkObject *child = NULL;
  AtkObject *c = NULL;
  AtkStateSet *ss = NULL;
  GPtrArray *ret = g_ptr_array_new_full (my_atk_table_get_n_columns (obj), g_object_unref);

  all_child = MY_ATK_OBJECT (self)->children->len;
  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_COLUMN_HEADER)
        g_ptr_array_add (ret, child);
    }
  c = g_ptr_array_index (ret, col);
  ss = atk_object_ref_state_set (c);
  if (atk_state_set_contains_state (ss, ATK_STATE_SELECTED))
    return TRUE;
  return FALSE;
}

static gboolean
my_atk_table_is_selected (AtkTable *obj, gint row, gint col)
{
  AtkObject *cell = atk_table_ref_at (obj, row, col);
  AtkStateSet *ss = atk_object_ref_state_set (cell);
  gboolean ret = FALSE;

  if (atk_state_set_contains_state (ss, ATK_STATE_SELECTED))
    ret = TRUE;
  return ret;
}

static gboolean
my_atk_table_add_column_selection (AtkTable *obj, gint col)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), FALSE);
  gint i, all_child, counter = 0;
  AtkObject *child = NULL;
  AtkStateSet *ss = NULL;

  all_child = MY_ATK_OBJECT (self)->children->len;
  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_COLUMN_HEADER)
        {
          if (col == counter)
            {
              ss = atk_object_ref_state_set (child);
              if (!atk_state_set_contains_state (ss, ATK_STATE_SELECTED))
                {
                  atk_state_set_add_state (ss, ATK_STATE_SELECTED);
                  return TRUE;
                }
            }
          counter++;
        }
    }
  return FALSE;
}

static gboolean
my_atk_table_add_row_selection (AtkTable *obj, gint row)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), FALSE);
  gint i, all_child, counter = 0;
  AtkObject *child = NULL;
  AtkStateSet *ss = NULL;

  all_child = MY_ATK_OBJECT (self)->children->len;
  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_ROW_HEADER)
        {
          if (row == counter)
            {
              ss = atk_object_ref_state_set (child);
              if (!atk_state_set_contains_state (ss, ATK_STATE_SELECTED))
                {
                  atk_state_set_add_state (ss, ATK_STATE_SELECTED);
                  return TRUE;
                }
            }
          counter++;
        }
    }
  return FALSE;
}

static gboolean
my_atk_table_remove_column_selection (AtkTable *obj, gint col)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), FALSE);
  gint i, all_child, counter = 0;
  AtkObject *child = NULL;
  AtkStateSet *ss = NULL;

  all_child = MY_ATK_OBJECT (self)->children->len;
  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_COLUMN_HEADER)
        {
          if (col == counter)
            {
              ss = atk_object_ref_state_set (child);
              if (atk_state_set_contains_state (ss, ATK_STATE_SELECTED))
                {
                  atk_state_set_remove_state (ss, ATK_STATE_SELECTED);
                  return TRUE;
                }
            }
          counter++;
        }
    }
  return FALSE;
}

static gboolean
my_atk_table_remove_row_selection (AtkTable *obj, gint row)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), FALSE);
  gint i, all_child, counter = 0;
  AtkObject *child = NULL;
  AtkStateSet *ss = NULL;

  all_child = MY_ATK_OBJECT (self)->children->len;
  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_TABLE_ROW_HEADER)
        {
          if (row == counter)
            {
              ss = atk_object_ref_state_set (child);
              if (atk_state_set_contains_state (ss, ATK_STATE_SELECTED))
                {
                  atk_state_set_remove_state (ss, ATK_STATE_SELECTED);
                  return TRUE;
                }
            }
          counter++;
        }
    }
  return FALSE;
}

static AtkObject *
my_atk_table_get_summary (AtkTable *obj)
{
  MyAtkTable *self = MY_ATK_TABLE (obj);
  g_return_val_if_fail (MY_IS_ATK_TABLE (obj), NULL);
  gint i, all_child;
  AtkObject *summary = NULL;

  all_child = MY_ATK_OBJECT (self)->children->len;
  AtkObject *child = NULL;
  for (i = 0; i < all_child; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (obj), i);
      if (atk_object_get_role (child) == ATK_ROLE_HEADING)
        summary = child;
    }
  return summary ? summary : NULL;
}

static void
my_atk_table_set_column_header (AtkTable *obj, gint column, AtkObject *header)
{
}

static void
my_atk_table_set_column_description (AtkTable *obj, gint column, const gchar *desc)
{
}

static void
my_atk_table_set_caption (AtkTable *obj, AtkObject *caption)
{
}

static void
my_atk_table_set_row_description (AtkTable *obj, gint row, const gchar *desc)
{
}

static void
my_atk_table_set_row_header (AtkTable *obj, gint column, AtkObject *header)
{
}

static void
my_atk_table_set_summary (AtkTable *table, AtkObject *accessible)
{
}

static void
my_atk_table_row_inserted (AtkTable *table, gint row, gint num_inserted)
{
}

static void
my_atk_table_column_inserted (AtkTable *table, gint column, gint num_inserted)
{
}

static void
my_atk_table_row_deleted (AtkTable *table, gint row, gint num_deleted)
{
}

static void
my_atk_table_column_deleted (AtkTable *table, gint column, gint num_deleted)
{
}

static void
my_atk_table_row_reordered (AtkTable *table)
{
}

static void
my_atk_table_column_reordered (AtkTable *table)
{
}

static void
my_atk_table_model_changed (AtkTable *table)
{
}

static void
atk_table_interface_init (AtkTableIface *iface)
{

  if (!iface)
    return;

  iface->ref_at = my_atk_table_ref_at;

  /* DEPRICATED BUT NOT IN ATSPI */
  iface->get_index_at = my_atk_table_get_index_at;
  iface->get_column_at_index = my_atk_table_get_column_at_index;
  iface->get_row_at_index = my_atk_table_get_row_at_index;
  /* DEPRICATED */

  iface->get_n_columns = my_atk_table_get_n_columns;
  iface->get_n_rows = my_atk_table_get_n_rows;
  iface->get_column_extent_at = my_atk_table_test_table_get_column_extent_at;
  iface->get_row_extent_at = my_atk_table_test_table_get_row_extent_at;
  iface->get_caption = my_atk_table_get_caption;
  iface->get_column_description = my_atk_table_get_column_description;
  iface->get_column_header = my_atk_table_get_column_header;
  iface->get_row_description = my_atk_table_get_row_description;
  iface->get_row_header = my_atk_table_get_row_header;
  iface->get_summary = my_atk_table_get_summary;
  iface->set_caption = my_atk_table_set_caption;
  iface->set_column_description = my_atk_table_set_column_description;
  iface->set_column_header = my_atk_table_set_column_header;
  iface->set_row_description = my_atk_table_set_row_description;
  iface->set_row_header = my_atk_table_set_row_header;
  iface->set_summary = my_atk_table_set_summary;
  iface->get_selected_columns = my_atk_table_get_selected_columns;
  iface->get_selected_rows = my_atk_table_get_selected_rows;
  iface->is_column_selected = my_atk_table_is_column_selected;
  iface->is_row_selected = my_atk_table_is_row_selected;
  iface->is_selected = my_atk_table_is_selected;
  iface->add_row_selection = my_atk_table_add_row_selection;
  iface->remove_row_selection = my_atk_table_remove_row_selection;
  iface->add_column_selection = my_atk_table_add_column_selection;
  iface->remove_column_selection = my_atk_table_remove_column_selection;
  iface->row_inserted = my_atk_table_row_inserted;
  iface->column_inserted = my_atk_table_column_inserted;
  iface->row_deleted = my_atk_table_row_deleted;
  iface->column_deleted = my_atk_table_column_deleted;
  iface->row_reordered = my_atk_table_row_reordered;
  iface->column_reordered = my_atk_table_column_reordered;
  iface->model_changed = my_atk_table_model_changed;
}

static void
my_atk_table_init (MyAtkTable *self)
{
  self->parent_table = NULL;
  self->table = g_ptr_array_new_with_free_func (GDestroyNotifyGPTRARRAYptrArray);
  self->row_header = g_ptr_array_new_with_free_func (GDestroyNotifyGPTRARRAYptrArray);
  self->column_headers = g_ptr_array_new_with_free_func (GDestroyNotifyGPTRARRAYptrArray);
  self->selected = FALSE;
  self->col_desc = NULL;
}

static void
GDestroyNotifyGPTRARRAYptrArray (gpointer data)
{
  g_ptr_array_free (data, TRUE);
}

static void
my_atk_table_class_initialize (AtkObject *obj, gpointer data)
{
}

static void
my_atk_table_class_finalize (GObject *obj)
{
}

static void
my_atk_table_class_init (MyAtkTableClass *my_class)
{
  AtkObjectClass *atk_class = ATK_OBJECT_CLASS (my_class);
  GObjectClass *gobject_class = G_OBJECT_CLASS (my_class);

  gobject_class->finalize = my_atk_table_class_finalize;

  atk_class->initialize = my_atk_table_class_initialize;
}
