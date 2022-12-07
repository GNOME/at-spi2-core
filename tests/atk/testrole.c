/* ATK -  Accessibility Toolkit
 * Copyright (C) 2013 Igalia, S.L.
 *
 * Author: Alejandro Piñeiro Iglesias <apinheiro@igalia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include <atk/atk.h>
#include <string.h>

static void
test_role (void)
{
  AtkRole role1, role2;
  const gchar *name;

  name = atk_role_get_name (ATK_ROLE_PAGE_TAB);
  g_assert_cmpstr (name, ==, "page tab");

  name = atk_role_get_name (ATK_ROLE_LAYERED_PANE);
  g_assert_cmpstr (name, ==, "layered pane");

  role1 = atk_role_for_name ("list item");
  g_assert_cmpint (role1, ==, ATK_ROLE_LIST_ITEM);

  role2 = atk_role_for_name ("TEST_ROLE");
  g_assert_cmpint (role2, ==, ATK_ROLE_INVALID);
  /*
   * Check that a non-existent role returns NULL
   */
  g_assert_null (atk_role_get_name (ATK_ROLE_LAST_DEFINED + 2));
}

int
main (gint argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);
  g_test_add_func ("/atk/role/roles", test_role);

  return g_test_run ();
}
