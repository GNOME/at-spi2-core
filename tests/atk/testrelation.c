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

static void
test_relation (void)
{
  AtkRelationType type1, type2;
  const gchar *name;
  AtkObject *obj;
  gboolean ret_value;
  AtkRelationSet *set;
  AtkRelation *relation;
  gint n_relations;
  GPtrArray *array;

  name = atk_relation_type_get_name (ATK_RELATION_LABEL_FOR);
  g_assert_cmpstr (name, ==, "label-for");

  name = atk_relation_type_get_name (ATK_RELATION_NODE_CHILD_OF);
  g_assert_cmpstr (name, ==, "node-child-of");

  name = atk_relation_type_get_name (ATK_RELATION_EMBEDS);
  g_assert_cmpstr (name, ==, "embeds");

  type1 = atk_relation_type_for_name ("embedded-by");
  g_assert_cmpint (type1, ==, ATK_RELATION_EMBEDDED_BY);

  type1 = atk_relation_type_for_name ("controlled-by");
  g_assert_cmpint (type1, ==, ATK_RELATION_CONTROLLED_BY);

  type1 = atk_relation_type_register ("test-state");
  name = atk_relation_type_get_name (type1);
  g_assert_cmpstr (name, ==, "test-state");
  type2 = atk_relation_type_for_name ("test-state");
  g_assert_cmpint (type1, ==, type2);
  type2 = atk_relation_type_for_name ("TEST_STATE");
  g_assert_cmpint (type2, ==, 0);
  /*
   * Check that a non-existent type returns NULL
   */
  g_assert_null (atk_relation_type_get_name (ATK_RELATION_LAST_DEFINED + 2));

  obj = g_object_new (ATK_TYPE_OBJECT, NULL);
  ret_value = atk_object_add_relationship (obj, ATK_RELATION_LABEL_FOR, obj);
  g_assert_cmpint (ret_value, !=, 0);
  set = atk_object_ref_relation_set (obj);
  g_assert_nonnull (set);
  n_relations = atk_relation_set_get_n_relations (set);
  g_assert_cmpint (n_relations, ==, 1);
  relation = atk_relation_set_get_relation (set, 0);
  g_assert_nonnull (relation);
  type1 = atk_relation_get_relation_type (relation);
  g_assert_cmpint (type1, ==, ATK_RELATION_LABEL_FOR);
  array = atk_relation_get_target (relation);
  g_assert_true (obj == g_ptr_array_index (array, 0));
  g_object_unref (set);
  g_assert_true (atk_object_remove_relationship (obj, ATK_RELATION_LABEL_FOR, obj));
  set = atk_object_ref_relation_set (obj);
  g_assert_nonnull (set);
  n_relations = atk_relation_set_get_n_relations (set);
  g_assert_cmpint (n_relations, ==, 0);
  g_object_unref (set);
  g_object_unref (obj);
}

static void
test_text_attr (void)
{
  AtkTextAttribute attr1, attr2;
  const gchar *name;

  name = atk_text_attribute_get_name (ATK_TEXT_ATTR_PIXELS_INSIDE_WRAP);
  g_assert_cmpstr (name, ==, "pixels-inside-wrap");

  name = atk_text_attribute_get_name (ATK_TEXT_ATTR_BG_STIPPLE);
  g_assert_cmpstr (name, ==, "bg-stipple");

  attr1 = atk_text_attribute_for_name ("left-margin");
  g_assert_cmpint (attr1, ==, ATK_TEXT_ATTR_LEFT_MARGIN);

  attr1 = atk_text_attribute_register ("test-attribute");
  name = atk_text_attribute_get_name (attr1);
  g_assert_cmpstr (name, ==, "test-attribute");
  attr2 = atk_text_attribute_for_name ("test-attribute");
  g_assert_cmpint (attr1, ==, attr2);
  g_assert_cmpint (atk_text_attribute_for_name ("TEST_ATTR"), ==, 0);
  /*
   * Check that a non-existent attribute returns NULL
   */
  g_assert_null (atk_text_attribute_get_name (ATK_TEXT_ATTR_LAST_DEFINED + 2));
}

int
main (gint argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);
  g_test_add_func ("/atk/relation/relation", test_relation);
  g_test_add_func ("/atk/relation/text_attr", test_text_attr);

  return g_test_run ();
}
