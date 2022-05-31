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

#include "atk_test_util.h"
#include "atk_suite.h"

#define DATA_FILE TESTS_DATA_DIR"/test-hypertext.xml"

static void
atk_test_hyperlink_get_n_anchors (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiHypertext *obj1 = atspi_accessible_get_hypertext_iface (child);
  g_assert (obj1);
  AtspiHyperlink *obj = atspi_hypertext_get_link (obj1, 1, NULL);
  g_assert (obj);
  gint n = atspi_hyperlink_get_n_anchors (obj, NULL);
  g_assert_cmpint (n, ==, 1);
}

static void
atk_test_hyperlink_get_uri (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiHypertext *obj1 = atspi_accessible_get_hypertext_iface (child);
  g_assert (obj1);
  AtspiHyperlink *obj = atspi_hypertext_get_link (obj1, 1, NULL);
  g_assert (obj);
  gchar *str = atspi_hyperlink_get_uri (obj, 0, NULL);
  g_assert (str);
  g_assert_cmpstr (str, ==, "pinkbike.com");
}

static void
atk_test_hyperlink_get_object (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiHypertext *obj1 = atspi_accessible_get_hypertext_iface (child);
  g_assert (obj1);
  AtspiHyperlink *obj = atspi_hypertext_get_link (obj1, 1, NULL);
  g_assert (obj);
  AtspiAccessible *acc = atspi_hyperlink_get_object (obj, 0, NULL);
  g_assert (acc);
  AtspiText *at = atspi_accessible_get_text_iface (acc);
  g_assert (at);
  gchar *text = atspi_text_get_text (at, 0, 12, NULL);
  g_assert_cmpstr (text,==,"pinkbike.com");
  g_free (text);
}

static void
atk_test_hyperlink_get_index_range (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiHypertext *obj1 = atspi_accessible_get_hypertext_iface (child);
  g_assert (obj1);
  AtspiHyperlink *obj = atspi_hypertext_get_link (obj1, 1, NULL);
  g_assert (obj);
  AtspiRange *range = atspi_hyperlink_get_index_range (obj, NULL);
  g_assert (range);
  /*
    ATK do not define this function
  */
  g_assert_cmpint (range->start_offset, ==, -1);
  g_assert_cmpint (range->end_offset, ==, -1);
  g_free (range);
}

static void
atk_test_hyperlink_get_start_index (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiHypertext *obj1 = atspi_accessible_get_hypertext_iface (child);
  g_assert (obj1);
  AtspiHyperlink *obj = atspi_hypertext_get_link (obj1, 1, NULL);
  g_assert  (obj);
  gint n = atspi_hyperlink_get_start_index (obj, NULL);
  g_assert_cmpint (n, ==, 69);
}

static void
atk_test_hyperlink_get_end_index (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiHypertext *obj1 = atspi_accessible_get_hypertext_iface (child);
  g_assert (obj1);
  AtspiHyperlink *obj = atspi_hypertext_get_link (obj1, 1, NULL);
  g_assert (obj);
  gint n = atspi_hyperlink_get_end_index (obj, NULL);
  g_assert_cmpint (n, ==, 81);
}

static void
atk_test_hyperlink_is_valid (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiHypertext *obj1 = atspi_accessible_get_hypertext_iface (child);
  g_assert (obj1);
  AtspiHyperlink *obj = atspi_hypertext_get_link (obj1, 1, NULL);
  g_assert (obj);
  g_assert (atspi_hyperlink_is_valid (obj, NULL));
}

static void
teardown_hyperlink_test (gpointer fixture, gconstpointer user_data)
{
  terminate_app ();
}

void
atk_test_hyperlink(void)
{
  g_test_add_vtable(ATK_TEST_PATH_HYPERTEXT "/atk_test_hyperlink_get_n_anchors",
                    0, NULL, NULL, atk_test_hyperlink_get_n_anchors, teardown_hyperlink_test );
  g_test_add_vtable(ATK_TEST_PATH_HYPERTEXT "/atk_test_hyperlink_get_uri",
                    0, NULL, NULL, atk_test_hyperlink_get_uri, teardown_hyperlink_test);
  g_test_add_vtable(ATK_TEST_PATH_HYPERTEXT "/atk_test_hyperlink_get_object",
                    0, NULL, NULL, atk_test_hyperlink_get_object, teardown_hyperlink_test);
  g_test_add_vtable(ATK_TEST_PATH_HYPERTEXT "/atk_test_hyperlink_get_index_range",
                    0, NULL, NULL, atk_test_hyperlink_get_index_range, teardown_hyperlink_test);
  g_test_add_vtable(ATK_TEST_PATH_HYPERTEXT "/atk_test_hyperlink_get_start_index",
                    0, NULL, NULL, atk_test_hyperlink_get_start_index, teardown_hyperlink_test);
  g_test_add_vtable(ATK_TEST_PATH_HYPERTEXT "/atk_test_hyperlink_get_end_index",
                    0, NULL, NULL, atk_test_hyperlink_get_end_index, teardown_hyperlink_test);
  g_test_add_vtable(ATK_TEST_PATH_HYPERTEXT "/atk_test_hyperlink_is_valid",
                    0, NULL, NULL, atk_test_hyperlink_is_valid, teardown_hyperlink_test);
}
