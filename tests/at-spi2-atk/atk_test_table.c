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
atk_test_table_get_caption (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  AtspiAccessible *acc = atspi_table_get_caption (obj, NULL);
  check_name (acc, "caption name");
  g_object_unref (acc);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_summary (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  AtspiAccessible *acc = atspi_table_get_summary (obj, NULL);
  check_name (acc, "table summary name");
  g_object_unref (acc);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_n_columns (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  gint cnt = atspi_table_get_n_columns (obj, NULL);
  g_assert_cmpint (cnt, ==, 3);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_n_rows (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  gint cnt = atspi_table_get_n_rows (obj, NULL);
  g_assert_cmpint (cnt, ==, 4);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_accessible_at (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  AtspiAccessible *acc = atspi_table_get_accessible_at (obj, 0, 0, NULL);
  g_assert_nonnull (acc);
  check_name (acc, "cell 0/0");
  g_object_unref (acc);

  acc = atspi_table_get_accessible_at (obj, 3, 2, NULL);
  check_name (acc, "cell 2/3");
  g_object_unref (acc);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_index_at (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);

  gint cnt = atspi_table_get_index_at (obj, 0, 0, NULL);
  g_assert_cmpint (cnt, ==, 0);

  cnt = atspi_table_get_index_at (obj, 1, 0, NULL);
  g_assert_cmpint (cnt, ==, 3);

  cnt = atspi_table_get_index_at (obj, 0, 1, NULL);
  g_assert_cmpint (cnt, ==, 1);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_row_at_index (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);

  gint cnt = atspi_table_get_row_at_index (obj, 1, NULL);
  g_assert_cmpint (cnt, ==, 0);

  cnt = atspi_table_get_row_at_index (obj, 4, NULL);
  g_assert_cmpint (cnt, ==, 1);

  cnt = atspi_table_get_row_at_index (obj, 6, NULL);
  g_assert_cmpint (cnt, ==, 2);

  cnt = atspi_table_get_row_at_index (obj, 11, NULL);
  g_assert_cmpint (cnt, ==, 3);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_column_at_index (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);

  gint cnt = atspi_table_get_column_at_index (obj, 6, NULL);
  g_assert_cmpint (cnt, ==, 0);

  cnt = atspi_table_get_column_at_index (obj, 1, NULL);
  g_assert_cmpint (cnt, ==, 1);

  cnt = atspi_table_get_column_at_index (obj, 5, NULL);
  g_assert_cmpint (cnt, ==, 2);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_row_description (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  gchar *desc = atspi_table_get_row_description (obj, 0, NULL);
  g_assert_nonnull (desc);
  g_assert_cmpstr (desc, ==, "first row");
  g_free (desc);
  desc = atspi_table_get_row_description (obj, 2, NULL);
  g_assert_nonnull (desc);
  g_assert_cmpstr (desc, ==, "third row");
  g_free (desc);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_column_description (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  gchar *desc = atspi_table_get_column_description (obj, 0, NULL);
  g_assert_nonnull (desc);
  g_assert_cmpstr (desc, ==, "first column");
  g_free (desc);
  desc = atspi_table_get_column_description (obj, 2, NULL);
  g_assert_nonnull (desc);
  g_assert_cmpstr (desc, ==, "third column");
  g_free (desc);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_row_extent_at (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  gint cnt = atspi_table_get_row_extent_at (obj, 1, 1, NULL);
  g_assert_cmpint (cnt, ==, 1);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_column_extent_at (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  gint cnt = atspi_table_get_column_extent_at (obj, 1, 1, NULL);
  g_assert_cmpint (cnt, ==, 1);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_row_header (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  AtspiAccessible *acc = atspi_table_get_row_header (obj, 0, NULL);
  check_name (acc, "row 1 header");
  g_object_unref (acc);
  acc = atspi_table_get_row_header (obj, 3, NULL);
  check_name (acc, "row 4 header");
  g_object_unref (acc);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_column_header (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  AtspiAccessible *acc = atspi_table_get_column_header (obj, 0, NULL);
  check_name (acc, "column 1 header");
  g_object_unref (acc);
  acc = atspi_table_get_column_header (obj, 1, NULL);
  check_name (acc, "column 2 header");
  g_object_unref (acc);
  acc = atspi_table_get_column_header (obj, 2, NULL);
  check_name (acc, "column 3 header");
  g_object_unref (acc);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_n_selected_rows (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  gint cnt = atspi_table_get_n_selected_rows (obj, NULL);
  g_assert_cmpint (cnt, ==, 2);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_selected_rows (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  GArray *array = atspi_table_get_selected_rows (obj, NULL);
  g_assert_nonnull (array);
  g_assert_cmpint (array->len, ==, 2);
  g_assert_cmpint (g_array_index (array, gint, 0), ==, 0);
  g_assert_cmpint (g_array_index (array, gint, 1), ==, 2);
  g_array_free (array, TRUE);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_selected_columns (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  GArray *array = atspi_table_get_selected_columns (obj, NULL);
  g_assert_nonnull (array);
  g_assert_cmpint (array->len, ==, 1);
  g_assert_cmpint (g_array_index (array, gint, 0), ==, 1);

  g_array_free (array, TRUE);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_n_selected_columns (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);

  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  g_assert_nonnull (obj);
  gint cnt = atspi_table_get_n_selected_columns (obj, NULL);
  g_assert_cmpint (cnt, ==, 1);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_is_row_selected (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  g_assert_nonnull (obj);
  g_assert_true (atspi_table_is_row_selected (obj, 0, NULL));
  g_assert_false (atspi_table_is_row_selected (obj, 1, NULL));
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_is_column_selected (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  g_assert_true (atspi_table_is_column_selected (obj, 1, NULL));
  g_assert_false (atspi_table_is_column_selected (obj, 0, NULL));
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_add_row_selection (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  g_assert_false (atspi_table_is_row_selected (obj, 1, NULL));
  g_assert_true (atspi_table_add_row_selection (obj, 1, NULL));
  g_assert_true (atspi_table_is_row_selected (obj, 1, NULL));
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_add_column_selection (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  g_assert_false (atspi_table_is_column_selected (obj, 2, NULL));
  g_assert_true (atspi_table_add_column_selection (obj, 2, NULL));
  g_assert_true (atspi_table_is_column_selected (obj, 2, NULL));
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_remove_row_selection (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  g_assert_true (atspi_table_is_row_selected (obj, 2, NULL));
  g_assert_true (atspi_table_remove_row_selection (obj, 2, NULL));
  g_assert_false (atspi_table_is_row_selected (obj, 2, NULL));
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_remove_column_selection (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  g_assert_true (atspi_table_is_column_selected (obj, 1, NULL));
  g_assert_true (atspi_table_remove_column_selection (obj, 1, NULL));
  g_assert_false (atspi_table_is_column_selected (obj, 1, NULL));
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_get_row_column_extents_at_index (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
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
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_table_is_selected (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiTable *obj = atspi_accessible_get_table_iface (child);
  g_assert_true (atspi_table_is_selected (obj, 0, 2, NULL));
  g_assert_false (atspi_table_is_selected (obj, 1, 0, NULL));
  g_object_unref (obj);
  g_object_unref (child);
}

void
atk_test_table (void)
{
  g_test_add ("/table/atk_test_table_get_caption",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_caption, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_summary",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_summary, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_n_columns",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_n_columns, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_n_rows",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_n_rows, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_accessible_at",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_accessible_at, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_index_at",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_index_at, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_row_at_index",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_row_at_index, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_column_at_index",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_column_at_index, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_row_description",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_row_description, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_column_description",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_column_description, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_row_extent_at",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_row_extent_at, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_column_extent_at",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_column_extent_at, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_row_header",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_row_header, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_column_header",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_column_header, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_n_selected_rows",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_n_selected_rows, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_selected_rows",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_selected_rows, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_selected_columns",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_selected_columns, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_n_selected_columns",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_n_selected_columns, fixture_teardown);
  g_test_add ("/table/atk_test_table_is_row_selected",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_is_row_selected, fixture_teardown);
  g_test_add ("/table/atk_test_table_is_column_selected",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_is_column_selected, fixture_teardown);
  g_test_add ("/table/atk_test_table_add_row_selection",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_add_row_selection, fixture_teardown);
  g_test_add ("/table/atk_test_table_add_column_selection",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_add_column_selection, fixture_teardown);
  g_test_add ("/table/atk_test_table_remove_row_selection",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_remove_row_selection, fixture_teardown);
  g_test_add ("/table/atk_test_table_remove_column_selection",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_remove_column_selection, fixture_teardown);
  g_test_add ("/table/atk_test_table_get_row_column_extents_at_index",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_get_row_column_extents_at_index, fixture_teardown);
  g_test_add ("/table/atk_test_table_is_selected",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_table_is_selected, fixture_teardown);
}
