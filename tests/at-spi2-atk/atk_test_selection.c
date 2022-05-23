/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; https://wiki.gnome.org/Accessibility)
 *
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

#define DATA_FILE TESTS_DATA_DIR"/test-selection.xml"

static void
teardown_selection_test (gpointer fixture, gconstpointer user_data)
{
  terminate_app ();
}

static void
atk_test_selection_sample_get_interface (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  g_assert_cmpstr (atspi_accessible_get_name (obj, NULL), ==, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert (iface != NULL);
}

static void
atk_test_selection_get_n_selected_children (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  g_assert_cmpstr (atspi_accessible_get_name (obj, NULL), ==, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert (iface != NULL);

  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 3);
}

static void
atk_test_selection_get_selected_child (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  g_assert_cmpstr (atspi_accessible_get_name (obj, NULL), ==, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert (iface != NULL);
  gchar* valid_names[] = { "obj2/1", "obj2/2", "obj3/1"};

  AtspiAccessible *o = NULL;
  int i=0;
  int selected_count = atspi_selection_get_n_selected_children (iface, NULL);
  for (i=0; i<selected_count; i++) {
    o = atspi_selection_get_selected_child (iface, i, NULL);
    g_assert_cmpstr (atspi_accessible_get_name (o, NULL), ==, valid_names[i]);
  }
}

static void
atk_test_selection_select_child (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  g_assert_cmpstr (atspi_accessible_get_name (obj, NULL), ==, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert (iface != NULL);

  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 3);

  atspi_selection_select_child (iface, 2, NULL);
  atspi_selection_select_child (iface, 3, NULL);
  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 5);
}

static void
atk_test_selection_deselect_selected (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  g_assert_cmpstr (atspi_accessible_get_name (obj, NULL), ==, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert (iface != NULL);

  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 3);

  atspi_selection_select_child (iface, 2, NULL);
  atspi_selection_select_child (iface, 3, NULL);
  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 5);

  atspi_selection_deselect_selected_child (iface, 2, NULL);
  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 4);
}

static void
atk_test_selection_deselect_child (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  g_assert_cmpstr (atspi_accessible_get_name (obj, NULL), ==, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert (iface != NULL);

  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 3);

  atspi_selection_select_child (iface, 2, NULL);
  atspi_selection_select_child (iface, 3, NULL);
  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 5);

  atspi_selection_deselect_child (iface, 2, NULL);
  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 4);
}

static void
atk_test_selection_is_child_selected (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  g_assert_cmpstr (atspi_accessible_get_name (obj, NULL), ==, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert (iface != NULL);

  g_assert (atspi_selection_is_child_selected (iface, 0, NULL));
  g_assert (atspi_selection_is_child_selected (iface, 1, NULL));
  g_assert (atspi_selection_is_child_selected (iface, 4, NULL));
  g_assert (!atspi_selection_is_child_selected (iface,  2, NULL));
  g_assert (!atspi_selection_is_child_selected (iface, 3, NULL));
}

static void
atk_test_selection_select_all (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  g_assert_cmpstr (atspi_accessible_get_name (obj, NULL), ==, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert (iface != NULL);

  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 3);
  atspi_selection_select_all (iface, NULL);
  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 5);
}

static void
atk_test_selection_clear_selection (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  g_assert_cmpstr (atspi_accessible_get_name (obj, NULL), ==, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert (iface != NULL);

  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 3);
  atspi_selection_clear_selection (iface, NULL);
  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 0);
}

void
atk_test_selection (void)
{
  g_test_add_vtable (ATK_TEST_PATH_SELECTION "/atk_test_selection_sample_get_interface",
                     0, NULL, NULL, atk_test_selection_sample_get_interface, teardown_selection_test);
  g_test_add_vtable (ATK_TEST_PATH_SELECTION "/atk_test_selection_get_n_selected_children",
                     0, NULL, NULL, atk_test_selection_get_n_selected_children, teardown_selection_test);
  g_test_add_vtable (ATK_TEST_PATH_SELECTION "/atk_test_selection_get_selected_child",
                     0, NULL, NULL, atk_test_selection_get_selected_child, teardown_selection_test);
  g_test_add_vtable (ATK_TEST_PATH_SELECTION "/atk_test_selection_select_child",
                     0, NULL, NULL, atk_test_selection_select_child, teardown_selection_test);
  g_test_add_vtable (ATK_TEST_PATH_SELECTION "/atk_test_selection_deselect_selected",
                     0, NULL, NULL, atk_test_selection_deselect_selected, teardown_selection_test);
  g_test_add_vtable (ATK_TEST_PATH_SELECTION "/atk_test_selection_deselect_child",
                     0, NULL, NULL, atk_test_selection_deselect_child, teardown_selection_test);
  g_test_add_vtable (ATK_TEST_PATH_SELECTION "/atk_test_selection_is_child_selected",
                     0, NULL, NULL, atk_test_selection_is_child_selected, teardown_selection_test);
  g_test_add_vtable (ATK_TEST_PATH_SELECTION "/atk_test_selection_select_all",
                     0, NULL, NULL, atk_test_selection_select_all, teardown_selection_test);
  g_test_add_vtable (ATK_TEST_PATH_SELECTION "/atk_test_selection_clear_selection",
                     0, NULL, NULL, atk_test_selection_clear_selection, teardown_selection_test);

}
