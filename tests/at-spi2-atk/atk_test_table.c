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

#include "atk_test_util.h"
#include "atk_suite.h"

#define DATA_FILE TESTS_DATA_DIR"/test-table.xml"

static void
atk_test_table_get_caption (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  AtspiAccessible *acc = atspi_table_get_caption (obj, NULL);
  g_assert (acc);
  g_assert_cmpstr ("caption name", ==, atspi_accessible_get_name (acc, NULL));

}

static void
atk_test_table_get_summary (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  AtspiAccessible *acc = atspi_table_get_summary (obj, NULL);
  g_assert (acc);
  g_assert_cmpstr ("table summary name", ==, atspi_accessible_get_name (acc, NULL));
}

static void
atk_test_table_get_n_columns (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  gint cnt = atspi_table_get_n_columns (obj, NULL);
  g_assert_cmpint (cnt, ==, 3);
}

static void
atk_test_table_get_n_rows (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  gint cnt = atspi_table_get_n_rows (obj, NULL);
  g_assert_cmpint (cnt, ==, 4);
}

static void
atk_test_table_get_accessible_at (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  AtspiAccessible *acc = atspi_table_get_accessible_at (obj, 0, 0, NULL);
  g_assert (acc);
  g_assert_cmpstr ("cell 0/0", ==, atspi_accessible_get_name (acc, NULL));

  acc = atspi_table_get_accessible_at (obj, 3, 2, NULL);
  g_assert (acc);
  g_assert_cmpstr ("cell 2/3", ==, atspi_accessible_get_name (acc, NULL));
}

static void
atk_test_table_get_index_at (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);

  gint cnt = atspi_table_get_index_at (obj, 0, 0, NULL);
  g_assert_cmpint (cnt, ==, 0);

  cnt = atspi_table_get_index_at (obj, 1, 0, NULL);
  g_assert_cmpint (cnt, ==, 3);

  cnt = atspi_table_get_index_at (obj, 0, 1, NULL);
  g_assert_cmpint (cnt, ==, 1);
}

static void
atk_test_table_get_row_at_index (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);

  gint cnt = atspi_table_get_row_at_index (obj, 1, NULL);
  g_assert_cmpint (cnt, ==, 0);

  cnt = atspi_table_get_row_at_index (obj, 4, NULL);
  g_assert_cmpint (cnt, ==, 1);

  cnt = atspi_table_get_row_at_index (obj, 6, NULL);
  g_assert_cmpint (cnt, ==, 2);

  cnt = atspi_table_get_row_at_index (obj, 11, NULL);
  g_assert_cmpint (cnt, ==, 3);
}

static void
atk_test_table_get_column_at_index (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);

  gint cnt = atspi_table_get_column_at_index (obj, 6, NULL);
  g_assert_cmpint (cnt, ==, 0);

  cnt = atspi_table_get_column_at_index (obj, 1, NULL);
  g_assert_cmpint (cnt, ==, 1);

  cnt = atspi_table_get_column_at_index (obj, 5, NULL);
  g_assert_cmpint (cnt, ==, 2);
}

static void
atk_test_table_get_row_description (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  gchar *desc = atspi_table_get_row_description (obj, 0, NULL);
  g_assert (desc);
  g_assert_cmpstr (desc, ==, "first row");
  g_free (desc);
  desc = atspi_table_get_row_description (obj, 2, NULL);
  g_assert (desc);
  g_assert_cmpstr (desc, ==, "third row");
  g_free (desc);
}

static void
atk_test_table_get_column_description (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  gchar *desc = atspi_table_get_column_description (obj, 0, NULL);
  g_assert (desc);
  g_assert_cmpstr (desc, ==, "first column");
  g_free (desc);
  desc = atspi_table_get_column_description (obj, 2, NULL);
  g_assert (desc);
  g_assert_cmpstr (desc, ==, "third column");
  g_free (desc);
}

