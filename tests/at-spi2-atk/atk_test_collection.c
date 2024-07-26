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

#define DATA_FILE TESTS_DATA_DIR "/test-collection.xml"

static void
atk_test_collection_get_collection_iface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiCollection *iface = atspi_accessible_get_collection_iface (obj);
  g_assert_nonnull (iface);
  g_object_unref (iface);
}

static void
check_and_unref (GArray *array, gint index, const char *expected_name)
{
  AtspiAccessible *accessible = g_array_index (array, AtspiAccessible *, index);
  check_name (accessible, expected_name);
  g_object_unref (accessible);
}

static void
atk_test_collection_get_matches (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiCollection *iface = atspi_accessible_get_collection_iface (obj);
  g_assert_nonnull (iface);

  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);

  AtspiMatchRule *rule = NULL;
  AtspiStateSet *ss = atspi_accessible_get_state_set (child);

  rule = atspi_match_rule_new (ss,
                               ATSPI_Collection_MATCH_ALL,
                               NULL,
                               ATSPI_Collection_MATCH_ALL,
                               NULL,
                               ATSPI_Collection_MATCH_ALL,
                               NULL,
                               ATSPI_Collection_MATCH_ALL,
                               FALSE);
  g_object_unref (ss);
  GArray *ret = atspi_collection_get_matches (iface,
                                              rule,
                                              ATSPI_Collection_SORT_ORDER_CANONICAL,
                                              0,
                                              FALSE,
                                              NULL);
  g_assert_cmpint (3, ==, ret->len);

  check_and_unref (ret, 0, "obj1");
  check_and_unref (ret, 1, "obj2");
  check_and_unref (ret, 2, "obj3");
  g_array_free (ret, TRUE);
  g_object_unref (rule);
  g_object_unref (child);
  g_object_unref (iface);
}

static void
atk_test_collection_get_matches_to (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiCollection *iface = atspi_accessible_get_collection_iface (obj);
  g_assert_nonnull (iface);

  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiAccessible *child1 = atspi_accessible_get_child_at_index (obj, 1, NULL);

  AtspiMatchRule *rule = NULL;
  AtspiStateSet *ss = atspi_accessible_get_state_set (child);

  rule = atspi_match_rule_new (ss,
                               ATSPI_Collection_MATCH_ALL,
                               NULL,
                               ATSPI_Collection_MATCH_ALL,
                               NULL,
                               ATSPI_Collection_MATCH_ALL,
                               NULL,
                               ATSPI_Collection_MATCH_ALL,
                               FALSE);
  g_object_unref (ss);
  GArray *ret = atspi_collection_get_matches_to (iface,
                                                 child1,
                                                 rule,
                                                 ATSPI_Collection_SORT_ORDER_CANONICAL,
                                                 ATSPI_Collection_TREE_INORDER,
                                                 TRUE,
                                                 0,
                                                 FALSE,
                                                 NULL);
  g_assert_cmpint (1, ==, ret->len);
  check_and_unref (ret, 0, "obj1");
  g_array_free (ret, TRUE);
  g_object_unref (rule);
  g_object_unref (child1);
  g_object_unref (child);
  g_object_unref (iface);
}

static void
do_interface_test (AtspiCollection *iface, AtspiAccessible *start, const char *str, gint expected)
{
  AtspiMatchRule *rule = NULL;
  GArray *array;
  GArray *ret;

  array = g_array_new (FALSE, FALSE, sizeof (gchar *));
  g_array_insert_val (array, 0, str);
  rule = atspi_match_rule_new (NULL, ATSPI_Collection_MATCH_ALL,
                               NULL, ATSPI_Collection_MATCH_ALL,
                               NULL, ATSPI_Collection_MATCH_ALL,
                               array, ATSPI_Collection_MATCH_ALL,
                               FALSE);
  ret = atspi_collection_get_matches_from (iface, start, rule,
                                           ATSPI_Collection_SORT_ORDER_CANONICAL,
                                           ATSPI_Collection_TREE_INORDER,
                                           0, FALSE, NULL);
  g_assert_cmpint (expected, ==, ret->len);
  g_array_free (array, TRUE);
  g_array_free (ret, TRUE);
  g_object_unref (rule);
}

