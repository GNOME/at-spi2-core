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

#define DATA_FILE TESTS_DATA_DIR"/test-accessible.xml"

static void
teardown_accessible_test (gpointer fixture, gconstpointer user_data)
{
  terminate_app ();
}

static void
atk_test_accessible_get_name (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  g_assert_cmpstr (atspi_accessible_get_name (obj, NULL), ==, "root_object");
}

static void
atk_test_accessible_get_description (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  g_assert_cmpstr (atspi_accessible_get_description (obj, NULL), ==, "Root of the accessible tree" );
}

static void
atk_test_accessible_get_child_count (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  gint child_c = atspi_accessible_get_child_count (obj, NULL);
  g_assert_cmpint ( 3, ==, child_c );
}

static void
atk_test_accessible_get_parent (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL );
  AtspiAccessible *parent = atspi_accessible_get_parent (child, NULL );
  g_assert (parent == obj );
}

static void
atk_test_accessible_get_child_at_index (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 1, NULL );
  g_assert_cmpstr (atspi_accessible_get_name (child, NULL), ==, "obj2");
}

static void
atk_test_accessible_get_index_in_parent (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 2, NULL);
  int index = atspi_accessible_get_index_in_parent (child, NULL);
  g_assert_cmpint (index, ==, 2);
}

static void
atk_test_accessible_get_relation_set_1 (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child1 = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (child1, 0, NULL);
  GArray *rel_set = atspi_accessible_get_relation_set (child, NULL);
  g_assert_cmpint (rel_set->len, == , 1);
}

static void
atk_test_accessible_get_relation_set_2 (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *obj2 = atspi_accessible_get_child_at_index (obj, 1, NULL);
  AtspiAccessible *obj2_1 = atspi_accessible_get_child_at_index (obj2, 0, NULL);
  GArray *rel_set = atspi_accessible_get_relation_set (obj2_1, NULL);
  int i=0;
  for (i = 0; i < rel_set->len; i++) {
    AtspiRelation *a = g_array_index (rel_set, AtspiRelation *, i);
    g_assert_cmpint (atspi_relation_get_relation_type (a), == , ATSPI_RELATION_CONTROLLER_FOR );
    g_assert_cmpint (atspi_relation_get_n_targets (a), ==, 1);
    AtspiAccessible *target = atspi_relation_get_target (a, 0);
    g_assert_cmpstr (atspi_accessible_get_name (target,NULL), == , "obj2");
  }
}

static void
atk_test_accessible_get_role (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiRole root_role = atspi_accessible_get_role (obj, NULL);
  g_assert_cmpint (root_role, ==, ATSPI_ROLE_ACCELERATOR_LABEL);
}

static void
atk_test_accessible_get_role_name (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  gchar *root_role_name = atspi_accessible_get_role_name (obj, NULL);
  g_assert_cmpstr (root_role_name, ==, "accelerator label");
}

static void
atk_test_accessible_get_localized_role_name (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  gchar *root_role_name = atspi_accessible_get_localized_role_name (obj, NULL);
  g_assert_cmpstr (root_role_name, ==, "accelerator label");
}

static void
atk_test_accessible_get_state_set (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
  AtspiStateSet *states = atspi_accessible_get_state_set (child);
  GArray *states_arr = atspi_state_set_get_states (states);

  AtspiStateType valid_states[] = {
    ATSPI_STATE_MODAL,
    ATSPI_STATE_MULTI_LINE,
  };
  g_assert_cmpint (states_arr->len, ==, 2);
  int i = 0;
  for (i = 0; i < states_arr->len; ++i) {
    g_assert_cmpint (valid_states[i], ==, g_array_index (states_arr, AtspiStateType, i));
    g_assert (atspi_state_set_contains (states, ATSPI_STATE_MODAL));
    g_assert (atspi_state_set_contains(states, ATSPI_STATE_MULTI_LINE));
  }
}

static void
atk_test_accessible_get_attributes (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  GHashTable *attr_hash_tab = atspi_accessible_get_attributes (obj, NULL);
  GHashTableIter iter;
  gpointer key, value;

  gchar *valid_keys[] = { "atspi" };
  gchar *valid_values[] = { "test" };

  g_hash_table_iter_init (&iter, attr_hash_tab );
  int i = 0;
  while (g_hash_table_iter_next (&iter, &key, &value)) {
    g_assert_cmpstr (valid_keys[i], ==, (gchar *)key );
    g_assert_cmpstr (valid_values[i], ==, (gchar *)value );
    ++i;
  }
}

