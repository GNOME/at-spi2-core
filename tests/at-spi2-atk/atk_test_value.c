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

#define DATA_FILE TESTS_DATA_DIR"/test-value.xml"

static void
atk_test_value_get_value_iface (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiValue *obj = atspi_accessible_get_value_iface (child);
  g_assert (obj);
}

static void
atk_test_value_get_minimum_value (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiValue *obj = atspi_accessible_get_value_iface (child);
  gdouble val = atspi_value_get_minimum_value (obj, NULL);
  g_assert_cmpfloat (val, ==, 1.25);
}

static void
atk_test_value_get_current_value (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiValue *obj = atspi_accessible_get_value_iface (child);
  gdouble val = atspi_value_get_current_value (obj, NULL);
  g_assert_cmpfloat (val, ==, 2.25);
}

static void
atk_test_value_get_maximum_value (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiValue *obj = atspi_accessible_get_value_iface (child);
  gdouble val = atspi_value_get_maximum_value (obj, NULL);
  g_assert_cmpfloat (val, ==, 3.25);
}

static void
atk_test_value_set_current_value (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiValue *obj = atspi_accessible_get_value_iface (child);
  g_assert (atspi_value_set_current_value (obj, 2.5, NULL));
  gdouble val = atspi_value_get_current_value (obj, NULL);
  g_assert_cmpfloat (val, ==, 2.5);
}

static void
atk_test_value_get_minimum_increment (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *_obj = get_root_obj (DATA_FILE);
  g_assert (_obj);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (_obj, 0, NULL);
  g_assert (child);
  AtspiValue *obj = atspi_accessible_get_value_iface (child);
  gdouble val = atspi_value_get_minimum_increment (obj, NULL);
  g_assert_cmpfloat (val, ==, 0.25);
}

static void
teardown_value_test (gpointer fixture, gconstpointer user_data)
{
  terminate_app ();
}

void
atk_test_value (void)
{
  g_test_add_vtable (ATK_TEST_PATH_VALUE "/atk_test_value_get_value_iface",
                     0, NULL, NULL, atk_test_value_get_value_iface, teardown_value_test );
  g_test_add_vtable (ATK_TEST_PATH_VALUE "/atk_test_value_get_minimum_value",
                     0, NULL, NULL, atk_test_value_get_minimum_value, teardown_value_test );
  g_test_add_vtable (ATK_TEST_PATH_VALUE "/atk_test_value_get_current_value",
                     0, NULL, NULL, atk_test_value_get_current_value, teardown_value_test );
  g_test_add_vtable (ATK_TEST_PATH_VALUE "/atk_test_value_get_maximum_value",
                     0, NULL, NULL, atk_test_value_get_maximum_value, teardown_value_test );
  g_test_add_vtable (ATK_TEST_PATH_VALUE "/atk_test_value_set_current_value",
                     0, NULL, NULL, atk_test_value_set_current_value, teardown_value_test );
  g_test_add_vtable (ATK_TEST_PATH_VALUE "/atk_test_value_get_minimum_increment",
                     0, NULL, NULL, atk_test_value_get_minimum_increment, teardown_value_test );
}
