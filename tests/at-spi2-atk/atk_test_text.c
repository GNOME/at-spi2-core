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

#define DATA_FILE TESTS_DATA_DIR "/test-text.xml"

static gboolean
GHRunc_find (gpointer key, gpointer value, gpointer user_data)
{
  if (!g_strcmp0 (key, user_data))
    {
      return TRUE;
    }
  return FALSE;
}

static void
atk_test_text_get_character_count (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  gint count = atspi_text_get_character_count (obj, NULL);
  g_assert_cmpint (count, ==, 16);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_get_text (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  gchar *text = atspi_text_get_text (obj, 9, 14, NULL);
  g_assert_cmpstr (text, ==, "works");
  g_free (text);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_get_caret_offset (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  gint pos = atspi_text_get_caret_offset (obj, NULL);
  g_assert_cmpint (pos, ==, -1);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_set_caret_offset (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  g_assert_true (atspi_text_set_caret_offset (obj, 5, NULL));
  g_assert_false (atspi_text_set_caret_offset (obj, -1, NULL));
  gint pos = atspi_text_get_caret_offset (obj, NULL);
  g_assert_cmpint (pos, ==, 5);
  g_object_unref (obj);
  g_object_unref (child);
}
static void
atk_test_text_get_character_at_offset (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  guint chr = atspi_text_get_character_at_offset (obj, 5, NULL);
  g_assert_cmpint (chr, ==, 32);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_get_character_extents (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  AtspiRect *rec = atspi_text_get_character_extents (obj, 6, ATSPI_COORD_TYPE_SCREEN, NULL);
  g_assert_cmpint (rec->x, ==, 100);
  g_assert_cmpint (rec->y, ==, 33);
  g_assert_cmpint (rec->width, ==, 110);
  g_assert_cmpint (rec->height, ==, 30);

  g_free (rec);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_get_range_extents (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  AtspiRect *rec = atspi_text_get_range_extents (obj, 0, 1, ATSPI_COORD_TYPE_SCREEN, NULL);

  g_assert_cmpint (rec->x, ==, 100);
  g_assert_cmpint (rec->y, ==, 33);
  g_assert_cmpint (rec->width, ==, 110);
  g_assert_cmpint (rec->height, ==, 30);

  g_free (rec);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_add_selection (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  g_assert_true (atspi_text_add_selection (obj, 9, 14, NULL));
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_get_n_selections (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  gint n = atspi_text_get_n_selections (obj, NULL);
  g_assert_cmpint (n, ==, 0);
  g_assert_true (atspi_text_add_selection (obj, 9, 14, NULL));
  g_assert_true (atspi_text_add_selection (obj, 9, 14, NULL));
  g_assert_true (atspi_text_add_selection (obj, 9, 14, NULL));
  n = atspi_text_get_n_selections (obj, NULL);
  g_assert_cmpint (n, ==, 3);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_get_selection (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  g_assert_true (atspi_text_add_selection (obj, 9, 14, NULL));
  g_assert_true (atspi_text_add_selection (obj, 14, 15, NULL));

  AtspiRange *range = atspi_text_get_selection (obj, 0, NULL);
  g_assert_nonnull (range);
  g_assert_cmpint (range->start_offset, ==, 9);
  g_assert_cmpint (range->end_offset, ==, 14);
  g_free (range);

  range = atspi_text_get_selection (obj, 1, NULL);
  g_assert_nonnull (range);
  g_assert_cmpint (range->start_offset, ==, 14);
  g_assert_cmpint (range->end_offset, ==, 15);
  g_free (range);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_set_selection (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  g_assert_true (atspi_text_add_selection (obj, 9, 14, NULL));
  g_assert_true (atspi_text_add_selection (obj, 14, 15, NULL));
  g_assert_true (atspi_text_set_selection (obj, 0, 1, 2, NULL));
  g_assert_true (atspi_text_set_selection (obj, 1, 3, 4, NULL));

  AtspiRange *range = atspi_text_get_selection (obj, 0, NULL);
  g_assert_nonnull (range);
  g_assert_cmpint (range->start_offset, ==, 1);
  g_assert_cmpint (range->end_offset, ==, 2);
  g_free (range);

  range = atspi_text_get_selection (obj, 1, NULL);
  g_assert_nonnull (range);
  g_assert_cmpint (range->start_offset, ==, 3);
  g_assert_cmpint (range->end_offset, ==, 4);
  g_free (range);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_remove_selection (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  gint n = atspi_text_get_n_selections (obj, NULL);
  g_assert_cmpint (n, ==, 0);

  g_assert_true (atspi_text_add_selection (obj, 9, 14, NULL));
  g_assert_true (atspi_text_add_selection (obj, 14, 15, NULL));
  n = atspi_text_get_n_selections (obj, NULL);
  g_assert_cmpint (n, ==, 2);

  g_assert_true (atspi_text_remove_selection (obj, 1, NULL));
  n = atspi_text_get_n_selections (obj, NULL);
  g_assert_cmpint (n, ==, 1);

  g_assert_true (atspi_text_remove_selection (obj, 0, NULL));
  n = atspi_text_get_n_selections (obj, NULL);
  g_assert_cmpint (n, ==, 0);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_get_offset_at_point (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  gint n = atspi_text_get_offset_at_point (obj, 0, 0, ATSPI_COORD_TYPE_SCREEN, NULL);
  g_assert_cmpint (n, ==, 5);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_get_text_attribute_value (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  GError *err = NULL;
  gchar *str = atspi_text_get_text_attribute_value (obj, 0, "text_test_attr1", &err);
  if (err)
    g_print ("error msg:%s\n", err->message);
  g_assert_nonnull (str);
  g_assert_cmpstr (str, ==, "on");
  g_free (str);

  str = atspi_text_get_text_attribute_value (obj, 0, "text_test_attr2", NULL);
  g_assert_nonnull (str);
  g_assert_cmpstr (str, ==, "off");
  g_free (str);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_get_attribute_run (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  gint start_offset = 0;
  gint end_offset = 0;
  GHashTable *tab = atspi_text_get_attribute_run (obj, 0, FALSE, &start_offset, &end_offset, NULL);

  g_assert_nonnull (tab);

  g_assert_cmpstr ((const char *) g_hash_table_find (tab, GHRunc_find, "text_test_attr1"), ==, "on");
  g_assert_cmpstr ((const char *) g_hash_table_find (tab, GHRunc_find, "text_test_attr2"), ==, "off");
  g_assert_cmpint (start_offset, ==, 5);
  g_assert_cmpint (end_offset, ==, 10);
  g_hash_table_destroy (tab);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_get_default_attributes (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  GHashTable *tab = atspi_text_get_default_attributes (obj, NULL);
  g_assert_nonnull (tab);

  g_assert_cmpstr ((const char *) g_hash_table_find (tab, GHRunc_find, "bold_text"), ==, "on");
  g_assert_cmpstr ((const char *) g_hash_table_find (tab, GHRunc_find, "underline_text"), ==, "off");
  g_assert_cmpstr ((const char *) g_hash_table_find (tab, GHRunc_find, "dummy_text"), ==, "");
  g_hash_table_destroy (tab);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_get_text_attributes (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  gint start_offset = 0;
  gint end_offset = 0;
  GHashTable *tab = atspi_text_get_text_attributes (obj, 0, &start_offset, &end_offset, NULL);

  g_assert_nonnull (tab);

  g_assert_cmpstr ((const char *) g_hash_table_find (tab, GHRunc_find, "text_test_attr1"), ==, "on");
  g_assert_cmpstr ((const char *) g_hash_table_find (tab, GHRunc_find, "text_test_attr2"), ==, "off");
  g_assert_cmpint (start_offset, ==, 5);
  g_assert_cmpint (end_offset, ==, 10);
  g_hash_table_destroy (tab);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_get_string_at_offset_s1 (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  AtspiTextRange *range = atspi_text_get_string_at_offset (obj, 0, ATSPI_TEXT_GRANULARITY_CHAR, NULL);

  g_assert_cmpint (range->start_offset, ==, 0);
  g_assert_cmpint (range->end_offset, ==, 1);
  g_assert_cmpstr (range->content, ==, "t");

  g_boxed_free (ATSPI_TYPE_TEXT_RANGE, range);

  range = atspi_text_get_string_at_offset (obj, 5, ATSPI_TEXT_GRANULARITY_WORD, NULL);

  g_assert_cmpint (range->start_offset, ==, 6);
  g_assert_cmpint (range->end_offset, ==, 7);
  g_assert_cmpstr (range->content, ==, "it");

  g_boxed_free (ATSPI_TYPE_TEXT_RANGE, range);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_get_string_at_offset_s2 (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 1, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  AtspiTextRange *range = atspi_text_get_string_at_offset (obj, 20, ATSPI_TEXT_GRANULARITY_SENTENCE, NULL);

  g_assert_cmpint (range->start_offset, ==, 18);
  g_assert_cmpint (range->end_offset, ==, 34);
  g_assert_cmpstr (range->content, ==, "Second sentence.");

  g_boxed_free (ATSPI_TYPE_TEXT_RANGE, range);

  range = atspi_text_get_string_at_offset (obj, 21, ATSPI_TEXT_GRANULARITY_LINE, NULL);

  g_assert_cmpint (range->start_offset, ==, 18);
  g_assert_cmpint (range->end_offset, ==, 34);
  g_assert_cmpstr (range->content, ==, "Second sentence.");

  g_boxed_free (ATSPI_TYPE_TEXT_RANGE, range);

  range = atspi_text_get_string_at_offset (obj, 0, ATSPI_TEXT_GRANULARITY_PARAGRAPH, NULL);

  g_assert_cmpint (range->start_offset, ==, 0);
  g_assert_cmpint (range->end_offset, ==, 0);
  g_assert_cmpstr (range->content, ==, "");

  g_boxed_free (ATSPI_TYPE_TEXT_RANGE, range);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_text_get_bounded_ranges (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  GArray *array = atspi_text_get_bounded_ranges (obj, 15, 21, 100, 50, ATSPI_COORD_TYPE_SCREEN, ATSPI_TEXT_CLIP_MAX, ATSPI_TEXT_CLIP_MIN, NULL);
  g_assert_nonnull (array);

  AtspiTextRange *range = &g_array_index (array, AtspiTextRange, 0);
  g_assert_cmpint (g_array_get_element_size (array), ==, sizeof (AtspiTextRange));
  g_assert_cmpint (range->start_offset, ==, 0);
  g_assert_cmpint (range->end_offset, ==, 5);
  g_assert_cmpstr (range->content, ==, "text0");
  g_free (range->content);

  range = &g_array_index (array, AtspiTextRange, 1);
  g_assert_cmpint (g_array_get_element_size (array), ==, sizeof (AtspiTextRange));
  g_assert_cmpint (range->start_offset, ==, 6);
  g_assert_cmpint (range->end_offset, ==, 10);
  g_assert_cmpstr (range->content, ==, "it w");
  g_free (range->content);

  g_array_free (array, TRUE);
  g_object_unref (obj);
  g_object_unref (child);
}

void
atk_test_text (void)
{
  g_test_add ("/text/atk_test_text_get_character_count",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_get_character_count, fixture_teardown);
  g_test_add ("/text/atk_test_text_get_text",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_get_text, fixture_teardown);
  g_test_add ("/text/atk_test_text_get_caret_offset",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_get_caret_offset, fixture_teardown);
  g_test_add ("/text/atk_test_text_get_text_attributes",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_get_text_attributes, fixture_teardown);
  g_test_add ("/text/atk_test_text_get_attribute_run",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_get_attribute_run, fixture_teardown);
  g_test_add ("/text/atk_test_text_get_text_attribute_value",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_get_text_attribute_value, fixture_teardown);
  g_test_add ("/text/atk_test_text_get_default_attributes",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_get_default_attributes, fixture_teardown);
  g_test_add ("/text/atk_test_text_set_caret_offset",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_set_caret_offset, fixture_teardown);
  g_test_add ("/text/atk_test_text_get_string_at_offset_s1",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_get_string_at_offset_s1, fixture_teardown);
  g_test_add ("/text/atk_test_text_get_string_at_offset_s2",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_get_string_at_offset_s2, fixture_teardown);
  g_test_add ("/text/atk_test_text_get_character_at_offset",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_get_character_at_offset, fixture_teardown);
  g_test_add ("/text/atk_test_text_get_character_extents",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_get_character_extents, fixture_teardown);
  g_test_add ("/text/atk_test_text_get_offset_at_point",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_get_offset_at_point, fixture_teardown);
  g_test_add ("/text/atk_test_text_get_range_extents",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_get_range_extents, fixture_teardown);
  g_test_add ("/text/atk_test_text_get_bounded_ranges",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_get_bounded_ranges, fixture_teardown);
  g_test_add ("/text/atk_test_text_get_n_selections",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_get_n_selections, fixture_teardown);
  g_test_add ("/text/atk_test_text_get_selection",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_get_selection, fixture_teardown);
  g_test_add ("/text/atk_test_text_add_selection",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_add_selection, fixture_teardown);
  g_test_add ("/text/atk_test_text_remove_selection",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_remove_selection, fixture_teardown);
  g_test_add ("/text/atk_test_text_set_selection",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_text_set_selection, fixture_teardown);
}
