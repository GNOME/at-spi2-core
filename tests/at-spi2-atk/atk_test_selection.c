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

#define DATA_FILE TESTS_DATA_DIR "/test-selection.xml"

static void
atk_test_selection_sample_get_interface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  check_name (obj, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert_nonnull (iface);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_selection_get_n_selected_children (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  check_name (obj, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert_nonnull (iface);

  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 3);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_selection_get_selected_child (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  check_name (obj, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert_nonnull (iface);
  gchar *valid_names[] = { "obj2/1", "obj2/2", "obj3/1" };

  AtspiAccessible *o = NULL;
  int i = 0;
  int selected_count = atspi_selection_get_n_selected_children (iface, NULL);
  g_assert_cmpint (selected_count, ==, 3);
  for (i = 0; i < selected_count; i++)
    {
      o = atspi_selection_get_selected_child (iface, i, NULL);
      check_name (o, valid_names[i]);
      g_object_unref (o);
    }
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_selection_select_child (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  check_name (obj, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert_nonnull (iface);

  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 3);

  atspi_selection_select_child (iface, 2, NULL);
  atspi_selection_select_child (iface, 3, NULL);
  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 5);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_selection_deselect_selected (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  check_name (obj, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert_nonnull (iface);

  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 3);

  atspi_selection_select_child (iface, 2, NULL);
  atspi_selection_select_child (iface, 3, NULL);
  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 5);

  atspi_selection_deselect_selected_child (iface, 2, NULL);
  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 4);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_selection_deselect_child (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  check_name (obj, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert_nonnull (iface);

  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 3);

  atspi_selection_select_child (iface, 2, NULL);
  atspi_selection_select_child (iface, 3, NULL);
  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 5);

  atspi_selection_deselect_child (iface, 2, NULL);
  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 4);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_selection_is_child_selected (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  check_name (obj, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert_nonnull (iface);

  g_assert_true (atspi_selection_is_child_selected (iface, 0, NULL));
  g_assert_true (atspi_selection_is_child_selected (iface, 1, NULL));
  g_assert_true (atspi_selection_is_child_selected (iface, 4, NULL));
  g_assert_true (!atspi_selection_is_child_selected (iface, 2, NULL));
  g_assert_true (!atspi_selection_is_child_selected (iface, 3, NULL));
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_selection_select_all (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  check_name (obj, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert_nonnull (iface);

  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 3);
  atspi_selection_select_all (iface, NULL);
  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 5);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_selection_clear_selection (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  check_name (obj, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (child);
  g_assert_nonnull (iface);

  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 3);
  atspi_selection_clear_selection (iface, NULL);
  g_assert_cmpint (atspi_selection_get_n_selected_children (iface, NULL), ==, 0);
  g_object_unref (iface);
  g_object_unref (child);
}

void
atk_test_selection (void)
{
  g_test_add ("/selection/atk_test_selection_sample_get_interface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_selection_sample_get_interface, fixture_teardown);
  g_test_add ("/selection/atk_test_selection_get_n_selected_children",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_selection_get_n_selected_children, fixture_teardown);
  g_test_add ("/selection/atk_test_selection_get_selected_child",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_selection_get_selected_child, fixture_teardown);
  g_test_add ("/selection/atk_test_selection_select_child",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_selection_select_child, fixture_teardown);
  g_test_add ("/selection/atk_test_selection_deselect_selected",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_selection_deselect_selected, fixture_teardown);
  g_test_add ("/selection/atk_test_selection_deselect_child",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_selection_deselect_child, fixture_teardown);
  g_test_add ("/selection/atk_test_selection_is_child_selected",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_selection_is_child_selected, fixture_teardown);
  g_test_add ("/selection/atk_test_selection_select_all",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_selection_select_all, fixture_teardown);
  g_test_add ("/selection/atk_test_selection_clear_selection",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_selection_clear_selection, fixture_teardown);
}
