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
atk_test_accessible_get_table_cell (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);

  AtspiAccessible *cell = atspi_accessible_get_child_at_index (child, 9, NULL);
  AtspiTableCell *obj = atspi_accessible_get_table_cell (cell);
  g_assert (obj);
}

static void
atk_test_table_cell_get_column_span (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);

  AtspiAccessible *cell = atspi_accessible_get_child_at_index (child, 10, NULL);
  AtspiTableCell *obj = atspi_accessible_get_table_cell (cell);
  g_assert (obj);

  g_assert_cmpint (3, ==, atspi_table_cell_get_column_span (obj, NULL));

  cell = atspi_accessible_get_child_at_index (child, 11, NULL);
  obj = atspi_accessible_get_table_cell (cell);
  g_assert (obj);

  g_assert_cmpint (1, ==, atspi_table_cell_get_column_span (obj, NULL));
}

/*
static void
atk_test_table_cell_get_column_header_cells (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);

  AtspiAccessible *cell = atspi_accessible_get_child_at_index (child, 10, NULL);
  AtspiTableCell *obj = atspi_accessible_get_table_cell (cell);
  g_assert (obj);

  GPtrArray *ret = atspi_table_cell_get_column_header_cells (obj, NULL);
  g_assert_cmpint (3, ==, ret->len);
}
*/

static void
atk_test_table_cell_get_row_span (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);

  AtspiAccessible *cell = atspi_accessible_get_child_at_index (child, 9, NULL);
  AtspiTableCell *obj = atspi_accessible_get_table_cell (cell);
  g_assert (obj);

  g_assert_cmpint (2, ==, atspi_table_cell_get_row_span (obj, NULL));

  cell = atspi_accessible_get_child_at_index (child, 11, NULL);
  obj = atspi_accessible_get_table_cell (cell);
  g_assert (obj);

  g_assert_cmpint (1, ==, atspi_table_cell_get_column_span (obj, NULL));
}

/*
static void
atk_test_table_cell_get_row_header_cells (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);

  AtspiAccessible *cell = atspi_accessible_get_child_at_index (child, 10, NULL);
  AtspiTableCell *obj = atspi_accessible_get_table_cell (cell);
  g_assert (obj);

  GPtrArray *ret = atspi_table_cell_get_row_header_cells (obj, NULL);
  g_assert_cmpint (4, ==, ret->len);
}
*/

static void
atk_test_table_cell_get_row_column_span (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);

  AtspiAccessible *cell = atspi_accessible_get_child_at_index (child, 9, NULL);
  AtspiTableCell *obj = atspi_accessible_get_table_cell (cell);
  g_assert (obj);

  gint row = 10;
  gint column = 10;
  gint row_span = 10;
  gint column_span = 10;

  atspi_table_cell_get_row_column_span (obj, &row, &column, &row_span, &column_span, NULL);

  g_assert_cmpint (row, ==, 0);
  g_assert_cmpint (column, ==, 0);
  g_assert_cmpint (row_span, ==, 2);
  g_assert_cmpint (column_span, ==, 1);
}

static void
atk_test_table_cell_get_position (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);

  AtspiAccessible *cell = atspi_accessible_get_child_at_index (child, 9, NULL);
  AtspiTableCell *obj = atspi_accessible_get_table_cell (cell);
  g_assert (cell);

  gint row = 10;
  gint column = 10;

  atspi_table_cell_get_position (obj, &row, &column, NULL);

  /* TODO: not a very good test for the app to return (-1, -1) */
  g_assert_cmpint (row, ==, -1);
  g_assert_cmpint (column, ==, -1);
}

static void
atk_test_table_cell_get_table (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);

  AtspiAccessible *cell = atspi_accessible_get_child_at_index (child, 9, NULL);
  AtspiTableCell *obj = atspi_accessible_get_table_cell (cell);
  g_assert (obj);

  AtspiAccessible *tab = atspi_table_cell_get_table (obj, NULL);
  g_assert (tab);
  g_assert (child == tab);
}

static void
teardown_table_cell_test (gpointer fixture, gconstpointer user_data)
{
  terminate_app ();
}

void
atk_test_table_cell (void)
{
  g_test_add_vtable (ATK_TEST_PATH_TABLE_CELL "/atk_test_accessible_get_table_cell",
                     0, NULL, NULL, atk_test_accessible_get_table_cell, teardown_table_cell_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE_CELL "/atk_test_table_cell_get_column_span",
                     0, NULL, NULL, atk_test_table_cell_get_column_span, teardown_table_cell_test);
  /*
    g_test_add_vtable (ATK_TEST_PATH_TABLE_CELL "/atk_test_table_cell_get_column_header_cells",
                       0, NULL, NULL, atk_test_table_cell_get_column_header_cells, teardown_table_cell_test);
  */
  g_test_add_vtable (ATK_TEST_PATH_TABLE_CELL "/atk_test_table_cell_get_row_span",
                     0, NULL, NULL, atk_test_table_cell_get_row_span, teardown_table_cell_test);
  /*
    g_test_add_vtable (ATK_TEST_PATH_TABLE_CELL "/atk_test_table_cell_get_row_header_cells",
                       0, NULL, NULL, atk_test_table_cell_get_row_header_cells, teardown_table_cell_test);
  */
  g_test_add_vtable (ATK_TEST_PATH_TABLE_CELL "/atk_test_table_cell_get_row_column_span",
                     0, NULL, NULL, atk_test_table_cell_get_row_column_span, teardown_table_cell_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE_CELL "/atk_test_table_cell_get_position",
                     0, NULL, NULL, atk_test_table_cell_get_position, teardown_table_cell_test);
  g_test_add_vtable (ATK_TEST_PATH_TABLE_CELL "/atk_test_table_cell_get_table",
                     0, NULL, NULL, atk_test_table_cell_get_table, teardown_table_cell_test);


}
