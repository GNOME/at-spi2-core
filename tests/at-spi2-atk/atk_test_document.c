/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; https://wiki.gnome.org/Accessibility)
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#define DATA_FILE TESTS_DATA_DIR"/test-document.xml"

static void
teardown_document_test (gpointer fixture, gconstpointer user_data)
{
  terminate_app ();
}

static void
atk_test_document_get_document_iface (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiDocument *iface = atspi_accessible_get_document_iface (child);
  g_assert (iface != NULL);
}

static void
atk_test_document_get_locale (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiDocument *iface = atspi_accessible_get_document_iface (child);
  g_assert (iface != NULL);

  g_assert_cmpstr (atspi_document_get_locale (iface, NULL), ==, "document_locale");
}

static void
atk_test_document_get_attribute_value (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiDocument *iface = atspi_accessible_get_document_iface (child);
  g_assert (iface != NULL);

  g_assert_cmpstr (atspi_document_get_document_attribute_value (iface, "atspi1", NULL), ==, "test1");
  g_assert_cmpstr (atspi_document_get_document_attribute_value (iface, "atspi2", NULL), ==, "test2");
}

static void
atk_test_document_get_attributes (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiDocument *iface = atspi_accessible_get_document_iface (child);
  g_assert (iface != NULL);

  GHashTable *attr = atspi_document_get_document_attributes (iface, NULL);
  GHashTableIter iter;
  gpointer key, value;
  g_hash_table_iter_init (&iter, attr);

  gchar *valid_keys[] = {"atspi1", "atspi2"};
  gchar *valid_values[] = {"test1", "test2"};

  int i=0;
  while (g_hash_table_iter_next (&iter, &key, &value)) {
    g_assert_cmpstr (valid_keys[i], ==, (gchar *)key);
    g_assert_cmpstr (valid_values[i], ==, (gchar *)value);
    ++i;
  }
}

void
atk_test_document(void )
{
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_document_get_document_iface",
                     0, NULL, NULL, atk_test_document_get_document_iface, teardown_document_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_document_get_locale",
                     0, NULL, NULL, atk_test_document_get_locale, teardown_document_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_document_get_attribute_value",
                     0, NULL, NULL, atk_test_document_get_attribute_value, teardown_document_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_document_get_attributes",
                     0, NULL, NULL, atk_test_document_get_attributes, teardown_document_test );
}
