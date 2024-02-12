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

#define DATA_FILE TESTS_DATA_DIR "/test-accessible.xml"

static void
atk_test_accessible_get_state_set (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
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
    {
      g_assert_cmpint (valid_states[i], ==, g_array_index (states_arr, AtspiStateType, i));
      g_assert_true (atspi_state_set_contains (states, ATSPI_STATE_MODAL));
      g_assert_true (atspi_state_set_contains (states, ATSPI_STATE_MULTI_LINE));
    }
  g_array_free (states_arr, TRUE);
  g_object_unref (states);
  g_object_unref (child);
}

static void
atk_test_state_set_new (TestAppFixture *fixture, gconstpointer user_data)
{
  GArray *states_arr = g_array_new (FALSE, FALSE, sizeof (AtspiStateType));

  AtspiStateType state = ATSPI_STATE_FOCUSABLE;
  g_array_append_val (states_arr, state);
  state = ATSPI_STATE_FOCUSED;
  g_array_append_val (states_arr, state);

  g_assert_cmpint (states_arr->len, ==, 2);

  AtspiStateSet *ss = atspi_state_set_new (states_arr);

  g_assert_true (atspi_state_set_contains (ss, ATSPI_STATE_FOCUSABLE));
  g_assert_true (atspi_state_set_contains (ss, ATSPI_STATE_FOCUSED));
  g_object_unref (ss);
  g_array_free (states_arr, TRUE);
}

static void
atk_test_state_set_set_by_name (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiStateSet *states = atspi_accessible_get_state_set (child);
  GArray *states_arr = atspi_state_set_get_states (states);

  atspi_state_set_set_by_name (states, "modal", FALSE);
  g_array_free (states_arr, TRUE);

  states_arr = atspi_state_set_get_states (states);

  g_assert_cmpint (states_arr->len, ==, 1);
  g_assert_true (!atspi_state_set_contains (states, ATSPI_STATE_MODAL));
  g_assert_true (atspi_state_set_contains (states, ATSPI_STATE_MULTI_LINE));

  atspi_state_set_set_by_name (states, "modal", TRUE);
  g_assert_true (atspi_state_set_contains (states, ATSPI_STATE_MODAL));
  g_array_free (states_arr, TRUE);
  g_object_unref (states);
  g_object_unref (child);
}

static void
atk_test_state_set_add (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiStateSet *states = atspi_accessible_get_state_set (child);

  g_assert_true (!atspi_state_set_contains (states, ATSPI_STATE_FOCUSABLE));

  atspi_state_set_add (states, ATSPI_STATE_FOCUSABLE);

  g_assert_true (atspi_state_set_contains (states, ATSPI_STATE_FOCUSABLE));
  g_object_unref (states);
  g_object_unref (child);
}

static void
atk_test_state_set_compare (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
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

  g_assert_true (atspi_state_set_contains (ret, ATSPI_STATE_MODAL));
  g_assert_true (atspi_state_set_contains (ret, ATSPI_STATE_MULTI_LINE));
  g_assert_true (atspi_state_set_contains (ret, ATSPI_STATE_FOCUSED));
  g_assert_true (atspi_state_set_contains (ret, ATSPI_STATE_FOCUSABLE));
  g_object_unref (ret);
  g_object_unref (ss);
  g_array_free (states_arr, TRUE);
  g_object_unref (states);
  g_object_unref (child);
}

static void
atk_test_state_set_contains (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiStateSet *states = atspi_accessible_get_state_set (child);

  g_assert_true (!atspi_state_set_contains (states, ATSPI_STATE_FOCUSABLE));
  g_assert_true (atspi_state_set_contains (states, ATSPI_STATE_MODAL));
  g_object_unref (states);
  g_object_unref (child);
}

static void
atk_test_state_set_equals (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
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

  g_assert_true (atspi_state_set_equals (states, ss));
  g_object_unref (ss);
  g_array_free (states_arr, TRUE);
  g_object_unref (states);
  g_object_unref (child);
}

static void
atk_test_state_set_get_states (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
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
  g_assert_true (atspi_state_set_contains (states, ATSPI_STATE_MODAL));
  g_assert_true (atspi_state_set_contains (states, ATSPI_STATE_MULTI_LINE));
  g_array_free (states_arr, TRUE);
  g_object_unref (states);
  g_object_unref (child);
}

static void
atk_test_state_set_is_empty (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiStateSet *states = atspi_accessible_get_state_set (child);
  AtspiStateSet *root_states = atspi_accessible_get_state_set (obj);

  g_assert_true (!atspi_state_set_is_empty (states));
  g_assert_true (atspi_state_set_is_empty (root_states));
  g_object_unref (root_states);
  g_object_unref (states);
  g_object_unref (child);
}

static void
atk_test_state_set_remove (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiStateSet *states = atspi_accessible_get_state_set (child);
  GArray *states_arr = atspi_state_set_get_states (states);

  g_assert_cmpint (states_arr->len, ==, 2);
  atspi_state_set_remove (states, ATSPI_STATE_MODAL);
  g_array_free (states_arr, TRUE);

  states_arr = atspi_state_set_get_states (states);

  g_assert_cmpint (states_arr->len, ==, 1);
  g_assert_true (!atspi_state_set_contains (states, ATSPI_STATE_MODAL));
  g_assert_true (atspi_state_set_contains (states, ATSPI_STATE_MULTI_LINE));
  g_array_free (states_arr, TRUE);
  g_object_unref (states);
  g_object_unref (child);
}

void
atk_test_state_set (void)
{
  g_test_add ("/state_set/atk_test_accessible_get_state_set",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_state_set, fixture_teardown);
  g_test_add ("/state_set/atk_test_state_set_new",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_state_set_new, fixture_teardown);
  g_test_add ("/state_set/atk_test_state_set_set_by_name",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_state_set_set_by_name, fixture_teardown);
  g_test_add ("/state_set/atk_test_state_set_add",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_state_set_add, fixture_teardown);
  g_test_add ("/state_set/atk_test_state_set_compare",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_state_set_compare, fixture_teardown);
  g_test_add ("/state_set/atk_test_state_set_contains",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_state_set_contains, fixture_teardown);
  g_test_add ("/state_set/atk_test_state_set_equals",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_state_set_equals, fixture_teardown);
  g_test_add ("/state_set/atk_test_state_set_get_states",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_state_set_get_states, fixture_teardown);
  g_test_add ("/state_set/atk_test_state_set_is_empty",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_state_set_is_empty, fixture_teardown);
  g_test_add ("/state_set/atk_test_state_set_remove",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_state_set_remove, fixture_teardown);
}
