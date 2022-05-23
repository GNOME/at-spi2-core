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

#define DATA_FILE TESTS_DATA_DIR"/test-component.xml"

static void
teardown_component_test (gpointer fixture, gconstpointer user_data)
{
  terminate_app ();
}

static void
atk_test_component_sample (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  g_assert_cmpstr (atspi_accessible_get_name (obj, NULL), ==, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert (iface != NULL);
}

static void
atk_test_component_contains (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert (iface != NULL);

  gboolean ret = atspi_component_contains (iface, 400, 300, ATSPI_COORD_TYPE_SCREEN, NULL);
  g_assert (ret != FALSE);
}

static void
atk_test_component_get_accessible_at_point (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert (iface != NULL);

  AtspiAccessible *r = atspi_component_get_accessible_at_point (iface,
                       400,
                       300,
                       ATSPI_COORD_TYPE_SCREEN,
                       NULL);
  g_assert (r != NULL);
}

static void
atk_test_component_get_extents (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert (iface != NULL);

  AtspiRect *r = atspi_component_get_extents (iface, ATSPI_COORD_TYPE_SCREEN, NULL);
  g_assert_cmpint (r->x, ==, 350);
  g_assert_cmpint (r->y, ==, 200);
  g_assert_cmpint (r->width, ==, 250);
  g_assert_cmpint (r->height, ==, 250);
  g_free (r);
}

static void
atk_test_component_get_layer (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert (iface != NULL);

  AtspiComponentLayer layer = atspi_component_get_layer (iface, NULL);
  g_assert_cmpint (layer, ==, ATSPI_LAYER_WIDGET);
}

static void
atk_test_component_get_mdi_z_order (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert (iface != NULL);

  gshort ret = atspi_component_get_mdi_z_order (iface, NULL);
  g_assert_cmpint (ret, ==, 2);
}

static void
atk_test_component_grab_focus (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert (iface != NULL);

  gboolean ret = atspi_component_grab_focus (iface, NULL);
  g_assert (ret != FALSE);
}

static void
atk_test_component_get_alpha (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert (iface != NULL);

  gdouble ret = atspi_component_get_alpha (iface, NULL);
  g_assert_cmpint (ret, ==, 2.5);
}

static void
atk_test_component_set_extents (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert (iface != NULL);

  AtspiRect *r = atspi_component_get_extents (iface, ATSPI_COORD_TYPE_SCREEN, NULL);
  g_assert_cmpint (r->x, ==, 350);
  g_assert_cmpint (r->y, ==, 200);
  g_assert_cmpint (r->width, ==, 250);
  g_assert_cmpint (r->height, ==, 250);
  g_free (r);

  gboolean ret = atspi_component_set_extents (iface, 100, 100, 100, 100, ATSPI_COORD_TYPE_SCREEN, NULL);
  g_assert (ret != FALSE);

  r = atspi_component_get_extents (iface, ATSPI_COORD_TYPE_SCREEN, NULL);
  g_assert_cmpint (r->x, ==, 100);
  g_assert_cmpint (r->y, ==, 100);
  g_assert_cmpint (r->width, ==, 100);
  g_assert_cmpint (r->height, ==, 100);
  g_free (r);
}

void
atk_test_component (void)
{
  g_test_add_vtable (ATK_TEST_PATH_COMP "/atk_test_component_sample",
                     0, NULL, NULL, atk_test_component_sample, teardown_component_test);
  g_test_add_vtable (ATK_TEST_PATH_COMP "/atk_test_component_contains",
                     0, NULL, NULL, atk_test_component_contains, teardown_component_test);
  g_test_add_vtable (ATK_TEST_PATH_COMP "/atk_test_component_get_accessible_at_point",
                     0, NULL, NULL, atk_test_component_get_accessible_at_point, teardown_component_test);
  g_test_add_vtable (ATK_TEST_PATH_COMP "/atk_test_component_get_extents",
                     0, NULL, NULL, atk_test_component_get_extents, teardown_component_test);
  g_test_add_vtable (ATK_TEST_PATH_COMP "/atk_test_component_get_layer",
                     0, NULL, NULL, atk_test_component_get_layer, teardown_component_test);
  g_test_add_vtable (ATK_TEST_PATH_COMP "/atk_test_component_get_mdi_z_order",
                     0, NULL, NULL, atk_test_component_get_mdi_z_order, teardown_component_test);
  g_test_add_vtable (ATK_TEST_PATH_COMP "/atk_test_component_grab_focus",
                     0, NULL, NULL, atk_test_component_grab_focus, teardown_component_test);
  g_test_add_vtable (ATK_TEST_PATH_COMP "/atk_test_component_get_alpha",
                     0, NULL, NULL, atk_test_component_get_alpha, teardown_component_test);
  g_test_add_vtable (ATK_TEST_PATH_COMP "/atk_test_component_set_extents",
                     0, NULL, NULL, atk_test_component_set_extents, teardown_component_test);
}
