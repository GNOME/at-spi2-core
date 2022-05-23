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
atk_test_hypertext_get_n_links (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiHypertext *obj = atspi_accessible_get_hypertext_iface (child);
  g_assert (obj);
  gint cnt = atspi_hypertext_get_n_links (obj, NULL);
  g_assert_cmpint (cnt, ==, 2);
}

static void
atk_test_hypertext_get_link (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiHypertext *obj = atspi_accessible_get_hypertext_iface (child);
  g_assert (obj);
  AtspiHyperlink *link = atspi_hypertext_get_link (obj, 1, NULL);
  g_assert (link);
  gchar *str = atspi_hyperlink_get_uri (link, 0, NULL);
  g_assert (str);
  g_assert_cmpstr (str, ==, "pinkbike.com");

  g_free (str);

  link = atspi_hypertext_get_link (obj, 0, NULL);
  str = atspi_hyperlink_get_uri (link, 0, NULL);
  g_assert_cmpstr (str, ==, "dh-zone.com");

  g_free (str);
}

static void
atk_test_hypertext_get_link_index (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiHypertext *obj = atspi_accessible_get_hypertext_iface (child);
  g_assert (obj);
  gint cnt = atspi_hypertext_get_link_index (obj, 15, NULL);
  g_assert_cmpint (cnt, ==, -1);
  cnt = atspi_hypertext_get_link_index (obj, 55, NULL);
  g_assert_cmpint (cnt, ==, 0);
  cnt = atspi_hypertext_get_link_index (obj, 70, NULL);
  g_assert_cmpint (cnt, ==, 1);
}

static void
teardown_hypertext_test (gpointer fixture, gconstpointer user_data)
{
  terminate_app ();
}

void
atk_test_hypertext (void)
{
  g_test_add_vtable (ATK_TEST_PATH_HYPERTEXT "/atk_test_hypertext_get_n_links",
                     0, NULL, NULL, atk_test_hypertext_get_n_links, teardown_hypertext_test );
  g_test_add_vtable (ATK_TEST_PATH_HYPERTEXT "/atk_test_hypertext_get_links",
                     0, NULL, NULL, atk_test_hypertext_get_link, teardown_hypertext_test );
  g_test_add_vtable (ATK_TEST_PATH_HYPERTEXT "/atk_test_hypertext_get_link_index",
                     0, NULL, NULL, atk_test_hypertext_get_link_index, teardown_hypertext_test );
}
