/* ATK -  Accessibility Toolkit
 * Copyright 2001 Sun Microsystems Inc.
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

static gboolean  test_relation (void);

static gboolean
test_relation (void)
{
  AtkRelationType type1, type2;
  G_CONST_RETURN gchar *name;

  name = atk_relation_type_get_name (ATK_RELATION_LABEL_FOR);
  g_return_val_if_fail (name, FALSE);
  if (strcmp (name, "label-for") != 0)
    {
      g_print ("Unexpected name for ATK_RELATION_LABEL_FOR %s\n", name);
      return FALSE;
    }

  name = atk_relation_type_get_name (ATK_RELATION_NODE_CHILD_OF);
  g_return_val_if_fail (name, FALSE);
  if (strcmp (name, "node-child-of") != 0)
    {
      g_print ("Unexpected name for ATK_STATE_MODAL %s\n", name);
      return FALSE;
    }

  type1 = atk_relation_type_for_name ("controlled-by");
  if (type1 != ATK_RELATION_CONTROLLED_BY)
    {
      g_print ("Unexpected type for focused\n");
      return FALSE;
    }

  type1 = atk_relation_type_register ("test-state");
  name = atk_relation_type_get_name (type1);
  g_return_val_if_fail (name, FALSE);
  if (strcmp (name, "test-state") != 0)
    {
      g_print ("Unexpected name for test-state %s\n", name);
      return FALSE;
    }
  type2 = atk_relation_type_for_name ("test-state");
  if (type1 != type2)
  {
    g_print ("Unexpected type for test-state\n");
    return FALSE;
  }
  type2 = atk_relation_type_for_name ("TEST_STATE");
  if (type2 != 0)
    {
      g_print ("Unexpected type for TEST_STATE\n");
      return FALSE;
    }
  /*
   * Check that a non-existent type returns NULL
   */
  name = atk_relation_type_get_name (ATK_RELATION_LAST_DEFINED + 2);
  if (name)
    {
      g_print ("Unexpected name for undefined type %s\n", name);
      return FALSE;
    }
  return TRUE;
}

int
gtk_module_init (gint  argc, 
                 char* argv[])
{
  gboolean b_ret;

  g_print("Relation test module loaded\n");

  b_ret = test_relation ();
  if (b_ret)
    g_print ("Relation tests succeeded\n");
  else
    g_print ("Relation tests failed\n");
  return 0;
}
