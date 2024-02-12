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

#include "atk_suite.h"
#include "atk_test_util.h"

#define DATA_FILE TESTS_DATA_DIR "/test-table.xml"

static void
atk_test_accessible_get_table_cell (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);

  AtspiAccessible *cell = atspi_accessible_get_child_at_index (child, 9, NULL);
  AtspiTableCell *obj = atspi_accessible_get_table_cell (cell);
  g_assert_nonnull (obj);
  g_object_unref (obj);
  g_object_unref (cell);
  g_object_unref (child);
}

static void
atk_test_table_cell_get_column_span (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);

  AtspiAccessible *cell = atspi_accessible_get_child_at_index (child, 10, NULL);
  AtspiTableCell *obj = atspi_accessible_get_table_cell (cell);
  g_assert_nonnull (obj);

  g_assert_cmpint (3, ==, atspi_table_cell_get_column_span (obj, NULL));
  g_object_unref (obj);
  g_object_unref (cell);

  cell = atspi_accessible_get_child_at_index (child, 11, NULL);
  obj = atspi_accessible_get_table_cell (cell);
  g_assert_nonnull (obj);

  g_assert_cmpint (1, ==, atspi_table_cell_get_column_span (obj, NULL));
  g_object_unref (obj);
  g_object_unref (cell);
  g_object_unref (child);
}

/*
static void
atk_test_table_cell_get_column_header_cells (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);

  AtspiAccessible *cell = atspi_accessible_get_child_at_index (child, 10, NULL);
  AtspiTableCell *obj = atspi_accessible_get_table_cell (cell);
  g_assert_nonnull (obj);

  GPtrArray *ret = atspi_table_cell_get_column_header_cells (obj, NULL);
  g_assert_cmpint (3, ==, ret->len);
  g_array_free (ret, TRUE);
  g_object_unref (obj);
  g_object_unref (cell);
  g_object_unref (child);
}
*/

static void
atk_test_table_cell_get_row_span (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);

  AtspiAccessible *cell = atspi_accessible_get_child_at_index (child, 9, NULL);
  AtspiTableCell *obj = atspi_accessible_get_table_cell (cell);
  g_assert_nonnull (obj);

  g_assert_cmpint (2, ==, atspi_table_cell_get_row_span (obj, NULL));
  g_object_unref (obj);
  g_object_unref (cell);

  cell = atspi_accessible_get_child_at_index (child, 11, NULL);
  obj = atspi_accessible_get_table_cell (cell);
  g_assert_nonnull (obj);

  g_assert_cmpint (1, ==, atspi_table_cell_get_column_span (obj, NULL));
  g_object_unref (obj);
  g_object_unref (cell);
  g_object_unref (child);
}

/*
static void
atk_test_table_cell_get_row_header_cells (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);

  AtspiAccessible *cell = atspi_accessible_get_child_at_index (child, 10, NULL);
  AtspiTableCell *obj = atspi_accessible_get_table_cell (cell);
  g_assert_nonnull (obj);

  GPtrArray *ret = atspi_table_cell_get_row_header_cells (obj, NULL);
  g_assert_cmpint (4, ==, ret->len);
  g_array_free (ret, TRUE);
  g_object_unref (obj);
  g_object_unref (cell);
  g_object_unref (child);
}
*/

static void
atk_test_table_cell_get_row_column_span (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);

  AtspiAccessible *cell = atspi_accessible_get_child_at_index (child, 9, NULL);
  AtspiTableCell *obj = atspi_accessible_get_table_cell (cell);
  g_assert_nonnull (obj);

  gint row = 10;
  gint column = 10;
  gint row_span = 10;
  gint column_span = 10;

  atspi_table_cell_get_row_column_span (obj, &row, &column, &row_span, &column_span, NULL);

  g_assert_cmpint (row, ==, 0);
  g_assert_cmpint (column, ==, 0);
  g_assert_cmpint (row_span, ==, 2);
  g_assert_cmpint (column_span, ==, 1);
  g_object_unref (obj);
  g_object_unref (cell);
  g_object_unref (child);
}

static void
atk_test_table_cell_get_position (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);

  AtspiAccessible *cell = atspi_accessible_get_child_at_index (child, 9, NULL);
  AtspiTableCell *obj = atspi_accessible_get_table_cell (cell);
  g_assert_nonnull (cell);

  gint row = 10;
  gint column = 10;

  atspi_table_cell_get_position (obj, &row, &column, NULL);

  /* TODO: not a very good test for the app to return (-1, -1) */
  g_assert_cmpint (row, ==, -1);
  g_assert_cmpint (column, ==, -1);
  g_object_unref (obj);
  g_object_unref (cell);
  g_object_unref (child);
}

static void
atk_test_table_cell_get_table (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);

  AtspiAccessible *cell = atspi_accessible_get_child_at_index (child, 9, NULL);
  AtspiTableCell *obj = atspi_accessible_get_table_cell (cell);
  g_assert_nonnull (obj);

  AtspiAccessible *tab = atspi_table_cell_get_table (obj, NULL);
  g_assert_nonnull (tab);
  g_assert_true (child == tab);
  g_object_unref (tab);
  g_object_unref (obj);
  g_object_unref (cell);
  g_object_unref (child);
}

void
atk_test_table_cell (void)
{
  g_test_add ("/table_cell/atk_test_accessible_get_table_cell",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_table_cell, fixture_teardown);
  g_test_add ("/table_cell/atk_test_table_cell_get_column_span",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_cell_get_column_span, fixture_teardown);
  /*
    g_test_add ("/table_cell/atk_test_table_cell_get_column_header_cells",
                TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_cell_get_column_header_cells, fixture_teardown);
  */
  g_test_add ("/table_cell/atk_test_table_cell_get_row_span",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_cell_get_row_span, fixture_teardown);
  /*
    g_test_add ("/table_cell/atk_test_table_cell_get_row_header_cells",
                TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_cell_get_row_header_cells, fixture_teardown);
  */
  g_test_add ("/table_cell/atk_test_table_cell_get_row_column_span",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_cell_get_row_column_span, fixture_teardown);
  g_test_add ("/table_cell/atk_test_table_cell_get_position",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_cell_get_position, fixture_teardown);
  g_test_add ("/table_cell/atk_test_table_cell_get_table",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_cell_get_table, fixture_teardown);
}
