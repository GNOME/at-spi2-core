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

#define DATA_FILE TESTS_DATA_DIR "/test-value.xml"

static void
atk_test_value_get_value_iface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiValue *obj = atspi_accessible_get_value_iface (child);
  g_assert_nonnull (obj);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_value_get_minimum_value (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiValue *obj = atspi_accessible_get_value_iface (child);
  gdouble val = atspi_value_get_minimum_value (obj, NULL);
  g_assert_cmpfloat (val, ==, 1.25);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_value_get_current_value (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiValue *obj = atspi_accessible_get_value_iface (child);
  gdouble val = atspi_value_get_current_value (obj, NULL);
  g_assert_cmpfloat (val, ==, 2.25);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_value_get_maximum_value (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiValue *obj = atspi_accessible_get_value_iface (child);
  gdouble val = atspi_value_get_maximum_value (obj, NULL);
  g_assert_cmpfloat (val, ==, 3.25);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_value_set_current_value (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiValue *obj = atspi_accessible_get_value_iface (child);
  g_assert_true (atspi_value_set_current_value (obj, 2.5, NULL));
  gdouble val = atspi_value_get_current_value (obj, NULL);
  g_assert_cmpfloat (val, ==, 2.5);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_value_get_minimum_increment (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiValue *obj = atspi_accessible_get_value_iface (child);
  gdouble val = atspi_value_get_minimum_increment (obj, NULL);
  g_assert_cmpfloat (val, ==, 0.25);
  g_object_unref (obj);
  g_object_unref (child);
}

static void
atk_test_value_get_text (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = fixture->root_obj;
  g_assert_nonnull (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert_nonnull (child);
  AtspiValue *obj = atspi_accessible_get_value_iface (child);
  gchar *text = atspi_value_get_text (obj, NULL);
  g_assert_cmpstr (text, ==, "2.250000");
  g_free (text);
  g_object_unref (obj);
  g_object_unref (child);
}

void
atk_test_value (void)
{
  g_test_add ("/value/atk_test_value_get_value_iface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_value_get_value_iface, fixture_teardown);
  g_test_add ("/value/atk_test_value_get_minimum_value",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_value_get_minimum_value, fixture_teardown);
  g_test_add ("/value/atk_test_value_get_current_value",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_value_get_current_value, fixture_teardown);
  g_test_add ("/value/atk_test_value_get_maximum_value",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_value_get_maximum_value, fixture_teardown);
  g_test_add ("/value/atk_test_value_set_current_value",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_value_set_current_value, fixture_teardown);
  g_test_add ("/value/atk_test_value_get_minimum_increment",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_value_get_minimum_increment, fixture_teardown);
  g_test_add ("/value/atk_test_value_get_text",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_value_get_text, fixture_teardown);
}
