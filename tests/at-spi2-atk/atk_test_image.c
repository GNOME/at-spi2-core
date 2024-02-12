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

#define DATA_FILE TESTS_DATA_DIR "/test-image.xml"

static void
atk_test_image_sample_get_interface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  check_name (obj, "root_object");
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiImage *iface = atspi_accessible_get_image_iface (child);
  g_assert_nonnull (iface);
  g_object_unref (iface);
  g_object_unref (child);
}

static void
atk_test_image_get_image_description (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiImage *image = atspi_accessible_get_image_iface (child);
  gchar *desc = atspi_image_get_image_description (image, NULL);
  g_assert_nonnull (desc);
  g_assert_cmpstr (desc, ==, "image description");
  g_free (desc);
  g_object_unref (image);
  g_object_unref (child);
}

static void
atk_test_image_get_image_size (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiImage *image = atspi_accessible_get_image_iface (child);
  AtspiPoint *p = atspi_image_get_image_size (image, NULL);
  g_assert_nonnull (p);

  g_assert_cmpint (p->x, ==, 100);
  g_assert_cmpint (p->y, ==, 50);
  g_free (p);
  g_object_unref (image);
  g_object_unref (child);
}

static void
atk_test_image_get_image_position (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiImage *image = atspi_accessible_get_image_iface (child);
  AtspiPoint *p = atspi_image_get_image_position (image, ATSPI_COORD_TYPE_SCREEN, NULL);
  g_assert_nonnull (p);
  g_assert_cmpint (p->x, ==, 500);
  g_assert_cmpint (p->y, ==, 50);
  g_free (p);
  g_object_unref (image);
  g_object_unref (child);
}

static void
atk_test_image_get_image_extents (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiImage *image = atspi_accessible_get_image_iface (child);
  AtspiRect *r = atspi_image_get_image_extents (image, ATSPI_COORD_TYPE_SCREEN, NULL);
  g_assert_nonnull (r);

  g_assert_cmpint (r->x, ==, 500);
  g_assert_cmpint (r->y, ==, 50);
  g_assert_cmpint (r->width, ==, 100);
  g_assert_cmpint (r->height, ==, 50);

  g_free (r);
  g_object_unref (image);
  g_object_unref (child);
}

static void
atk_test_image_get_image_locale (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiImage *image = atspi_accessible_get_image_iface (child);
  gchar *locale = atspi_image_get_image_locale (image, NULL);

  g_assert_nonnull (locale);
  g_assert_cmpstr (locale, ==, "image_locale");
  g_free (locale);
  g_object_unref (image);
  g_object_unref (child);
}

void
atk_test_image (void)
{
  g_test_add ("/image/atk_test_image_sample_get_interface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_image_sample_get_interface, fixture_teardown);
  g_test_add ("/image/atk_test_image_get_image_description",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_image_get_image_description, fixture_teardown);
  g_test_add ("/image/atk_test_image_get_image_size",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_image_get_image_size, fixture_teardown);
  g_test_add ("/image/atk_test_image_get_image_position",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_image_get_image_position, fixture_teardown);
  g_test_add ("/image/atk_test_image_get_image_extents",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_image_get_image_extents, fixture_teardown);
  g_test_add ("/image/atk_test_image_get_image_locale",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_image_get_image_locale, fixture_teardown);
}
