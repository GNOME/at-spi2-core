/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; https://wiki.gnome.org/Accessibility)
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <libintl.h>
#define _(x) dgettext ("at-spi2-core", x)

#define DATA_FILE TESTS_DATA_DIR "/test-accessible.xml"

static void
atk_test_accessible_get_name (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  check_name (obj, "root_object");
}

static void
atk_test_accessible_get_description (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  gchar *str = atspi_accessible_get_description (obj, NULL);
  g_assert_cmpstr (str, ==, "Root of the accessible tree");
  g_free (str);
}

static void
atk_test_accessible_get_child_count (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  gint child_c = atspi_accessible_get_child_count (obj, NULL);
  g_assert_cmpint (3, ==, child_c);
}

static void
atk_test_accessible_get_parent (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiAccessible *parent = atspi_accessible_get_parent (child, NULL);
  g_assert_true (parent == obj);
  g_object_unref (parent);
  g_object_unref (child);
}

static void
atk_test_accessible_get_child_at_index (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL);
  check_name (child, "obj2");
  g_object_unref (child);
}

static void
atk_test_accessible_get_index_in_parent (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 2, NULL);
  int index = atspi_accessible_get_index_in_parent (child, NULL);
  g_assert_cmpint (index, ==, 2);
  g_object_unref (child);
}

static void
atk_test_accessible_get_relation_set_1 (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child1 = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (child1, 0, NULL);
  GArray *rel_set = atspi_accessible_get_relation_set (child, NULL);
  g_assert_cmpint (rel_set->len, ==, 1);
  g_object_unref (g_array_index (rel_set, AtspiRelation *, 0));
  g_array_free (rel_set, TRUE);
  g_object_unref (child);
  g_object_unref (child1);
}

static void
atk_test_accessible_get_relation_set_2 (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *obj2 = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiAccessible *obj2_1 = atspi_accessible_get_child_at_index (obj2, 0, NULL);
  GArray *rel_set = atspi_accessible_get_relation_set (obj2_1, NULL);
  int i = 0;
  for (i = 0; i < rel_set->len; i++)
    {
      AtspiRelation *a = g_array_index (rel_set, AtspiRelation *, i);
      g_assert_cmpint (atspi_relation_get_relation_type (a), ==, ATSPI_RELATION_CONTROLLER_FOR);
      g_assert_cmpint (atspi_relation_get_n_targets (a), ==, 1);
      AtspiAccessible *target = atspi_relation_get_target (a, 0);
      check_name (target, "obj2");
      g_object_unref (target);
      g_object_unref (a);
    }
  g_array_free (rel_set, TRUE);
  g_object_unref (obj2_1);
  g_object_unref (obj2);
}

static void
atk_test_accessible_get_role (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiRole root_role = atspi_accessible_get_role (obj, NULL);
  g_assert_cmpint (root_role, ==, ATSPI_ROLE_ACCELERATOR_LABEL);
}

static void
atk_test_accessible_get_role_name (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  gchar *root_role_name = atspi_accessible_get_role_name (obj, NULL);
  g_assert_cmpstr (root_role_name, ==, "accelerator label");
  g_free (root_role_name);
}

static void
atk_test_accessible_get_localized_role_name (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  gchar *root_role_name = atspi_accessible_get_localized_role_name (obj, NULL);
  g_assert_cmpstr (root_role_name, ==, _ ("accelerator label"));
  g_free (root_role_name);
}

static void
atk_test_accessible_get_state_set (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiStateSet *states = atspi_accessible_get_state_set (child);
  GArray *states_arr = atspi_state_set_get_states (states);

  AtspiStateType valid_states[] = {
    ATSPI_STATE_MODAL,
    ATSPI_STATE_MULTI_LINE,
  };
  g_assert_cmpint (states_arr->len, ==, 2);
  int i = 0;
  for (i = 0; i < states_arr->len; ++i)
    {
      g_assert_cmpint (valid_states[i], ==, g_array_index (states_arr, AtspiStateType, i));
      g_assert_true (atspi_state_set_contains (states, ATSPI_STATE_MODAL));
      g_assert_true (atspi_state_set_contains (states, ATSPI_STATE_MULTI_LINE));
    }
  g_array_free (states_arr, TRUE);
  g_object_unref (states);
  g_object_unref (child);
}