static void
atk_test_collection_get_matches_from (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiCollection *iface = atspi_accessible_get_collection_iface (obj);
  GHashTable *attributes;
  GArray *array;
  AtspiRole role;

  g_assert_nonnull (iface);

  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiAccessible *child1 = atspi_accessible_get_child_at_index (obj, 1, NULL);

  AtspiMatchRule *rule = NULL;
  AtspiStateSet *ss = atspi_accessible_get_state_set (child1);

  rule = atspi_match_rule_new (ss,
                               ATSPI_Collection_MATCH_ALL,
                               NULL,
                               ATSPI_Collection_MATCH_ALL,
                               NULL,
                               ATSPI_Collection_MATCH_ALL,
                               NULL,
                               ATSPI_Collection_MATCH_ALL,
                               FALSE);
  GArray *ret = atspi_collection_get_matches_from (iface,
                                                   child1,
                                                   rule,
                                                   ATSPI_Collection_SORT_ORDER_CANONICAL,
                                                   ATSPI_Collection_TREE_INORDER,
                                                   0,
                                                   FALSE,
                                                   NULL);
  g_object_unref (ss);
  g_assert_cmpint (2, ==, ret->len);
  check_and_unref (ret, 0, "obj2/1");
  check_and_unref (ret, 1, "obj3");
  g_array_free (ret, TRUE);
  g_object_unref (rule);

  attributes = g_hash_table_new (g_str_hash, g_str_equal);
  g_hash_table_insert (attributes, "layout-guess", "true");
  rule = atspi_match_rule_new (NULL,
                               ATSPI_Collection_MATCH_ALL,
                               attributes,
                               ATSPI_Collection_MATCH_NONE,
                               NULL,
                               ATSPI_Collection_MATCH_ALL,
                               NULL,
                               ATSPI_Collection_MATCH_ALL,
                               FALSE);
  ret = atspi_collection_get_matches_from (iface,
                                           child1,
                                           rule,
                                           ATSPI_Collection_SORT_ORDER_CANONICAL,
                                           ATSPI_Collection_TREE_INORDER,
                                           0,
                                           FALSE,
                                           NULL);
  g_hash_table_unref (attributes);
  g_assert_cmpint (5, ==, ret->len);
  g_array_free (ret, TRUE);
  g_object_unref (rule);

  array = g_array_new (FALSE, FALSE, sizeof (AtspiRole));
  role = ATSPI_ROLE_LINK;
  g_array_insert_val (array, 0, role);
  rule = atspi_match_rule_new (NULL,
                               ATSPI_Collection_MATCH_ALL,
                               NULL,
                               ATSPI_Collection_MATCH_ALL,
                               array,
                               ATSPI_Collection_MATCH_NONE,
                               NULL,
                               ATSPI_Collection_MATCH_ALL,
                               FALSE);
  ret = atspi_collection_get_matches_from (iface,
                                           child1,
                                           rule,
                                           ATSPI_Collection_SORT_ORDER_CANONICAL,
                                           ATSPI_Collection_TREE_INORDER,
                                           0,
                                           FALSE,
                                           NULL);
  g_assert_cmpint (4, ==, ret->len);
  g_array_free (array, TRUE);
  g_array_free (ret, TRUE);
  g_object_unref (rule);

  do_interface_test (iface, child1, "Action", 1);
  do_interface_test (iface, child1, "Action(click)", 1);
  do_interface_test (iface, child1, "Action(toggle)", 0);

  g_object_unref (child1);
  g_object_unref (child);
  g_object_unref (iface);
}

void
atk_test_collection (void)
{
  g_test_add ("/collection/atk_test_collection_get_collection_iface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_collection_get_collection_iface, fixture_teardown);
  g_test_add ("/collection/atk_test_collection_get_matches",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_collection_get_matches, fixture_teardown);
  g_test_add ("/collection/atk_test_collection_get_matches_to",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_collection_get_matches_to, fixture_teardown);
  g_test_add ("/collection/atk_test_collection_get_matches_from",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_collection_get_matches_from, fixture_teardown);
}