static void
atk_test_accessible_get_attributes_as_array (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  gchar *valid_attr[] = { "atspi:test", NULL };
  GArray *attr_arr = atspi_accessible_get_attributes_as_array ( obj, NULL);
  int i = 0;
  g_assert (attr_arr->len == (sizeof(valid_attr)/sizeof(gchar *))-1);
  for( i = 0; i < attr_arr->len; ++i) {
    g_assert_cmpstr (valid_attr[i], ==, g_array_index (attr_arr, gchar *, i));
  }
}

static void
atk_test_accessible_get_toolkit_name (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  gchar *toolkit_name = atspi_accessible_get_toolkit_name (obj, NULL);

  g_assert_cmpstr (toolkit_name, ==, "atspitesting-toolkit");
}

static void
atk_test_accessible_get_toolkit_version (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  gchar *toolkit_ver = atspi_accessible_get_toolkit_version (obj, NULL);
  /* should be empty string, because no value is setted */
  g_assert_cmpstr (toolkit_ver, ==, "");
}

static void
atk_test_accessible_get_atspi_version (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  gchar *atspi_version = atspi_accessible_get_atspi_version (obj, NULL);
  g_assert_cmpstr (atspi_version, ==, "2.1");
}

static void
atk_test_accessible_get_id (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  gint app_id = atspi_accessible_get_id (obj, NULL);
  g_assert_cmpint (app_id, !=, -1);
}

static void
atk_test_accessible_get_application (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAccessible *app = atspi_accessible_get_application (obj, NULL);
  g_assert (app != NULL);
}

static void
atk_test_accessible_get_action_iface (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiAction *iface = atspi_accessible_get_action_iface (obj);
  g_assert (iface == NULL);
}

static void
atk_test_accessible_get_collection_iface (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiCollection *iface = atspi_accessible_get_collection_iface (obj);
  g_assert (iface != NULL);
}

static void
atk_test_accessible_get_component_iface (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiComponent *iface = atspi_accessible_get_component_iface (obj);
  g_assert (iface == NULL);
}

static void
atk_test_accessible_get_document_iface (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiDocument *iface = atspi_accessible_get_document_iface (obj);
  g_assert (iface == NULL);
}

static void
atk_test_accessible_get_editable_text_iface (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiEditableText *iface = atspi_accessible_get_editable_text_iface (obj);
  g_assert (iface == NULL);
}

static void
atk_test_accessible_get_hypertext_iface (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiHypertext *iface = atspi_accessible_get_hypertext_iface (obj);
  g_assert (iface == NULL);
}

static void
atk_test_accessible_get_image_iface (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiImage *iface = atspi_accessible_get_image_iface (obj);
  g_assert (iface == NULL);
}

static void
atk_test_accessible_get_selection_iface (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiSelection *iface = atspi_accessible_get_selection_iface (obj);
  g_assert (iface == NULL);
}

static void
atk_test_accessible_get_table_iface (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiTable *iface = atspi_accessible_get_table_iface (obj);
  g_assert (iface == NULL);
}

static void
atk_test_accessible_get_text_iface (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiText *iface = atspi_accessible_get_text_iface (obj);
  g_assert (iface == NULL);
}

static void
atk_test_accessible_get_value_iface (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  AtspiValue *iface = atspi_accessible_get_value_iface (obj);
  g_assert (iface == NULL);
}

static void
atk_test_accessible_get_interfaces (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  gchar *valid_obj_ifaces[] = { "Accessible", "Collection" };

  GArray *ifaces = atspi_accessible_get_interfaces (obj);
  g_assert (ifaces->len == 2);
  int i = 0;
  for (i = 0; i < ifaces->len; ++i)
    g_assert_cmpstr (valid_obj_ifaces[i], ==, g_array_index (ifaces, gchar *, i));
}

