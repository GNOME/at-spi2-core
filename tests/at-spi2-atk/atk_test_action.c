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

#define DATA_FILE TESTS_DATA_DIR"/test-action.xml"

static void
teardown_action_test (gpointer fixture, gconstpointer user_data)
{
  terminate_app ();
}

static void
atk_test_action_sample_get_interface (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  g_assert_cmpstr (atspi_accessible_get_name (obj, NULL), ==, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj,1, NULL);
  AtspiAction *iface = atspi_accessible_get_action_iface (child);
  g_assert (iface != NULL);
}

static void
atk_test_action_get_action_description (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj,1, NULL);
  AtspiAction *action = atspi_accessible_get_action_iface (child);
  g_assert_cmpstr (atspi_action_get_action_description (action, 0, NULL), == ,"action1 description");
}

static void
atk_test_action_get_action_name (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj,1, NULL);
  AtspiAction *action = atspi_accessible_get_action_iface (child);
  g_assert_cmpstr (atspi_action_get_action_name (action, 0, NULL), == ,"action1");
}

static void
atk_test_action_get_n_actions (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj,1, NULL);
  AtspiAction *action = atspi_accessible_get_action_iface (child);
  g_assert_cmpint (atspi_action_get_n_actions (action, NULL), == , 2);
}

static void
atk_test_action_get_key_binding (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj,1, NULL);
  AtspiAction *action = atspi_accessible_get_action_iface (child);
  g_assert_cmpstr (atspi_action_get_key_binding (action, 0, NULL), == ,"action1 key binding");
}

static void
atk_test_action_get_localized_name (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj,1, NULL);
  AtspiAction *action = atspi_accessible_get_action_iface (child);
  g_assert_cmpstr (atspi_action_get_localized_name (action, 0, NULL), == ,"action1");
}

static void
atk_test_action_do_action (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj,1, NULL);
  AtspiAction *action = atspi_accessible_get_action_iface (child);
  g_assert (action != NULL);
  atspi_action_do_action (action, 0, NULL);
  atspi_accessible_clear_cache (obj);
  AtspiStateSet *s = atspi_accessible_get_state_set (child);
  GArray *array = atspi_state_set_get_states (s);
  g_assert_cmpint (array->len, ==, 1);
}

void
atk_test_action (void)
{
  g_test_add_vtable (ATK_TEST_PATH_ACTION "/atk_test_action_sample_get_interface",
                     0, NULL, NULL, atk_test_action_sample_get_interface, teardown_action_test);
  g_test_add_vtable (ATK_TEST_PATH_ACTION "/atk_test_action_get_action_description",
                     0, NULL, NULL, atk_test_action_get_action_description, teardown_action_test);
  g_test_add_vtable (ATK_TEST_PATH_ACTION "/atk_test_action_get_action_name",
                     0, NULL, NULL, atk_test_action_get_action_name, teardown_action_test);
  g_test_add_vtable (ATK_TEST_PATH_ACTION "/atk_test_action_get_n_actions",
                     0, NULL, NULL, atk_test_action_get_n_actions, teardown_action_test);
  g_test_add_vtable (ATK_TEST_PATH_ACTION "/atk_test_action_get_key_binding",
                     0, NULL, NULL, atk_test_action_get_key_binding, teardown_action_test);
  g_test_add_vtable (ATK_TEST_PATH_ACTION "/atk_test_action_get_localized_name",
                     0, NULL, NULL, atk_test_action_get_localized_name, teardown_action_test);
  g_test_add_vtable (ATK_TEST_PATH_ACTION "/atk_test_action_do_action",
                     0, NULL, NULL, atk_test_action_do_action, teardown_action_test);
}
