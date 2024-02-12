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

#define DATA_FILE TESTS_DATA_DIR "/test-editable-text.xml"

static void
atk_test_editable_text_get_editable_text_iface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiEditableText *iface = atspi_accessible_get_editable_text_iface (child);
  g_assert_nonnull (iface);
  g_object_unref (iface);
  g_object_unref (child);
}

#if 0
/* Function is in docs but not implemented */
static void
atk_test_editable_text_set_attributes  (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiEditableText *iface = atspi_accessible_get_editable_text_iface (child);
  g_assert_nonnull (iface);

  g_assert_true (atspi_editable_text_set_attributes (iface, "attribute", 1, 2, NULL));
}
#endif

static void
atk_test_editable_text_insert_text (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiEditableText *iface = atspi_accessible_get_editable_text_iface (child);
  g_assert_nonnull (iface);
  g_assert_true (atspi_editable_text_insert_text (iface, 0, "test_text", 9, NULL));
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_editable_text_copy_text (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiEditableText *iface = atspi_accessible_get_editable_text_iface (child);
  g_assert_nonnull (iface);
  g_assert_true (atspi_editable_text_copy_text (iface, 1, 2, NULL));
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_editable_text_cut_text (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiEditableText *iface = atspi_accessible_get_editable_text_iface (child);
  g_assert_nonnull (iface);
  g_assert_true (atspi_editable_text_cut_text (iface, 1, 2, NULL));
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_editable_text_delete_text (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiEditableText *iface = atspi_accessible_get_editable_text_iface (child);
  g_assert_nonnull (iface);

  g_assert_true (atspi_editable_text_delete_text (iface, 1, 2, NULL));
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_editable_text_paste_text (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiEditableText *iface = atspi_accessible_get_editable_text_iface (child);
  g_assert_nonnull (iface);

  g_assert_true (atspi_editable_text_paste_text (iface, 2, NULL));
  g_object_unref (iface);
  g_object_unref (child);
}

void
atk_test_editable_text (void)
{
  g_test_add ("/editable_text/atk_test_editable_text_get_editable_text_iface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_editable_text_get_editable_text_iface, fixture_teardown);
#if 0
  g_test_add ("/editable_text/atk_test_editable_text_set_attributes",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_editable_text_set_attributes, fixture_teardown);
#endif
  g_test_add ("/editable_text/atk_test_editable_text_insert_text",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_editable_text_insert_text, fixture_teardown);
  g_test_add ("/editable_text/atk_test_editable_text_copy_text",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_editable_text_copy_text, fixture_teardown);
  g_test_add ("/editable_text/atk_test_editable_text_cut_text",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_editable_text_cut_text, fixture_teardown);
  g_test_add ("/editable_text/atk_test_editable_text_delete_text",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_editable_text_delete_text, fixture_teardown);
  g_test_add ("/editable_text/atk_test_editable_text_paste_text",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_editable_text_paste_text, fixture_teardown);
}