static void
atk_test_table_get_row_extent_at (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  gint cnt = atspi_table_get_row_extent_at (obj, 1, 1, NULL);
  g_assert_cmpint (cnt, ==, 1);
}

static void
atk_test_table_get_column_extent_at (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  gint cnt = atspi_table_get_column_extent_at (obj, 1, 1, NULL);
  g_assert_cmpint (cnt, ==, 1);
}

static void
atk_test_table_get_row_header (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  AtspiAccessible *acc = atspi_table_get_row_header (obj, 0, NULL);
  g_assert (acc);
  g_assert_cmpstr ("row 1 header", ==, atspi_accessible_get_name (acc, NULL));
  acc = atspi_table_get_row_header (obj, 3, NULL);
  g_assert (acc);
  g_assert_cmpstr ("row 4 header", ==, atspi_accessible_get_name (acc, NULL));
}

static void
atk_test_table_get_column_header (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  AtspiAccessible *acc = atspi_table_get_column_header (obj, 0, NULL);
  g_assert (acc);
  g_assert_cmpstr ("column 1 header", ==, atspi_accessible_get_name (acc, NULL));
  acc = atspi_table_get_column_header (obj, 1, NULL);
  g_assert (acc);
  g_assert_cmpstr ("column 2 header", ==, atspi_accessible_get_name (acc, NULL));
  acc = atspi_table_get_column_header (obj, 2, NULL);
  g_assert (acc);
  g_assert_cmpstr ("column 3 header", ==, atspi_accessible_get_name (acc, NULL));
}

static void
atk_test_table_get_n_selected_rows (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  gint cnt = atspi_table_get_n_selected_rows (obj, NULL);
  g_assert_cmpint (cnt, ==, 2);
}

static void
atk_test_table_get_selected_rows (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  GArray *array = atspi_table_get_selected_rows (obj, NULL);
  g_assert (array);
  g_assert_cmpint (array->len, ==, 2);
  g_assert_cmpint (g_array_index (array,gint, 0), ==, 0);
  g_assert_cmpint (g_array_index (array,gint, 1), ==, 2);
  g_array_free (array, TRUE);
}

static void
atk_test_table_get_selected_columns (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  GArray *array = atspi_table_get_selected_columns (obj, NULL);
  g_assert (array);
  g_assert_cmpint (array->len, ==, 1);
  g_assert_cmpint (g_array_index (array, gint, 0), ==, 1);

  g_array_free (array, TRUE);
}

static void
atk_test_table_get_n_selected_columns (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);

  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  g_assert(obj);
  gint cnt = atspi_table_get_n_selected_columns (obj, NULL);
  g_assert_cmpint (cnt, ==, 1);
}

static void
atk_test_table_is_row_selected (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  g_assert_true (atspi_table_is_row_selected (obj, 0, NULL));
  g_assert_false (atspi_table_is_row_selected (obj, 1, NULL));
}

static void
atk_test_table_is_column_selected (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  g_assert_true (atspi_table_is_column_selected (obj, 1, NULL));
  g_assert_false (atspi_table_is_column_selected (obj, 0, NULL));
}

static void
atk_test_table_add_row_selection (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  g_assert_false (atspi_table_is_row_selected (obj, 1, NULL));
  g_assert_true (atspi_table_add_row_selection (obj, 1, NULL));
  g_assert_true (atspi_table_is_row_selected (obj, 1, NULL));
}

static void
atk_test_table_add_column_selection (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  g_assert_false (atspi_table_is_column_selected (obj, 2, NULL));
  g_assert_true (atspi_table_add_column_selection (obj, 2, NULL));
  g_assert_true (atspi_table_is_column_selected (obj, 2, NULL));
}

static void
atk_test_table_remove_row_selection (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  g_assert_true (atspi_table_is_row_selected (obj, 2, NULL));
  g_assert_true (atspi_table_remove_row_selection (obj, 2, NULL));
  g_assert_false (atspi_table_is_row_selected (obj, 2, NULL));
}

