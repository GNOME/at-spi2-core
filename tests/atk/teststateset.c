/* ATK -  Accessibility Toolkit
 * Copyright 2001 Sun Microsystems Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <atk/atk.h>

#include <string.h>

static void test_state_set (void);
static void test_state (void);

static void
test_state_set (void)
{
  AtkStateSet *state_set1, *state_set2, *state_set3;
  AtkStateType state_array[3];

  state_set1 = atk_state_set_new ();

  g_assert_true (atk_state_set_is_empty (state_set1));

  g_assert_true (atk_state_set_add_state (state_set1, ATK_STATE_ACTIVE));

  g_assert_false (atk_state_set_is_empty (state_set1));

  g_assert_false (atk_state_set_add_state (state_set1, ATK_STATE_ACTIVE));

  state_array[0] = ATK_STATE_ACTIVE;
  state_array[1] = ATK_STATE_VISIBLE;
  state_array[2] = ATK_STATE_BUSY;
  atk_state_set_add_states (state_set1, state_array, 3);

  g_assert_true (atk_state_set_contains_state (state_set1, ATK_STATE_ACTIVE));
  g_assert_true (atk_state_set_contains_state (state_set1, ATK_STATE_VISIBLE));
  g_assert_true (atk_state_set_contains_state (state_set1, ATK_STATE_BUSY));
  g_assert_false (atk_state_set_contains_state (state_set1, ATK_STATE_VERTICAL));

  atk_state_set_remove_state (state_set1, ATK_STATE_BUSY);
  g_assert_false (atk_state_set_contains_state (state_set1, ATK_STATE_BUSY));
  g_assert_true (atk_state_set_contains_state (state_set1, ATK_STATE_VISIBLE));
  g_assert_false (atk_state_set_contains_states (state_set1, state_array, 3));
  g_assert_true (atk_state_set_contains_states (state_set1, state_array, 2));

  state_array[0] = ATK_STATE_SINGLE_LINE;
  state_array[1] = ATK_STATE_VISIBLE;
  state_array[2] = ATK_STATE_VERTICAL;

  state_set2 = atk_state_set_new ();
  atk_state_set_add_states (state_set2, state_array, 3);

  state_set3 = atk_state_set_and_sets (state_set1, state_set2);
  g_assert_true (atk_state_set_contains_state (state_set3, ATK_STATE_VISIBLE));
  g_assert_false (atk_state_set_contains_state (state_set3, ATK_STATE_BUSY));
  g_object_unref (state_set3);

  atk_state_set_remove_state (state_set1, ATK_STATE_VISIBLE);
  state_set3 = atk_state_set_and_sets (state_set1, state_set2);
  g_assert_null (state_set3);

  state_set3 = atk_state_set_or_sets (state_set1, state_set2);
  g_assert_true (atk_state_set_contains_state (state_set3, ATK_STATE_VISIBLE));
  g_assert_false (atk_state_set_contains_state (state_set3, ATK_STATE_INVALID));
  g_object_unref (state_set3);

  g_assert_true (atk_state_set_add_state (state_set1, ATK_STATE_VISIBLE));
  state_set3 = atk_state_set_xor_sets (state_set1, state_set2);
  g_assert_false (atk_state_set_contains_state (state_set3, ATK_STATE_VISIBLE));
  g_assert_true (atk_state_set_contains_state (state_set3, ATK_STATE_ACTIVE));

  atk_state_set_clear_states (state_set1);
  g_assert_false (atk_state_set_contains_state (state_set1, ATK_STATE_ACTIVE));

  g_object_unref (state_set1);
  g_object_unref (state_set2);
  g_object_unref (state_set3);
}

static void
test_state (void)
{
  AtkStateType type1, type2;
  const gchar *name;

  name = atk_state_type_get_name (ATK_STATE_VISIBLE);
  g_assert_cmpstr (name, ==, "visible");

  name = atk_state_type_get_name (ATK_STATE_MODAL);
  g_assert_cmpstr (name, ==, "modal");

  g_assert_cmpint (atk_state_type_for_name ("focused"), ==, ATK_STATE_FOCUSED);

  type1 = atk_state_type_register ("test-state");
  name = atk_state_type_get_name (type1);
  g_assert_cmpstr (name, ==, "test-state");
  type2 = atk_state_type_for_name ("test-state");
  g_assert_cmpint (type1, ==, type2);
  g_assert_cmpint (atk_state_type_for_name ("TEST_STATE"), ==, 0);

  /*
   * Check that a non-existent type returns NULL
   */
  g_assert_null (atk_state_type_get_name (ATK_STATE_LAST_DEFINED + 2));
}

int
main (gint argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);
  g_test_add_func ("/atk/state/state_set", test_state_set);
  g_test_add_func ("/atk/state/state", test_state);

  return g_test_run ();
}
