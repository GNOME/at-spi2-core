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

#define DATA_FILE TESTS_DATA_DIR "/test-action.xml"

static void
atk_test_action_sample_get_interface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  check_name (obj, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiAction *iface = atspi_accessible_get_action_iface (child);
  g_assert_nonnull (iface);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_action_get_action_description (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiAction *action = atspi_accessible_get_action_iface (child);
  gchar *str = atspi_action_get_action_description (action, 0, NULL);
  g_assert_cmpstr (str, ==, "action1 description");
  g_free (str);
  g_object_unref (action);
  g_object_unref (child);
}

static void
atk_test_action_get_action_name (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiAction *action = atspi_accessible_get_action_iface (child);
  gchar *str = atspi_action_get_action_name (action, 0, NULL);
  g_assert_cmpstr (str, ==, "action1");
  g_free (str);
  g_object_unref (action);
  g_object_unref (child);
}

static void
atk_test_action_get_n_actions (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiAction *action = atspi_accessible_get_action_iface (child);
  g_assert_cmpint (atspi_action_get_n_actions (action, NULL), ==, 2);
  g_object_unref (action);
  g_object_unref (child);
}

static void
atk_test_action_get_key_binding (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiAction *action = atspi_accessible_get_action_iface (child);
  gchar *str = atspi_action_get_key_binding (action, 0, NULL);
  g_assert_cmpstr (str, ==, "action1 key binding");
  g_free (str);
  g_object_unref (action);
  g_object_unref (child);
}

static void
atk_test_action_get_localized_name (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiAction *action = atspi_accessible_get_action_iface (child);
  gchar *str = atspi_action_get_localized_name (action, 0, NULL);
  g_assert_cmpstr (str, ==, "action1");
  g_free (str);
  g_object_unref (action);
  g_object_unref (child);
}

static void
atk_test_action_do_action (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiAction *action = atspi_accessible_get_action_iface (child);
  g_assert_nonnull (action);
  atspi_action_do_action (action, 0, NULL);
  atspi_accessible_clear_cache (obj);
  AtspiStateSet *s = atspi_accessible_get_state_set (child);
  GArray *array = atspi_state_set_get_states (s);
  g_assert_cmpint (array->len, ==, 1);
  g_array_free (array, TRUE);
  g_object_unref (s);
  g_object_unref (action);
  g_object_unref (child);
}

void
atk_test_action (void)
{
  g_test_add ("/action/atk_test_action_sample_get_interface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_action_sample_get_interface, fixture_teardown);
  g_test_add ("/action/atk_test_action_get_action_description",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_action_get_action_description, fixture_teardown);
  g_test_add ("/action/atk_test_action_get_action_name",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_action_get_action_name, fixture_teardown);
  g_test_add ("/action/atk_test_action_get_n_actions",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_action_get_n_actions, fixture_teardown);
  g_test_add ("/action/atk_test_action_get_key_binding",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_action_get_key_binding, fixture_teardown);
  g_test_add ("/action/atk_test_action_get_localized_name",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_action_get_localized_name, fixture_teardown);
  g_test_add ("/action/atk_test_action_do_action",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_action_do_action, fixture_teardown);
}
