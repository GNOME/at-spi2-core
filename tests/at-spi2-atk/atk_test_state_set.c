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

#define DATA_FILE TESTS_DATA_DIR"/test-accessible.xml"

static void
teardown_state_set_test (gpointer fixture, gconstpointer user_data)
{
  terminate_app ();
}

static void
atk_test_accessible_get_state_set (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiStateSet *states = atspi_accessible_get_state_set (child);
  GArray *states_arr = atspi_state_set_get_states (states);

  AtspiStateType valid_states[] = {
    ATSPI_STATE_MODAL,
    ATSPI_STATE_MULTI_LINE,
  };
  g_assert_cmpint (states_arr->len, ==, 2);
  int i = 0;
  for (i = 0; i < states_arr->len; ++i) {
    g_assert_cmpint (valid_states[i], ==, g_array_index (states_arr, AtspiStateType, i));
    g_assert (atspi_state_set_contains (states, ATSPI_STATE_MODAL));
    g_assert (atspi_state_set_contains (states, ATSPI_STATE_MULTI_LINE));
  }
}

static void
atk_test_state_set_new (gpointer fixture, gconstpointer user_data)
{
  GArray *states_arr = g_array_new (FALSE, FALSE, sizeof (AtspiStateType));

  gint state = 0;
  state = 11; // ATSPI_STATE_FOCUSABLE
  g_array_append_val (states_arr, state);
  state = 12; // ATSPI_STATE_FOCUSED
  g_array_append_val (states_arr, state);

  g_assert_cmpint (states_arr->len, ==, 2);

  AtspiStateSet *ss = atspi_state_set_new (states_arr);

  AtspiStateType valid_states[] = {
    ATSPI_STATE_FOCUSABLE,
    ATSPI_STATE_FOCUSED,
  };

  g_assert (atspi_state_set_contains (ss, valid_states[0]));
  g_assert (atspi_state_set_contains (ss, valid_states[1]));
  int i = 0;
  for (i = 0; i < states_arr->len; ++i) {
    g_assert_cmpint (valid_states[i], ==, g_array_index (states_arr, AtspiStateType, i));
  }
}

static void
atk_test_state_set_set_by_name (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiStateSet *states = atspi_accessible_get_state_set (child);
  GArray *states_arr = atspi_state_set_get_states (states);

  atspi_state_set_set_by_name (states, "modal", FALSE);

  states_arr = atspi_state_set_get_states (states);

  g_assert_cmpint (states_arr->len, ==, 1);
  g_assert (!atspi_state_set_contains (states, ATSPI_STATE_MODAL));
  g_assert (atspi_state_set_contains (states, ATSPI_STATE_MULTI_LINE));

  atspi_state_set_set_by_name (states, "modal", TRUE);
  g_assert (atspi_state_set_contains (states, ATSPI_STATE_MODAL));
}

static void
atk_test_state_set_add (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiStateSet *states = atspi_accessible_get_state_set (child);

  g_assert (!atspi_state_set_contains (states, ATSPI_STATE_FOCUSABLE));

  atspi_state_set_add (states, ATSPI_STATE_FOCUSABLE);

  g_assert (atspi_state_set_contains (states, ATSPI_STATE_FOCUSABLE));

}

static void
atk_test_state_set_compare (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiStateSet *states = atspi_accessible_get_state_set (child);
  GArray *states_arr = g_array_new (FALSE, FALSE, sizeof (AtspiStateType));

  gint state = 0;
  state = 11; // ATSPI_STATE_FOCUSABLE
  g_array_append_val (states_arr, state);
  state = 12; // ATSPI_STATE_FOCUSED
  g_array_append_val (states_arr, state);

  g_assert_cmpint (states_arr->len, ==, 2);

  AtspiStateSet *ss = atspi_state_set_new (states_arr);

  AtspiStateSet *ret = atspi_state_set_compare (states, ss);

  g_assert (atspi_state_set_contains (ret, ATSPI_STATE_MODAL));
  g_assert (atspi_state_set_contains (ret, ATSPI_STATE_MULTI_LINE));
  g_assert (atspi_state_set_contains (ret, ATSPI_STATE_FOCUSED));
  g_assert (atspi_state_set_contains (ret, ATSPI_STATE_FOCUSABLE));
}

static void
atk_test_state_set_contains (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiStateSet *states = atspi_accessible_get_state_set (child);

  g_assert (!atspi_state_set_contains (states, ATSPI_STATE_FOCUSABLE));
  g_assert (atspi_state_set_contains (states, ATSPI_STATE_MODAL));
}