static void
atk_test_accessible_get_attributes (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  GHashTable *attr_hash_tab = atspi_accessible_get_attributes (obj, NULL);
  GHashTableIter iter;
  gpointer key, value;

  g_hash_table_iter_init (&iter, attr_hash_tab);

  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      const char *key_str = key;
      const char *value_str = value;

      if (strcmp (key_str, "atspi1") == 0)
        {
          g_assert_cmpstr (value_str, ==, "test1");
        }
      else if (strcmp (key_str, "atspi2") == 0)
        {
          g_assert_cmpstr (value_str, ==, "test2");
        }
      else
        {
          g_assert_not_reached ();
        }
    }
  g_hash_table_unref (attr_hash_tab);
}

static void
atk_test_accessible_get_attributes_as_array (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  GArray *attr_arr = atspi_accessible_get_attributes_as_array (obj, NULL);
  int i;
  g_assert_cmpint (attr_arr->len, ==, 2);
  for (i = 0; i < attr_arr->len; ++i)
    {
      char *pair = g_array_index (attr_arr, gchar *, i);

      g_assert_true (strcmp (pair, "atspi1:test1") == 0 || strcmp (pair, "atspi2:test2") == 0);
      g_free (pair);
    }
  g_array_free (attr_arr, TRUE);
}

static void
atk_test_accessible_get_toolkit_name (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  gchar *toolkit_name = atspi_accessible_get_toolkit_name (obj, NULL);

  g_assert_cmpstr (toolkit_name, ==, "atspitesting-toolkit");
  g_free (toolkit_name);
}

static void
atk_test_accessible_get_toolkit_version (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  gchar *toolkit_ver = atspi_accessible_get_toolkit_version (obj, NULL);
  /* should be empty string, because no value is setted */
  g_assert_cmpstr (toolkit_ver, ==, "");
  g_free (toolkit_ver);
}

static void
atk_test_accessible_get_atspi_version (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  gchar *atspi_version = atspi_accessible_get_atspi_version (obj, NULL);
  g_assert_cmpstr (atspi_version, ==, "2.1");
  g_free (atspi_version);
}

static void
atk_test_accessible_get_id (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  gint app_id = atspi_accessible_get_id (obj, NULL);
  g_assert_cmpint (app_id, !=, -1);
}

static void
atk_test_accessible_get_application (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *app = atspi_accessible_get_application (obj, NULL);
  g_assert_nonnull (app);
  g_object_unref (app);
}

static void
atk_test_accessible_get_action_iface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAction *iface = atspi_accessible_get_action_iface (obj);
  g_assert_null (iface);
}

static void
atk_test_accessible_get_collection_iface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiCollection *iface = atspi_accessible_get_collection_iface (obj);
  g_assert_nonnull (iface);
  g_object_unref (iface);
}

static void
atk_test_accessible_get_component_iface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiComponent *iface = atspi_accessible_get_component_iface (obj);
  g_assert_null (iface);
}

static void
atk_test_accessible_get_document_iface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiDocument *iface = atspi_accessible_get_document_iface (obj);
  g_assert_null (iface);
}

static void
atk_test_accessible_get_editable_text_iface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiEditableText *iface = atspi_accessible_get_editable_text_iface (obj);
  g_assert_null (iface);
}

static void
atk_test_accessible_get_hypertext_iface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiHypertext *iface = atspi_accessible_get_hypertext_iface (obj);
  g_assert_null (iface);
}

static void
atk_test_accessible_get_image_iface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiImage *iface = atspi_accessible_get_image_iface (obj);
  g_assert_null (iface);
}

static void
atk_test_accessible_get_selection_iface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiSelection *iface = atspi_accessible_get_selection_iface (obj);
  g_assert_null (iface);
}

static void
atk_test_accessible_get_table_iface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiTable *iface = atspi_accessible_get_table_iface (obj);
  g_assert_null (iface);
}

