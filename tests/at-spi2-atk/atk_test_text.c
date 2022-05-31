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

#include "atk_test_util.h"
#include "atk_suite.h"

#define DATA_FILE TESTS_DATA_DIR"/test-text.xml"

static gboolean GHRunc_find (gpointer key, gpointer value, gpointer user_data)
{
  if (!g_strcmp0 (key, user_data)) {
    return TRUE;
  }
  return FALSE;
}

static void
atk_test_text_get_character_count (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  gint count = atspi_text_get_character_count (obj, NULL);
  g_assert_cmpint (count, ==, 16);
}

static void
atk_test_text_get_text (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  gchar *text = atspi_text_get_text (obj, 9, 14, NULL);
  g_assert_cmpstr (text, ==, "works");
  g_free (text);
}

static void
atk_test_text_get_caret_offset (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  gint pos = atspi_text_get_caret_offset (obj, NULL);
  g_assert_cmpint (pos,==,-1);
}

static void
atk_test_text_set_caret_offset (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  g_assert_true (atspi_text_set_caret_offset (obj, 5, NULL));
  g_assert_false (atspi_text_set_caret_offset (obj, -1, NULL));
  gint pos = atspi_text_get_caret_offset (obj, NULL);
  g_assert_cmpint (pos, ==, 5);
}
static void
atk_test_text_get_character_at_offset (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  guint chr = atspi_text_get_character_at_offset (obj, 5, NULL);
  g_assert_cmpint (chr, ==, 32);
}

static void
atk_test_text_get_character_extents (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  AtspiRect *rec = atspi_text_get_character_extents (obj, 6, ATSPI_COORD_TYPE_SCREEN, NULL);
  g_assert_cmpint (rec->x, ==, 100);
  g_assert_cmpint (rec->y, ==, 33);
  g_assert_cmpint (rec->width, ==, 110);
  g_assert_cmpint (rec->height, ==, 30);

  g_free (rec);
}

static void
atk_test_text_get_range_extents (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  AtspiRect *rec = atspi_text_get_range_extents (obj, 0, 1, ATSPI_COORD_TYPE_SCREEN, NULL);

  g_assert_cmpint (rec->x, ==, 100);
  g_assert_cmpint (rec->y, ==, 33);
  g_assert_cmpint (rec->width, ==, 110);
  g_assert_cmpint (rec->height, ==, 30);

  g_free (rec);
}

static void
atk_test_text_add_selection (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  g_assert_true (atspi_text_add_selection (obj, 9, 14, NULL));
}

static void
atk_test_text_get_n_selections (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  gint n = atspi_text_get_n_selections (obj, NULL);
  g_assert_cmpint (n, ==, 0);
  g_assert_true (atspi_text_add_selection (obj, 9, 14, NULL));
  g_assert_true (atspi_text_add_selection (obj, 9, 14, NULL));
  g_assert_true (atspi_text_add_selection (obj, 9, 14, NULL));
  n = atspi_text_get_n_selections (obj, NULL);
  g_assert_cmpint (n, ==, 3);
}

static void
atk_test_text_get_selection (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  g_assert_true (atspi_text_add_selection (obj, 9, 14, NULL));
  g_assert_true (atspi_text_add_selection (obj, 14, 15, NULL));

  AtspiRange *range = atspi_text_get_selection (obj, 0, NULL);
  g_assert (range);
  g_assert_cmpint (range->start_offset, ==, 9);
  g_assert_cmpint (range->end_offset, ==, 14);
  g_free (range);

  range = atspi_text_get_selection (obj, 1, NULL);
  g_assert (range);
  g_assert_cmpint (range->start_offset, ==, 14);
  g_assert_cmpint (range->end_offset, ==, 15);
  g_free (range);
}

static void
atk_test_text_set_selection (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  g_assert_true(atspi_text_add_selection (obj, 9, 14, NULL));
  g_assert_true(atspi_text_add_selection (obj, 14, 15, NULL));
  g_assert_true(atspi_text_set_selection (obj, 0, 1, 2, NULL));
  g_assert_true(atspi_text_set_selection (obj, 1, 3, 4, NULL));

  AtspiRange *range = atspi_text_get_selection (obj, 0, NULL);
  g_assert (range);
  g_assert_cmpint (range->start_offset, ==, 1);
  g_assert_cmpint (range->end_offset, ==, 2);
  g_free (range);

  range = atspi_text_get_selection (obj, 1, NULL);
  g_assert (range);
  g_assert_cmpint (range->start_offset, ==, 3);
  g_assert_cmpint (range->end_offset, ==, 4);
  g_free (range);
}