static void
atk_test_state_set_equals (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiStateSet *states = atspi_accessible_get_state_set (child);
  GArray *states_arr = g_array_new (FALSE, FALSE, sizeof (AtspiStateType));

  gint state = 0;
  state = 16; // ATSPI_STATE_MODAL
  g_array_append_val (states_arr, state);
  state = 17; // ATSPI_STATE_MULTI_LINE
  g_array_append_val (states_arr, state);

  g_assert_cmpint (states_arr->len, ==, 2);

  AtspiStateSet *ss = atspi_state_set_new (states_arr);

  g_assert (atspi_state_set_equals (states, ss));
}

static void
atk_test_state_set_get_states (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiStateSet *states = atspi_accessible_get_state_set (child);
  GArray *states_arr = atspi_state_set_get_states (states);

  AtspiStateType valid_states[] = {
    ATSPI_STATE_MODAL,
    ATSPI_STATE_MULTI_LINE,
  };
  g_assert_cmpint (states_arr->len, ==, 2);
  int i = 0;
  for (i = 0; i < states_arr->len; ++i)
    g_assert_cmpint (valid_states[i], ==, g_array_index (states_arr, AtspiStateType, i));
  g_assert (atspi_state_set_contains (states, ATSPI_STATE_MODAL));
  g_assert (atspi_state_set_contains (states, ATSPI_STATE_MULTI_LINE));
}

static void
atk_test_state_set_is_empty (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiStateSet *states = atspi_accessible_get_state_set (child);
  AtspiStateSet *root_states = atspi_accessible_get_state_set (obj);

  g_assert (!atspi_state_set_is_empty (states));
  g_assert (atspi_state_set_is_empty (root_states));
}

static void
atk_test_state_set_remove (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiStateSet *states = atspi_accessible_get_state_set (child);
  GArray *states_arr = atspi_state_set_get_states (states);

  g_assert_cmpint (states_arr->len, ==, 2);
  atspi_state_set_remove (states, ATSPI_STATE_MODAL);

  states_arr = atspi_state_set_get_states (states);

  g_assert_cmpint (states_arr->len, ==, 1);
  g_assert (!atspi_state_set_contains (states, ATSPI_STATE_MODAL));
  g_assert (atspi_state_set_contains (states, ATSPI_STATE_MULTI_LINE));
}

void
atk_test_state_set (void)
{
  g_test_add_vtable (ATK_TEST_PATH_STATE_SET "/atk_test_accessible_get_state_set",
                     0, NULL, NULL, atk_test_accessible_get_state_set, teardown_state_set_test);
  g_test_add_vtable (ATK_TEST_PATH_STATE_SET "/atk_test_state_set_new",
                     0, NULL, NULL, atk_test_state_set_new, teardown_state_set_test);
  g_test_add_vtable (ATK_TEST_PATH_STATE_SET "/atk_test_state_set_set_by_name",
                     0, NULL, NULL, atk_test_state_set_set_by_name, teardown_state_set_test);
  g_test_add_vtable (ATK_TEST_PATH_STATE_SET "/atk_test_state_set_add",
                     0, NULL, NULL, atk_test_state_set_add, teardown_state_set_test);
  g_test_add_vtable (ATK_TEST_PATH_STATE_SET "/atk_test_state_set_compare",
                     0, NULL, NULL, atk_test_state_set_compare, teardown_state_set_test);
  g_test_add_vtable (ATK_TEST_PATH_STATE_SET "/atk_test_state_set_contains",
                     0, NULL, NULL, atk_test_state_set_contains, teardown_state_set_test);
  g_test_add_vtable (ATK_TEST_PATH_STATE_SET "/atk_test_state_set_equals",
                     0, NULL, NULL, atk_test_state_set_equals, teardown_state_set_test);
  g_test_add_vtable (ATK_TEST_PATH_STATE_SET "/atk_test_state_set_get_states",
                     0, NULL, NULL, atk_test_state_set_get_states, teardown_state_set_test);
  g_test_add_vtable (ATK_TEST_PATH_STATE_SET "/atk_test_state_set_is_empty",
                     0, NULL, NULL, atk_test_state_set_is_empty, teardown_state_set_test);
  g_test_add_vtable (ATK_TEST_PATH_STATE_SET "/atk_test_state_set_remove",
                     0, NULL, NULL, atk_test_state_set_remove, teardown_state_set_test);
}

