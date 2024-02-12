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
atk_test_hypertext_get_n_links (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiHypertext *obj = atspi_accessible_get_hypertext_iface (child);
  g_assert_nonnull (obj);
  gint cnt = atspi_hypertext_get_n_links (obj, NULL);
  g_assert_cmpint (cnt, ==, 2);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_hypertext_get_link (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiHypertext *obj = atspi_accessible_get_hypertext_iface (child);
  g_assert_nonnull (obj);
  AtspiHyperlink *link = atspi_hypertext_get_link (obj, 1, NULL);
  g_assert_nonnull (link);
  gchar *str = atspi_hyperlink_get_uri (link, 0, NULL);
  g_assert_nonnull (str);
  g_assert_cmpstr (str, ==, "pinkbike.com");

  g_free (str);
  g_object_unref (link);

  link = atspi_hypertext_get_link (obj, 0, NULL);
  str = atspi_hyperlink_get_uri (link, 0, NULL);
  g_assert_cmpstr (str, ==, "dh-zone.com");

  g_free (str);
  g_object_unref (link);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_hypertext_get_link_index (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiHypertext *obj = atspi_accessible_get_hypertext_iface (child);
  g_assert_nonnull (obj);
  gint cnt = atspi_hypertext_get_link_index (obj, 15, NULL);
  g_assert_cmpint (cnt, ==, -1);
  cnt = atspi_hypertext_get_link_index (obj, 55, NULL);
  g_assert_cmpint (cnt, ==, 0);
  cnt = atspi_hypertext_get_link_index (obj, 70, NULL);
  g_assert_cmpint (cnt, ==, 1);
  g_object_unref (obj);
  g_object_unref (child);
}

void
atk_test_hypertext (void)
{
  g_test_add ("/hypertext/atk_test_hypertext_get_n_links",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_hypertext_get_n_links, fixture_teardown);
  g_test_add ("/hypertext/atk_test_hypertext_get_links",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_hypertext_get_link, fixture_teardown);
  g_test_add ("/hypertext/atk_test_hypertext_get_link_index",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_hypertext_get_link_index, fixture_teardown);
}
