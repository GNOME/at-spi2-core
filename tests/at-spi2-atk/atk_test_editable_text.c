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

#define DATA_FILE TESTS_DATA_DIR"/test-editable-text.xml"

static void
teardown_editable_text_test (gpointer fixture, gconstpointer user_data)
{
  terminate_app ();
}

static void
atk_test_editable_text_get_editable_text_iface (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiEditableText *iface = atspi_accessible_get_editable_text_iface (child);
  g_assert (iface != NULL);
}

#if 0
/* Function is in docs but not implemented */
static void
atk_test_editable_text_set_attributes  (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiEditableText *iface = atspi_accessible_get_editable_text_iface (child);
  g_assert (iface != NULL);

  g_assert (atspi_editable_text_set_attributes (iface, "attribute", 1, 2, NULL));
}
#endif

static void
atk_test_editable_text_insert_text (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiEditableText *iface = atspi_accessible_get_editable_text_iface (child);
  g_assert (iface != NULL);
  g_assert (atspi_editable_text_insert_text (iface, 0, "test_text", 9, NULL));
}

static void
atk_test_editable_text_copy_text (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiEditableText *iface = atspi_accessible_get_editable_text_iface (child);
  g_assert (iface != NULL);
  g_assert (atspi_editable_text_copy_text (iface, 1, 2, NULL));
}

static void
atk_test_editable_text_cut_text (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiEditableText *iface = atspi_accessible_get_editable_text_iface (child);
  g_assert (iface != NULL);
  g_assert (atspi_editable_text_cut_text (iface, 1, 2, NULL));
}

static void
atk_test_editable_text_delete_text (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiEditableText *iface = atspi_accessible_get_editable_text_iface (child);
  g_assert (iface != NULL);

  g_assert (atspi_editable_text_delete_text (iface, 1, 2, NULL));
}

static void
atk_test_editable_text_paste_text (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiEditableText *iface = atspi_accessible_get_editable_text_iface (child);
  g_assert (iface != NULL);

  g_assert (atspi_editable_text_paste_text (iface, 2, NULL));
}

void
atk_test_editable_text(void )
{
  g_test_add_vtable (ATK_TEST_PATH_EDIT_TEXT "/atk_test_editable_text_get_editable_text_iface",
                     0, NULL, NULL, atk_test_editable_text_get_editable_text_iface, teardown_editable_text_test);
#if 0
  g_test_add_vtable (ATK_TEST_PATH_EDIT_TEXT "/atk_test_editable_text_set_attributes",
                     0, NULL, NULL, atk_test_editable_text_set_attributes, teardown_editable_text_test);
#endif
  g_test_add_vtable (ATK_TEST_PATH_EDIT_TEXT "/atk_test_editable_text_insert_text",
                     0, NULL, NULL, atk_test_editable_text_insert_text, teardown_editable_text_test);
  g_test_add_vtable (ATK_TEST_PATH_EDIT_TEXT "/atk_test_editable_text_copy_text",
                     0, NULL, NULL, atk_test_editable_text_copy_text, teardown_editable_text_test);
  g_test_add_vtable (ATK_TEST_PATH_EDIT_TEXT "/atk_test_editable_text_cut_text",
                     0, NULL, NULL, atk_test_editable_text_cut_text, teardown_editable_text_test);
  g_test_add_vtable (ATK_TEST_PATH_EDIT_TEXT "/atk_test_editable_text_delete_text",
                     0, NULL, NULL, atk_test_editable_text_delete_text, teardown_editable_text_test);
  g_test_add_vtable (ATK_TEST_PATH_EDIT_TEXT "/atk_test_editable_text_paste_text",
                     0, NULL, NULL, atk_test_editable_text_paste_text, teardown_editable_text_test);
}