static void
atk_test_accessible_get_object_locale (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  const gchar *obj_locale = atspi_accessible_get_object_locale (obj, NULL);
  g_assert_cmpstr (obj_locale, ==, setlocale (LC_MESSAGES, NULL));
}

static void
atk_test_accessible_set_cache_mask (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
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
  for (i=0; i<array->len; i++)
    atk_test_check_cache_cleared (g_ptr_array_index (array, i));
}

static void
atk_test_accessible_clear_cache (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  atspi_accessible_clear_cache (obj);
  atk_test_check_cache_cleared (obj);
}

static void
atk_test_accessible_get_process_id (gpointer fixture, gconstpointer user_data)
{
  AtspiAccessible *obj = get_root_obj (DATA_FILE);
  guint proc_id = atspi_accessible_get_process_id (obj, NULL);
  g_assert_cmpint (proc_id, ==, child_pid);
}

void
atk_test_accessible (void)
{
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_name",
                     0, NULL, NULL, atk_test_accessible_get_name, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_description",
                     0, NULL, NULL, atk_test_accessible_get_description, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_child_count",
                     0, NULL, NULL, atk_test_accessible_get_child_count, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_parent",
                     0, NULL, NULL, atk_test_accessible_get_parent, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_child_at_index",
                     0, NULL, NULL, atk_test_accessible_get_child_at_index, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_index_in_parent",
                     0, NULL, NULL, atk_test_accessible_get_index_in_parent, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_relation_set_1",
                     0, NULL, NULL, atk_test_accessible_get_relation_set_1, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_relation_set_2",
                     0, NULL, NULL, atk_test_accessible_get_relation_set_2, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_role",
                     0, NULL, NULL, atk_test_accessible_get_role, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_role_name",
                     0, NULL, NULL, atk_test_accessible_get_role_name, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_localized_role_name",
                     0, NULL, NULL, atk_test_accessible_get_localized_role_name, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_state_set",
                     0, NULL, NULL, atk_test_accessible_get_state_set, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_attributes",
                     0, NULL, NULL, atk_test_accessible_get_attributes, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_attributes_as_array",
                     0, NULL, NULL, atk_test_accessible_get_attributes_as_array, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_toolkit_name",
                     0, NULL, NULL, atk_test_accessible_get_toolkit_name, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_toolkit_version",
                     0, NULL, NULL, atk_test_accessible_get_toolkit_version, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_atspi_version",
                     0, NULL, NULL, atk_test_accessible_get_atspi_version, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_id",
                     0, NULL, NULL, atk_test_accessible_get_id, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_application",
                     0, NULL, NULL, atk_test_accessible_get_application, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_action_iface",
                     0, NULL, NULL, atk_test_accessible_get_action_iface, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_collection_iface",
                     0, NULL, NULL, atk_test_accessible_get_collection_iface, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_component_iface",
                     0, NULL, NULL, atk_test_accessible_get_component_iface, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_document_iface",
                     0, NULL, NULL, atk_test_accessible_get_document_iface, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_editable_text_iface",
                     0, NULL, NULL, atk_test_accessible_get_editable_text_iface, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_hypertext_iface",
                     0, NULL, NULL, atk_test_accessible_get_hypertext_iface, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_image_iface",
                     0, NULL, NULL, atk_test_accessible_get_image_iface, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_selection_iface",
                     0, NULL, NULL, atk_test_accessible_get_selection_iface, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_table_iface",
                     0, NULL, NULL, atk_test_accessible_get_table_iface, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_text_iface",
                     0, NULL, NULL, atk_test_accessible_get_text_iface, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_value_iface",
                     0, NULL, NULL, atk_test_accessible_get_value_iface, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_interfaces",
                     0, NULL, NULL, atk_test_accessible_get_interfaces, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_object_locale",
                     0, NULL, NULL, atk_test_accessible_get_object_locale, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_set_cache_mask",
                     0, NULL, NULL, atk_test_accessible_set_cache_mask, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_clear_cache",
                     0, NULL, NULL, atk_test_accessible_clear_cache, teardown_accessible_test);
  g_test_add_vtable (ATK_TEST_PATH_ACCESSIBLE "/atk_test_accessible_get_process_id",
                     0, NULL, NULL, atk_test_accessible_get_process_id, teardown_accessible_test);
}