static void
atk_test_text_remove_selection (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
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
}

static void
atk_test_text_get_offset_at_point (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  gint n = atspi_text_get_offset_at_point (obj, 0, 0, ATSPI_COORD_TYPE_SCREEN, NULL);
  g_assert_cmpint (n, ==, 5);
}

static void
atk_test_text_get_text_attribute_value (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  GError *err = NULL;
  gchar *str = atspi_text_get_text_attribute_value (obj, 0, "text_test_attr1", &err);
  if (err)
    g_print ("error msg:%s\n", err->message);
  g_assert (str);
  g_assert_cmpstr (str, ==, "on");
  g_free (str);

  str = atspi_text_get_text_attribute_value (obj, 0, "text_test_attr2", NULL);
  g_assert (str);
  g_assert_cmpstr (str, ==, "off");
  g_free (str);
}

static void
atk_test_text_get_attribute_run (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  gint start_offset = 0;
  gint end_offset = 0;
  GHashTable *tab =  atspi_text_get_attribute_run (obj, 0, FALSE, &start_offset, &end_offset , NULL);

  g_assert (tab);

  g_assert_cmpstr ((const char*) g_hash_table_find (tab, GHRunc_find, "text_test_attr1"), ==, "on");
  g_assert_cmpstr ((const char*) g_hash_table_find (tab, GHRunc_find, "text_test_attr2"), ==, "off");
  g_assert_cmpint (start_offset, == , 5);
  g_assert_cmpint (end_offset, == , 10);
  g_hash_table_destroy (tab);
}

static void
atk_test_text_get_defualt_attributes (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  GHashTable *tab = atspi_text_get_default_attributes (obj, NULL);
  g_assert (tab);

  g_assert_cmpstr ((const char*) g_hash_table_find (tab, GHRunc_find, "bold_text"), ==, "on");
  g_assert_cmpstr ((const char*) g_hash_table_find (tab, GHRunc_find, "underline_text"), ==, "off");
  g_assert_cmpstr ((const char*) g_hash_table_find (tab, GHRunc_find, "dummy_text"), ==, "");
  g_hash_table_destroy (tab);
}

static void
atk_test_text_get_text_attributes (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  gint start_offset = 0;
  gint end_offset = 0;
  GHashTable *tab =  atspi_text_get_text_attributes (obj, 0, &start_offset, &end_offset, NULL);

  g_assert (tab);

  g_assert_cmpstr ((const char*) g_hash_table_find (tab, GHRunc_find, "text_test_attr1"), ==, "on");
  g_assert_cmpstr ((const char*) g_hash_table_find (tab, GHRunc_find, "text_test_attr2"), ==, "off");
  g_assert_cmpint (start_offset, == , 5);
  g_assert_cmpint (end_offset, == , 10);
  g_hash_table_destroy (tab);
}

static void
atk_test_text_get_string_at_offset_s1 (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  AtspiTextRange *range = atspi_text_get_string_at_offset (obj, 0, ATSPI_TEXT_GRANULARITY_CHAR, NULL);

  g_assert_cmpint (range->start_offset, ==, 0);
  g_assert_cmpint (range->end_offset, ==, 1);
  g_assert_cmpstr (range->content, ==, "t");

  g_free (range);

  range = atspi_text_get_string_at_offset (obj, 5, ATSPI_TEXT_GRANULARITY_WORD, NULL);

  g_assert_cmpint (range->start_offset, ==, 6);
  g_assert_cmpint (range->end_offset, ==, 7);
  g_assert_cmpstr (range->content, ==, "it");

  g_free (range);
}
static void
atk_test_text_get_string_at_offset_s2 (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 1, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  AtspiTextRange *range = atspi_text_get_string_at_offset (obj, 20, ATSPI_TEXT_GRANULARITY_SENTENCE, NULL);

  g_assert_cmpint (range->start_offset, ==, 18);
  g_assert_cmpint (range->end_offset, ==, 34);
  g_assert_cmpstr (range->content, ==, "Second sentence.");

  g_free (range);

  range = atspi_text_get_string_at_offset (obj, 21, ATSPI_TEXT_GRANULARITY_LINE, NULL);

  g_assert_cmpint (range->start_offset, ==, 18);
  g_assert_cmpint (range->end_offset, ==, 34);
  g_assert_cmpstr (range->content, ==, "Second sentence.");

  g_free (range);

  range = atspi_text_get_string_at_offset (obj, 0, ATSPI_TEXT_GRANULARITY_PARAGRAPH, NULL);

  g_assert_cmpint (range->start_offset, ==, 0);
  g_assert_cmpint (range->end_offset, ==, 0);
  g_assert_cmpstr (range->content, ==, "");

  g_free (range);
}