static void
atk_test_accessible_get_text_iface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiText *iface = atspi_accessible_get_text_iface (obj);
  g_assert_null (iface);
}

static void
atk_test_accessible_get_value_iface (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiValue *iface = atspi_accessible_get_value_iface (obj);
  g_assert_null (iface);
}

static void
atk_test_accessible_get_interfaces (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  gchar *valid_obj_ifaces[] = { "Accessible", "Collection" };

  GArray *ifaces = atspi_accessible_get_interfaces (obj);
  g_assert_cmpint (ifaces->len, ==, 2);
  int i = 0;
  for (i = 0; i < ifaces->len; ++i)
    {
      g_assert_cmpstr (valid_obj_ifaces[i], ==, g_array_index (ifaces, gchar *, i));
      g_free (g_array_index (ifaces, gchar *, i));
    }
  g_array_free (ifaces, TRUE);
}

static void
atk_test_accessible_get_object_locale (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  const gchar *obj_locale = atspi_accessible_get_object_locale (obj, NULL);
  g_assert_cmpstr (obj_locale, ==, setlocale (LC_MESSAGES, NULL));
}

static void
atk_test_accessible_set_cache_mask (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiCache cache_mask = ATSPI_CACHE_ROLE;
  atspi_accessible_set_cache_mask (obj, cache_mask);
  g_assert_cmpint (obj->parent.app->cache, ==, cache_mask);
}

static void
atk_test_check_cache_cleared (AtspiAccessible *obj)
{
  g_assert_cmpint (obj->cached_properties, ==, ATSPI_CACHE_NONE);
  GPtrArray *array = obj->children;
  int i;
  for (i = 0; i < array->len; i++)
    atk_test_check_cache_cleared (g_ptr_array_index (array, i));
}

static void
atk_test_accessible_clear_cache (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  atspi_accessible_clear_cache (obj);
  atk_test_check_cache_cleared (obj);
}

static void
atk_test_accessible_get_process_id (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  guint proc_id = atspi_accessible_get_process_id (obj, NULL);
  g_assert_cmpint (proc_id, ==, fixture->child_pid);
}

static void
atk_test_accessible_get_help_text (TestAppFixture *fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = fixture->root_obj;
  AtspiAccessible *child1 = atspi_accessible_get_child_at_index (obj, 0, NULL);
  gchar *str = atspi_accessible_get_help_text (child1, NULL);
  g_assert_cmpstr (str, ==, "test dialog");
  g_free (str);
  g_object_unref (child1);
}

void
atk_test_accessible (void)
{
  g_test_add ("/accessible/atk_test_accessible_get_name",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_name, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_description",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_description, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_child_count",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_child_count, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_parent",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_parent, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_child_at_index",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_child_at_index, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_index_in_parent",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_index_in_parent, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_relation_set_1",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_relation_set_1, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_relation_set_2",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_relation_set_2, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_role",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_role, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_role_name",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_role_name, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_localized_role_name",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_localized_role_name, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_state_set",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_state_set, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_attributes",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_attributes, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_attributes_as_array",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_attributes_as_array, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_toolkit_name",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_toolkit_name, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_toolkit_version",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_toolkit_version, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_atspi_version",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_atspi_version, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_id",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_id, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_application",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_application, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_action_iface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_action_iface, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_collection_iface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_collection_iface, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_component_iface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_component_iface, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_document_iface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_document_iface, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_editable_text_iface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_editable_text_iface, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_hypertext_iface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_hypertext_iface, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_image_iface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_image_iface, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_selection_iface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_selection_iface, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_table_iface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_table_iface, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_text_iface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_text_iface, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_value_iface",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_value_iface, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_interfaces",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_interfaces, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_object_locale",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_object_locale, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_set_cache_mask",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_set_cache_mask, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_clear_cache",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_clear_cache, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_process_id",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_process_id, fixture_teardown);
  g_test_add ("/accessible/atk_test_accessible_get_help_text",
              TestAppFixture, DATA_FILE, fixture_setup, atk_test_accessible_get_help_text, fixture_teardown);
}
