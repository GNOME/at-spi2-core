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

#define DATA_FILE TESTS_DATA_DIR "/test-component.xml"

static void
atk_test_component_sample (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  check_name (obj, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert_nonnull (iface);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_component_contains (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert_nonnull (iface);

  gboolean ret = atspi_component_contains (iface, 400, 300, ATSPI_COORD_TYPE_SCREEN, NULL);
  g_assert_true (ret);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_component_get_accessible_at_point (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert_nonnull (iface);

  AtspiAccessible *r = atspi_component_get_accessible_at_point (iface,
                                                                400,
                                                                300,
                                                                ATSPI_COORD_TYPE_SCREEN,
                                                                NULL);
  g_assert_nonnull (r);
  g_object_unref (r);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_component_get_extents (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert_nonnull (iface);

  AtspiRect *r = atspi_component_get_extents (iface, ATSPI_COORD_TYPE_SCREEN, NULL);
  g_assert_cmpint (r->x, ==, 350);
  g_assert_cmpint (r->y, ==, 200);
  g_assert_cmpint (r->width, ==, 250);
  g_assert_cmpint (r->height, ==, 250);
  g_free (r);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_component_get_layer (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert_nonnull (iface);

  AtspiComponentLayer layer = atspi_component_get_layer (iface, NULL);
  g_assert_cmpint (layer, ==, ATSPI_LAYER_WIDGET);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_component_get_mdi_z_order (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert_nonnull (iface);

  gshort ret = atspi_component_get_mdi_z_order (iface, NULL);
  g_assert_cmpint (ret, ==, 2);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_component_grab_focus (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert_nonnull (iface);

  gboolean ret = atspi_component_grab_focus (iface, NULL);
  g_assert_true (ret);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_component_get_alpha (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert_nonnull (iface);

  gdouble ret = atspi_component_get_alpha (iface, NULL);
  g_assert_cmpfloat (ret, ==, 2.5);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_component_set_extents (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiComponent *iface = atspi_accessible_get_component_iface (child);
  g_assert_nonnull (iface);

  AtspiRect *r = atspi_component_get_extents (iface, ATSPI_COORD_TYPE_SCREEN, NULL);
  g_assert_cmpint (r->x, ==, 350);
  g_assert_cmpint (r->y, ==, 200);
  g_assert_cmpint (r->width, ==, 250);
  g_assert_cmpint (r->height, ==, 250);
  g_free (r);

  gboolean ret = atspi_component_set_extents (iface, 100, 100, 100, 100, ATSPI_COORD_TYPE_SCREEN, NULL);
  g_assert_true (ret);

  r = atspi_component_get_extents (iface, ATSPI_COORD_TYPE_SCREEN, NULL);
  g_assert_cmpint (r->x, ==, 100);
  g_assert_cmpint (r->y, ==, 100);
  g_assert_cmpint (r->width, ==, 100);
  g_assert_cmpint (r->height, ==, 100);
  g_free (r);
  g_object_unref (iface);
  g_object_unref (child);
}

void
atk_test_component (void)
{
  g_test_add ("/component/atk_test_component_sample",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_component_sample, fixture_teardown);
  g_test_add ("/component/atk_test_component_contains",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_component_contains, fixture_teardown);
  g_test_add ("/component/atk_test_component_get_accessible_at_point",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_component_get_accessible_at_point, fixture_teardown);
  g_test_add ("/component/atk_test_component_get_extents",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_component_get_extents, fixture_teardown);
  g_test_add ("/component/atk_test_component_get_layer",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_component_get_layer, fixture_teardown);
  g_test_add ("/component/atk_test_component_get_mdi_z_order",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_component_get_mdi_z_order, fixture_teardown);
  g_test_add ("/component/atk_test_component_grab_focus",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_component_grab_focus, fixture_teardown);
  g_test_add ("/component/atk_test_component_get_alpha",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_component_get_alpha, fixture_teardown);
  g_test_add ("/component/atk_test_component_set_extents",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_component_set_extents, fixture_teardown);
}