static void
atk_test_text_get_bounded_ranges (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiText *obj = atspi_accessible_get_text_iface (child);

  GArray *array = atspi_text_get_bounded_ranges (obj, 15, 21, 100, 50, ATSPI_COORD_TYPE_SCREEN, ATSPI_TEXT_CLIP_MAX, ATSPI_TEXT_CLIP_MIN, NULL);
  g_assert (array);

  AtspiTextRange *range  = &g_array_index (array, AtspiTextRange, 0);
  g_assert_cmpint (g_array_get_element_size (array), ==, sizeof (AtspiTextRange));
  g_assert_cmpint (range->start_offset, ==, 0);
  g_assert_cmpint (range->end_offset, ==, 5);
  g_assert_cmpstr (range->content, ==, "text0");

  range  = &g_array_index (array, AtspiTextRange, 1);
  g_assert_cmpint (g_array_get_element_size (array), ==, sizeof (AtspiTextRange));
  g_assert_cmpint (range->start_offset, ==, 6);
  g_assert_cmpint (range->end_offset, ==, 10);
  g_assert_cmpstr (range->content, ==, "it w");

  g_array_free (array,TRUE);
}

static void
teardown_text_test (gpointer fixture, gconstpointer user_data)
{
  terminate_app ();
}

void
atk_test_text (void)
{
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_get_character_count",
                     0, NULL, NULL, atk_test_text_get_character_count, teardown_text_test);
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_get_text",
                     0, NULL, NULL, atk_test_text_get_text, teardown_text_test);
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_get_caret_offset",
                     0, NULL, NULL, atk_test_text_get_caret_offset, teardown_text_test);
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_get_text_attributes",
                     0, NULL, NULL, atk_test_text_get_text_attributes, teardown_text_test);
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_get_attribute_run",
                     0, NULL, NULL, atk_test_text_get_attribute_run, teardown_text_test);
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_get_text_attribute_value",
                     0, NULL, NULL, atk_test_text_get_text_attribute_value, teardown_text_test);
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_get_defualt_attributes",
                     0, NULL, NULL, atk_test_text_get_defualt_attributes, teardown_text_test);
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_set_caret_offset",
                     0, NULL, NULL, atk_test_text_set_caret_offset, teardown_text_test);
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_get_string_at_offset_s1",
                     0, NULL, NULL, atk_test_text_get_string_at_offset_s1, teardown_text_test );
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_get_string_at_offset_s2",
                     0, NULL, NULL, atk_test_text_get_string_at_offset_s2, teardown_text_test );
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_get_character_at_offset",
                     0, NULL, NULL, atk_test_text_get_character_at_offset, teardown_text_test);
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_get_character_extents",
                     0, NULL, NULL, atk_test_text_get_character_extents, teardown_text_test);
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_get_offset_at_point",
                     0, NULL, NULL, atk_test_text_get_offset_at_point, teardown_text_test);
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_get_range_extents",
                     0, NULL, NULL, atk_test_text_get_range_extents, teardown_text_test);
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_get_bounded_ranges",
                     0, NULL, NULL, atk_test_text_get_bounded_ranges, teardown_text_test);
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_get_n_selections",
                     0, NULL, NULL, atk_test_text_get_n_selections, teardown_text_test);
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_get_selection",
                     0, NULL, NULL, atk_test_text_get_selection, teardown_text_test);
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_add_selection",
                     0, NULL, NULL, atk_test_text_add_selection, teardown_text_test );
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_remove_selection",
                     0, NULL, NULL, atk_test_text_remove_selection, teardown_text_test );
  g_test_add_vtable (ATK_TEST_PATH_TEXT "/atk_test_text_set_selection",
                     0, NULL, NULL, atk_test_text_set_selection, teardown_text_test);
}
