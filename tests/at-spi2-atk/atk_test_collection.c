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

#define DATA_FILE TESTS_DATA_DIR"/test-collection.xml"

static void
teardown_collection_test (gpointer fixture, gconstpointer user_data)
{
  terminate_app ();
}

static void
atk_test_collection_get_collection_iface (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiCollection *iface = atspi_accessible_get_collection_iface (obj);
  g_assert (iface);
}

static void
atk_test_collection_get_matches (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiCollection *iface = atspi_accessible_get_collection_iface (obj);
  g_assert (iface);

  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);

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
  GArray *ret = atspi_collection_get_matches (iface,
                rule,
                ATSPI_Collection_SORT_ORDER_CANONICAL,
                0,
                FALSE,
                NULL);
  g_assert_cmpint (2, ==, ret->len);

  AtspiAccessible *get = NULL;
  get = g_array_index (ret, AtspiAccessible *, 0);
  g_assert_cmpstr("obj1", ==, atspi_accessible_get_name (get, NULL));
  get = g_array_index (ret, AtspiAccessible *, 1);
  g_assert_cmpstr("obj3", ==, atspi_accessible_get_name (get, NULL));
}

static void
atk_test_collection_get_matches_to (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiCollection *iface = atspi_accessible_get_collection_iface (obj);
  g_assert (iface);

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
  AtspiAccessible *get = NULL;
  get = g_array_index (ret, AtspiAccessible *, 0);
  g_assert_cmpstr("obj1", ==, atspi_accessible_get_name (get, NULL));
}

static void
atk_test_collection_get_matches_from (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiCollection *iface = atspi_accessible_get_collection_iface (obj);
  g_assert (iface);

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
  GArray *ret = atspi_collection_get_matches_from (iface,
                child1,
                rule,
                ATSPI_Collection_SORT_ORDER_CANONICAL,
                ATSPI_Collection_TREE_INORDER,
                0,
                FALSE,
                NULL);
  g_assert_cmpint (3, ==, ret->len);
  AtspiAccessible *get = NULL;
  get = g_array_index (ret, AtspiAccessible *, 0);
  g_assert_cmpstr("obj2/1", ==, atspi_accessible_get_name (get, NULL));
  get = g_array_index (ret, AtspiAccessible *, 1);
  g_assert_cmpstr("obj3", ==, atspi_accessible_get_name (get, NULL));
  get = g_array_index (ret, AtspiAccessible *, 2);
  g_assert_cmpstr("obj3", ==, atspi_accessible_get_name (get, NULL));
}


void
atk_test_collection (void )
{
  g_test_add_vtable (ATK_TEST_PATH_COLLECTION "/atk_test_collection_get_collection_iface",
                     0, NULL, NULL, atk_test_collection_get_collection_iface, teardown_collection_test );
  g_test_add_vtable (ATK_TEST_PATH_COLLECTION "/atk_test_collection_get_matches",
                     0, NULL, NULL, atk_test_collection_get_matches, teardown_collection_test );
  g_test_add_vtable (ATK_TEST_PATH_COLLECTION "/atk_test_collection_get_matches_to",
                     0, NULL, NULL, atk_test_collection_get_matches_to, teardown_collection_test );
  g_test_add_vtable (ATK_TEST_PATH_COLLECTION "/atk_test_collection_get_matches_from",
                     0, NULL, NULL, atk_test_collection_get_matches_from, teardown_collection_test );
}

