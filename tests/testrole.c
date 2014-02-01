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

static gboolean
test_role (void)
{
  AtkRole role1, role2;
  const gchar *name;
  gboolean result = TRUE;

  name = atk_role_get_name (ATK_ROLE_PAGE_TAB);
  if (!name || strcmp (name, "page tab") != 0)
    {
      g_print ("Unexpected name for ATK_ROLE_PAGE_TAB."
               " Expected 'page tab', received '%s'\n", name);
      result = FALSE;
    }

  name = atk_role_get_name (ATK_ROLE_LAYERED_PANE);
  if (!name || strcmp (name, "layered pane") != 0)
    {
      g_print ("Unexpected name for ATK_ROLE_LAYERED_PANE."
               " Expected 'layered pane', received '%s'\n", name);
      result = FALSE;
    }

  role1 = atk_role_for_name ("list item");
  if (role1 != ATK_ROLE_LIST_ITEM)
    {
      g_print ("Unexpected role for list item."
               " Expected %i, received %i\n", ATK_ROLE_LIST_ITEM, role1);
      result = FALSE;
    }

  role2 = atk_role_for_name ("TEST_ROLE");
  if (role2 != ATK_ROLE_INVALID)
    {
      g_print ("Unexpected role for TEST_ROLE. Expected %i, received %i\n", ATK_ROLE_INVALID, role2);
      result = FALSE;
    }
  /*
   * Check that a non-existent role returns NULL
   */
  name = atk_role_get_name (ATK_ROLE_LAST_DEFINED + 2);
  if (name)
    {
      g_print ("Unexpected name for undefined role %s\n", name);
      result = FALSE;
    }

  return result;
}

static void
print_roles()
{
  AtkRole role;

  g_print("(Role, name, localized name) defined by the ATK library:\n");

  for (role = ATK_ROLE_INVALID; role < ATK_ROLE_LAST_DEFINED; role++)
    g_print ("(%i, %s, %s)\n", role,
             atk_role_get_name(role), atk_role_get_localized_name(role));

  g_print("(Role, name, localized name) for the extra roles:\n");
  for (;atk_role_get_name(role) != NULL; role++)
    g_print ("(%i, %s, %s)\n", role,
             atk_role_get_name(role), atk_role_get_localized_name(role));

}

int
main (int argc, char **argv)
{
  gboolean b_ret;

  g_print ("Starting Role test suite\n");

  b_ret = test_role ();

  print_roles();

  if (b_ret)
    g_print ("Role tests succeeded\n");
  else
    g_print ("Role tests failed\n");

  return 0;
}