static void
atk_test_table_remove_column_selection (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  g_assert_true (atspi_table_is_column_selected (obj, 1, NULL));
  g_assert_true (atspi_table_remove_column_selection (obj, 1, NULL));
  g_assert_false (atspi_table_is_column_selected (obj, 1, NULL));
}

static void
atk_test_table_get_row_column_extents_at_index (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  gint row;
  gint col;
  gint row_ext;
  gint col_ext;
  gboolean is_selected;
  g_assert_true (atspi_table_get_row_column_extents_at_index (obj, 0, &row, &col, &row_ext, &col_ext, &is_selected, NULL));

  g_assert_cmpint (row, ==, 0);
  g_assert_cmpint (col, ==, 0);
  g_assert_cmpint (row_ext, ==, 2);
  g_assert_cmpint (col_ext, ==, 1);
  g_assert_false (is_selected);
}

static void
atk_test_table_is_selected (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  g_assert_true (atspi_table_is_selected (obj, 0, 2, NULL));
  g_assert_false (atspi_table_is_selected (obj, 1, 0, NULL));
}

static void
teardown_table_test (gpointer fixture, gconstpointer user_data)
{
  terminate_app ();
}

void
atk_test_table(void)
{
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_caption",
                     0, NULL, NULL, atk_test_table_get_caption, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_summary",
                     0, NULL, NULL, atk_test_table_get_summary, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_n_columns",
                     0, NULL, NULL, atk_test_table_get_n_columns, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_n_rows",
                     0, NULL, NULL, atk_test_table_get_n_rows, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_accessible_at",
                     0, NULL, NULL, atk_test_table_get_accessible_at, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_index_at",
                     0, NULL, NULL, atk_test_table_get_index_at, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_row_at_index",
                     0, NULL, NULL, atk_test_table_get_row_at_index, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_column_at_index",
                     0, NULL, NULL, atk_test_table_get_column_at_index, teardown_table_test);
  g_test_add_vtable  (ATK_TEST_PATH_TABLE "/atk_test_table_get_row_description",
                      0, NULL, NULL, atk_test_table_get_row_description, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_column_description",
                     0, NULL, NULL, atk_test_table_get_column_description, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_row_extent_at",
                     0, NULL, NULL, atk_test_table_get_row_extent_at, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_column_extent_at",
                     0, NULL, NULL, atk_test_table_get_column_extent_at, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_row_header",
                     0, NULL, NULL, atk_test_table_get_row_header, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_column_header",
                     0, NULL, NULL, atk_test_table_get_column_header, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_n_selected_rows",
                     0, NULL, NULL, atk_test_table_get_n_selected_rows, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_selected_rows",
                     0, NULL, NULL, atk_test_table_get_selected_rows, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_selected_columns",
                     0, NULL, NULL, atk_test_table_get_selected_columns, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_n_selected_columns",
                     0, NULL, NULL, atk_test_table_get_n_selected_columns, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_is_row_selected",
                     0, NULL, NULL, atk_test_table_is_row_selected, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_is_column_selected",
                     0, NULL, NULL, atk_test_table_is_column_selected, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_add_row_selection",
                     0, NULL, NULL, atk_test_table_add_row_selection, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_add_column_selection",
                     0, NULL, NULL, atk_test_table_add_column_selection, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_remove_row_selection",
                     0, NULL, NULL, atk_test_table_remove_row_selection, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_remove_column_selection",
                     0, NULL, NULL, atk_test_table_remove_column_selection, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_get_row_column_extents_at_index",
                     0, NULL, NULL, atk_test_table_get_row_column_extents_at_index, teardown_table_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE "/atk_test_table_is_selected",
                     0, NULL, NULL, atk_test_table_is_selected, teardown_table_test);
}
