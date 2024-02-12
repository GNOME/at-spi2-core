/*
 * AT-SPI - Assistive Service Provider Interface
 * (Gnome AccessibiliTestAppFixture, DATA_FILE, fixture_setup://wiki.gnome.org/Accessibility)
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

#define DATA_FILE TESTS_DATA_DIR "/test-document.xml"

static void
atk_test_document_get_document_iface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiDocument *iface = atspi_accessible_get_document_iface (child);
  g_assert_nonnull (iface);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_document_get_locale (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiDocument *iface = atspi_accessible_get_document_iface (child);
  g_assert_nonnull (iface);

  gchar *str = atspi_document_get_locale (iface, NULL);
  g_assert_cmpstr (str, ==, "document_locale");
  g_free (str);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_document_get_attribute_value (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiDocument *iface = atspi_accessible_get_document_iface (child);
  gchar *str;
  g_assert_nonnull (iface);

  str = atspi_document_get_document_attribute_value (iface, "atspi1", NULL);
  g_assert_cmpstr (str, ==, "test1");
  g_free (str);
  str = atspi_document_get_document_attribute_value (iface, "atspi2", NULL);
  g_assert_cmpstr (str, ==, "test2");
  g_free (str);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_document_get_attributes (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiDocument *iface = atspi_accessible_get_document_iface (child);
  g_assert_nonnull (iface);

  GHashTable *attr = atspi_document_get_document_attributes (iface, NULL);
  GHashTableIter iter;
  gpointer key, value;
  g_hash_table_iter_init (&iter, attr);

  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      const char *key_str = key;
      const char *value_str = value;

      if (strcmp (key_str, "atspi1") == 0)
        {
          g_assert_cmpstr (value_str, ==, "test1");
        }
      else if (strcmp (key_str, "atspi2") == 0)
        {
          g_assert_cmpstr (value_str, ==, "test2");
        }
      else
        {
          g_assert_not_reached ();
        }
    }
  g_hash_table_unref (attr);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_document_text_selections (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiDocument *iface = atspi_accessible_get_document_iface (child);
  GArray *selections;
  AtspiTextSelection selection;

  g_assert_nonnull (iface);

  selections = atspi_document_get_text_selections (iface, NULL);
  g_assert_true (selections);
  g_assert_cmpint (selections->len, ==, 0);
  g_array_free (selections, TRUE);

  selections = g_array_new (FALSE, TRUE, sizeof (AtspiTextSelection));
  selection.start_object = obj;
  selection.start_offset = 0;
  selection.end_object = obj;
  selection.end_offset = 2;
  selection.start_is_active = TRUE;
  g_array_append_val (selections, selection);
  selection.start_offset = 3;
  selection.end_offset = 5;
  selection.start_is_active = FALSE;
  g_array_append_val (selections, selection);
  atspi_document_set_text_selections (iface, selections, NULL);
  g_array_free (selections, TRUE);

  selections = atspi_document_get_text_selections (iface, NULL);
  g_assert_true (selections);
  g_assert_cmpint (selections->len, ==, 2);
  selection = g_array_index (selections, AtspiTextSelection, 0);
  g_assert_true (selection.start_object == obj);
  g_assert_cmpint (selection.start_offset, ==, 0);
  g_assert_cmpint (selection.end_offset, ==, 2);
  g_assert_true (selection.start_is_active);
  selection = g_array_index (selections, AtspiTextSelection, 1);
  g_assert_true (selection.start_object == obj);
  g_assert_cmpint (selection.start_offset, ==, 3);
  g_assert_cmpint (selection.end_offset, ==, 5);
  g_assert_false (selection.start_is_active);
  g_array_free (selections, TRUE);

  g_object_unref (iface);
  g_object_unref (child);
}

void
atk_test_document (void)
{
  g_test_add ("/document/atk_test_document_get_document_iface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_document_get_document_iface, fixture_teardown);
  g_test_add ("/document/atk_test_document_get_locale",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_document_get_locale, fixture_teardown);
  g_test_add ("/document/atk_test_document_get_attribute_value",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_document_get_attribute_value, fixture_teardown);
  g_test_add ("/document/atk_test_document_get_attributes",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_document_get_attributes, fixture_teardown);
  g_test_add ("/document/atk_test_document_text_selections",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_document_text_selections, fixture_teardown);
}
