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

#include "atk_suite.h"
#include "atk_test_util.h"

#define DATA_FILE TESTS_DATA_DIR "/test-hypertext.xml"

static void
atk_test_hyperlink_get_n_anchors (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiHypertext *obj1 = atspi_accessible_get_hypertext_iface (child);
  g_assert_nonnull (obj1);
  AtspiHyperlink *obj = atspi_hypertext_get_link (obj1, 1, NULL);
  g_assert_nonnull (obj);
  gint n = atspi_hyperlink_get_n_anchors (obj, NULL);
  g_assert_cmpint (n, ==, 1);
  g_object_unref (obj);
  g_object_unref (obj1);
  g_object_unref (child);
}

static void
atk_test_hyperlink_get_uri (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiHypertext *obj1 = atspi_accessible_get_hypertext_iface (child);
  g_assert_nonnull (obj1);
  AtspiHyperlink *obj = atspi_hypertext_get_link (obj1, 1, NULL);
  g_assert_nonnull (obj);
  gchar *str = atspi_hyperlink_get_uri (obj, 0, NULL);
  g_assert_nonnull (str);
  g_assert_cmpstr (str, ==, "pinkbike.com");
  g_free (str);
  g_object_unref (obj);
  g_object_unref (obj1);
  g_object_unref (child);
}

static void
atk_test_hyperlink_get_object (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiHypertext *obj1 = atspi_accessible_get_hypertext_iface (child);
  g_assert_nonnull (obj1);
  AtspiHyperlink *obj = atspi_hypertext_get_link (obj1, 1, NULL);
  g_assert_nonnull (obj);
  AtspiAccessible *acc = atspi_hyperlink_get_object (obj, 0, NULL);
  g_assert_nonnull (acc);
  AtspiText *at = atspi_accessible_get_text_iface (acc);
  g_assert_nonnull (at);
  gchar *text = atspi_text_get_text (at, 0, 12, NULL);
  g_assert_cmpstr (text, ==, "pinkbike.com");
  g_free (text);
  g_object_unref (at);
  g_object_unref (acc);
  g_object_unref (obj);
  g_object_unref (obj1);
  g_object_unref (child);
}

static void
atk_test_hyperlink_get_index_range (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiHypertext *obj1 = atspi_accessible_get_hypertext_iface (child);
  g_assert_nonnull (obj1);
  AtspiHyperlink *obj = atspi_hypertext_get_link (obj1, 1, NULL);
  g_assert_nonnull (obj);
  AtspiRange *range = atspi_hyperlink_get_index_range (obj, NULL);
  g_assert_nonnull (range);
  /*
    ATK do not define this function
  */
  g_assert_cmpint (range->start_offset, ==, -1);
  g_assert_cmpint (range->end_offset, ==, -1);
  g_free (range);
  g_object_unref (obj);
  g_object_unref (obj1);
  g_object_unref (child);
}

static void
atk_test_hyperlink_get_start_index (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiHypertext *obj1 = atspi_accessible_get_hypertext_iface (child);
  g_assert_nonnull (obj1);
  AtspiHyperlink *obj = atspi_hypertext_get_link (obj1, 1, NULL);
  g_assert_nonnull (obj);
  gint n = atspi_hyperlink_get_start_index (obj, NULL);
  g_assert_cmpint (n, ==, 69);
  g_object_unref (obj);
  g_object_unref (obj1);
  g_object_unref (child);
}

static void
atk_test_hyperlink_get_end_index (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiHypertext *obj1 = atspi_accessible_get_hypertext_iface (child);
  g_assert_nonnull (obj1);
  AtspiHyperlink *obj = atspi_hypertext_get_link (obj1, 1, NULL);
  g_assert_nonnull (obj);
  gint n = atspi_hyperlink_get_end_index (obj, NULL);
  g_assert_cmpint (n, ==, 81);
  g_object_unref (obj);
  g_object_unref (obj1);
  g_object_unref (child);
}

static void
atk_test_hyperlink_is_valid (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiHypertext *obj1 = atspi_accessible_get_hypertext_iface (child);
  g_assert_nonnull (obj1);
  AtspiHyperlink *obj = atspi_hypertext_get_link (obj1, 1, NULL);
  g_assert_nonnull (obj);
  g_assert_true (atspi_hyperlink_is_valid (obj, NULL));
  g_object_unref (obj);
  g_object_unref (obj1);
  g_object_unref (child);
}

void
atk_test_hyperlink (void)
{
  g_test_add ("/hyperlink/atk_test_hyperlink_get_n_anchors",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_hyperlink_get_n_anchors, fixture_teardown);
  g_test_add ("/hyperlink/atk_test_hyperlink_get_uri",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_hyperlink_get_uri, fixture_teardown);
  g_test_add ("/hyperlink/atk_test_hyperlink_get_object",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_hyperlink_get_object, fixture_teardown);
  g_test_add ("/hyperlink/atk_test_hyperlink_get_index_range",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_hyperlink_get_index_range, fixture_teardown);
  g_test_add ("/hyperlink/atk_test_hyperlink_get_start_index",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_hyperlink_get_start_index, fixture_teardown);
  g_test_add ("/hyperlink/atk_test_hyperlink_get_end_index",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_hyperlink_get_end_index, fixture_teardown);
  g_test_add ("/hyperlink/atk_test_hyperlink_is_valid",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_hyperlink_is_valid, fixture_teardown);
}
