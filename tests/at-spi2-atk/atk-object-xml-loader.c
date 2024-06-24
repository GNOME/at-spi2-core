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

#include <glib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdlib.h>
#include <string.h>

#include "my-atk.h"

#define ACCESSIBLE_NODE ((const xmlChar *) "accessible")
#define ACC_ACTION_NODE ((const xmlChar *) "accessible_action")
#define ACC_COMPONENT_NODE ((const xmlChar *) "accessible_component")
#define ACC_DOCUMENT_NODE ((const xmlChar *) "accessible_document")
#define ACC_HYPERLINK_NODE ((const xmlChar *) "accessible_hyperlink")
#define ACC_HYPERTEXT_NODE ((const xmlChar *) "accessible_hypertext")
#define ACC_IMAGE_NODE ((const xmlChar *) "accessible_image")
#define ACC_TABLE_NODE ((const xmlChar *) "accessible_table")
#define ACC_TABLE_CELL_NODE ((const xmlChar *) "accessible_table_cell")
#define ACC_EDIT_TEXT_NODE ((const xmlChar *) "accessible_editable_text")
#define ACC_TEXT_NODE ((const xmlChar *) "accessible_text")
#define ACC_SELECTION_NODE ((const xmlChar *) "accessible_selection")
#define ACC_VALUE_NODE ((const xmlChar *) "accessible_value")
#define ACTION_NODE ((const xmlChar *) "action")
#define DOCUMENT_NODE ((const xmlChar *) "document")
#define INTERFACE_NODE ((const xmlChar *) "interface")
#define RELATION_NODE ((const xmlChar *) "relation")
#define STATE_NODE ((const xmlChar *) "state")
#define COMPONENT_NODE ((const xmlChar *) "component")
#define HYPERLINK_NODE ((const xmlChar *) "hyperlink")
#define HYPERTEXT_NODE ((const xmlChar *) "hypertext")
#define IMAGE_NODE ((const xmlChar *) "image")
#define TABLE_NODE ((const xmlChar *) "table")
#define TABLE_CELL_NODE ((const xmlChar *) "table_cell")
#define TEXT_NODE ((const xmlChar *) "text_node")
#define VALUE_NODE ((const xmlChar *) "value_node")
#define SELECT_NODE ((const xmlChar *) "select_node")

#define NAME_ATTR ((const xmlChar *) "name")
#define DESC_ATTR ((const xmlChar *) "description")
#define ROLE_ATTR ((const xmlChar *) "role")
#define HELP_TEXT_ATTR ((const xmlChar *) "help_text")
#define MIN_ATTR ((const xmlChar *) "min")
#define MAX_ATTR ((const xmlChar *) "max")
#define CURRENT_ATTR ((const xmlChar *) "current")
#define STEP_ATTR ((const xmlChar *) "step")
#define COL_HEADER_ATTR ((const xmlChar *) "col_header")
#define ROW_HEADER_ATTR ((const xmlChar *) "row_header")
#define COL_DESC_ATTR ((const xmlChar *) "col_desc")
#define ROW_DESC_ATTR ((const xmlChar *) "row_desc")
#define SELECTED_ATTR ((const xmlChar *) "selected")
#define SELECTED_COL_ATTR ((const xmlChar *) "selected_col")
#define RELATION_TYPE_ATTR ((const xmlChar *) "relation_type")
#define RELATION_TARGET_NAME_ATTR ((const xmlChar *) "target_name")
#define STATE_TYPE_ATTR ((const xmlChar *) "state_enum")
#define ACTION_NAME_ATTR ((const xmlChar *) "action_name")
#define ACTION_DES_ATTR ((const xmlChar *) "action_description")
#define ACTION_KEY_BIND_ATTR ((const xmlChar *) "key_binding")
#define COMP_X_ATTR ((const xmlChar *) "x")
#define COMP_Y_ATTR ((const xmlChar *) "y")
#define COMP_WIDTH_ATTR ((const xmlChar *) "width")
#define COMP_HEIGHT_ATTR ((const xmlChar *) "height")
#define COMP_LAYER_ATTR ((const xmlChar *) "layer")
#define COMP_ZORDER_ATTR ((const xmlChar *) "zorder")
#define COMP_ALPHA_ATTR ((const xmlChar *) "alpha")
#define IMAGE_DES_ATTR ((const xmlChar *) "image_description")
#define IMAGE_LOCALE_ATTR ((const xmlChar *) "image_locale")
#define TEXT_TEXT_ATTR ((const xmlChar *) "text")
#define TEXT_BOLD_ATTR ((const xmlChar *) "bold_text")
#define TEXT_UNDERLINE_ATTR ((const xmlChar *) "underline_text")
#define TEXT_DUMMY_ATTR ((const xmlChar *) "dummy_text")
#define START_ATTR ((const xmlChar *) "start")
#define END_ATTR ((const xmlChar *) "end")
#define LINK_ATTR ((const xmlChar *) "link")
#define CELL_X_ATTR ((const xmlChar *) "cell_x")
#define CELL_Y_ATTR ((const xmlChar *) "cell_y")
#define ROW_SPAN_ATTR ((const xmlChar *) "row_span")
#define COLUMN_SPAN_ATTR ((const xmlChar *) "column_span")
#define SELECT_ATTR ((const xmlChar *) "selected")
#define PAGE_ATTR ((const xmlChar *) "page_no")
#define PAGE_NUM_ATTR ((const xmlChar *) "page_number")

MyAtkObject *relation_target = NULL;

static double
atof_get_prop (xmlNode *node, const xmlChar *attr)
{
  double ret;
  xmlChar *str = xmlGetProp (node, attr);
  if (!str)
    return 0;
  ret = atof ((const char *) str);
  xmlFree (str);

  return ret;
}

static int
atoi_get_prop (xmlNode *node, const xmlChar *attr)
{
  int ret;
  xmlChar *str = xmlGetProp (node, attr);
  if (!str)
    return 0;
  ret = atoi ((const char *) str);
  xmlFree (str);

  return ret;
}

static AtkAttribute *
get_atk_attribute (xmlNode *node, const xmlChar *attr)
{
  xmlChar *str;
  AtkAttribute *tmp = g_malloc (sizeof (AtkAttribute));

  if (!tmp)
    return NULL;

  str = xmlGetProp (node, attr);
  tmp->name = g_strdup ((const char *) attr);
  tmp->value = g_strdup ((const char *) str);

  xmlFree (str);
  return tmp;
}

static gpointer
create_atk_object_from_element (xmlNode *element)
{
  xmlNode *child_node;
  xmlNode *child_node2;

  gpointer obj;
  gpointer child_obj = NULL;
  AtkRelationSet *relation_set = NULL;
  AtkObject *array[1];
  AtkRelation *relation;
  AtkStateSet *state_set = NULL;
  AtkStateType state_type;

  xmlChar *name;
  xmlChar *description;
  xmlChar *role;
  xmlChar *help_text;
  gint relation_type;
  gint x_size, y_size;
  gint width, height;
  gint x_extent, y_extent, w_extent, h_extent;
  name = xmlGetProp (element, NAME_ATTR);
  description = xmlGetProp (element, DESC_ATTR);
  role = xmlGetProp (element, ROLE_ATTR);
  help_text = xmlGetProp (element, HELP_TEXT_ATTR);
  GType type = MY_TYPE_ATK_OBJECT;
  gint layer;
  gint zorder;
  gdouble alpha;

  if (!xmlStrcmp (element->name, ACCESSIBLE_NODE))
    type = MY_TYPE_ATK_OBJECT;

  if (!xmlStrcmp (element->name, ACC_ACTION_NODE))
    type = MY_TYPE_ATK_ACTION;

  if (!xmlStrcmp (element->name, ACC_COMPONENT_NODE))
    type = MY_TYPE_ATK_COMPONENT;

  if (!xmlStrcmp (element->name, ACC_DOCUMENT_NODE))
    type = MY_TYPE_ATK_DOCUMENT;

  if (!xmlStrcmp (element->name, ACC_EDIT_TEXT_NODE))
    type = MY_TYPE_ATK_EDITABLE_TEXT;

  if (!xmlStrcmp (element->name, ACC_HYPERLINK_NODE))
    type = MY_TYPE_ATK_HYPERTEXT;

  if (!xmlStrcmp (element->name, ACC_HYPERTEXT_NODE))
    type = MY_TYPE_ATK_HYPERTEXT;

  if (!xmlStrcmp (element->name, ACC_IMAGE_NODE))
    type = MY_TYPE_ATK_IMAGE;

  if (!xmlStrcmp (element->name, ACC_SELECTION_NODE))
    type = MY_TYPE_ATK_SELECTION;

  if (!xmlStrcmp (element->name, ACC_TEXT_NODE))
    type = MY_TYPE_ATK_TEXT;

  if (!xmlStrcmp (element->name, ACC_TABLE_NODE))
    type = MY_TYPE_ATK_TABLE;

  if (!xmlStrcmp (element->name, ACC_TABLE_CELL_NODE))
    type = MY_TYPE_ATK_TABLE_CELL;

  if (!xmlStrcmp (element->name, ACC_VALUE_NODE))
    type = MY_TYPE_ATK_VALUE;

  obj = g_object_new (type,
                      "accessible-name", name,
                      "accessible-description", description,
                      "accessible-role", atk_role_for_name ((const gchar *) role),
                      "accessible-help-text", (help_text ? (const char *)help_text : ""),
                      NULL);
  xmlFree (name);
  xmlFree (description);
  xmlFree (role);

  name = NULL;
  description = NULL;
  role = NULL;

  child_node = element->xmlChildrenNode;
  while (child_node != NULL)
    {
      if (!xmlStrcmp (child_node->name, ACCESSIBLE_NODE) ||
          !xmlStrcmp (child_node->name, ACC_ACTION_NODE) ||
          !xmlStrcmp (child_node->name, ACC_COMPONENT_NODE) ||
          !xmlStrcmp (child_node->name, ACC_DOCUMENT_NODE) ||
          !xmlStrcmp (child_node->name, ACC_EDIT_TEXT_NODE) ||
          !xmlStrcmp (child_node->name, ACC_HYPERLINK_NODE) ||
          !xmlStrcmp (child_node->name, ACC_HYPERTEXT_NODE) ||
          !xmlStrcmp (child_node->name, ACC_IMAGE_NODE) ||
          !xmlStrcmp (child_node->name, ACC_SELECTION_NODE) ||
          !xmlStrcmp (child_node->name, ACC_TABLE_NODE) ||
          !xmlStrcmp (child_node->name, ACC_TABLE_CELL_NODE) ||
          !xmlStrcmp (child_node->name, ACC_TEXT_NODE) ||
          !xmlStrcmp (child_node->name, ACC_VALUE_NODE))
        {
          child_obj = create_atk_object_from_element (child_node);
          my_atk_object_add_child (obj, child_obj);
        }
      child_node2 = child_node->xmlChildrenNode;
      while (child_node2 != NULL)
        {
          if (!xmlStrcmp (child_node2->name, RELATION_NODE))
            {
              xmlChar *relation_target_name = xmlGetProp (child_node2, RELATION_TARGET_NAME_ATTR);
              relation_type = atoi_get_prop (child_node2, RELATION_TYPE_ATTR);
              relation_set = atk_object_ref_relation_set (ATK_OBJECT (child_obj));
              array[0] = ATK_OBJECT (obj);
              relation = atk_relation_new (array, 1, relation_type);
              atk_relation_new (array, 1, relation_type);
              atk_relation_set_add (relation_set, relation);
              g_object_unref (relation);
              g_object_unref (relation_set);
              xmlFree (relation_target_name);
            }
          if (!xmlStrcmp (child_node2->name, STATE_NODE))
            {
              xmlChar *state_enum = xmlGetProp (child_node2, STATE_TYPE_ATTR);

              state_set = atk_object_ref_state_set (ATK_OBJECT (child_obj));
              state_type = atk_state_type_for_name ((const gchar *) state_enum);
              atk_state_set_add_state (state_set, state_type);
              g_object_unref (state_set);
              xmlFree (state_enum);
            }
          if (!xmlStrcmp (child_node2->name, ACTION_NODE))
            {
              xmlChar *action_name = xmlGetProp (child_node2, ACTION_NAME_ATTR);
              xmlChar *action_des = xmlGetProp (child_node2, ACTION_DES_ATTR);
              xmlChar *action_key_bind = xmlGetProp (child_node2, ACTION_KEY_BIND_ATTR);

              my_atk_action_add_action (child_obj, (const gchar *) action_name,
                                        (const gchar *) action_des,
                                        (const gchar *) action_key_bind);
              xmlFree (action_key_bind);
              xmlFree (action_des);
              xmlFree (action_name);
            }
          if (!xmlStrcmp (child_node2->name, COMPONENT_NODE))
            {
              x_extent = atoi_get_prop (child_node2, COMP_X_ATTR);
              y_extent = atoi_get_prop (child_node2, COMP_Y_ATTR);
              w_extent = atoi_get_prop (child_node2, COMP_WIDTH_ATTR);
              h_extent = atoi_get_prop (child_node2, COMP_HEIGHT_ATTR);
              layer = atoi_get_prop (child_node2, COMP_LAYER_ATTR);
              zorder = atoi_get_prop (child_node2, COMP_ZORDER_ATTR);
              alpha = atof_get_prop (child_node2, COMP_ALPHA_ATTR);
              atk_component_set_extents (ATK_COMPONENT (child_obj),
                                         x_extent,
                                         y_extent,
                                         w_extent,
                                         h_extent,
                                         ATK_XY_SCREEN);
              my_atk_component_set_layer (ATK_COMPONENT (child_obj), layer);
              my_atk_component_set_mdi_zorder (ATK_COMPONENT (child_obj), zorder);
              my_atk_component_set_alpha (ATK_COMPONENT (child_obj), alpha);
            }
          if (!xmlStrcmp (child_node2->name, DOCUMENT_NODE))
            {
              my_atk_set_document (ATK_DOCUMENT (child_obj),
                                   atoi_get_prop (child_node2, PAGE_ATTR),
                                   atoi_get_prop (child_node2, PAGE_NUM_ATTR));
            }
          if (!xmlStrcmp (child_node2->name, HYPERLINK_NODE))
            {
              xmlChar *text = xmlGetProp (child_node2, TEXT_TEXT_ATTR);

              my_atk_set_hypertext (ATK_HYPERTEXT (child_obj), (const gchar *) text);
              xmlFree (text);
            }
          if (!xmlStrcmp (child_node2->name, HYPERTEXT_NODE))
            {
              xmlChar *text = xmlGetProp (child_node2, TEXT_TEXT_ATTR);

              my_atk_set_hypertext (ATK_HYPERTEXT (child_obj), (const gchar *) text);
              xmlFree (text);
            }
          if (!xmlStrcmp (child_node2->name, IMAGE_NODE))
            {
              xmlChar *image_des = xmlGetProp (child_node2, IMAGE_DES_ATTR);
              xmlChar *image_locale = xmlGetProp (child_node2, IMAGE_LOCALE_ATTR);
              x_size = atoi_get_prop (child_node2, COMP_X_ATTR);
              y_size = atoi_get_prop (child_node2, COMP_Y_ATTR);
              width = atoi_get_prop (child_node2, COMP_WIDTH_ATTR);
              height = atoi_get_prop (child_node2, COMP_HEIGHT_ATTR);

              my_atk_set_image (ATK_IMAGE (child_obj),
                                (const gchar *) image_des,
                                x_size,
                                y_size,
                                width,
                                height,
                                (const gchar *) image_locale);
              xmlFree (image_locale);
              xmlFree (image_des);
            }
          if (!xmlStrcmp (child_node2->name, TEXT_NODE))
            {
              xmlChar *text = xmlGetProp (child_node2, TEXT_TEXT_ATTR);
              AtkAttributeSet *attrSet = NULL;
              AtkAttribute *a1 = get_atk_attribute (child_node2, TEXT_BOLD_ATTR);
              AtkAttribute *a2 = get_atk_attribute (child_node2, TEXT_UNDERLINE_ATTR);
              AtkAttribute *a3 = get_atk_attribute (child_node2, TEXT_DUMMY_ATTR);
              attrSet = g_slist_append (NULL, a1);
              attrSet = g_slist_append (attrSet, a2);
              attrSet = g_slist_append (attrSet, a3);
              my_atk_set_text (ATK_TEXT (child_obj),
                               (const gchar *) text,
                               atoi_get_prop (child_node2, COMP_X_ATTR),
                               atoi_get_prop (child_node2, COMP_Y_ATTR),
                               atoi_get_prop (child_node2, COMP_WIDTH_ATTR),
                               atoi_get_prop (child_node2, COMP_HEIGHT_ATTR),
                               attrSet);
              xmlFree (text);
            }
          if (!xmlStrcmp (child_node2->name, TABLE_CELL_NODE))
            {
              my_atk_set_table_cell (ATK_TABLE_CELL (child_obj),
                                     atoi_get_prop (child_node2, CELL_X_ATTR),
                                     atoi_get_prop (child_node2, CELL_Y_ATTR),
                                     atoi_get_prop (child_node2, ROW_SPAN_ATTR),
                                     atoi_get_prop (child_node2, COLUMN_SPAN_ATTR));
            }
          if (!xmlStrcmp (child_node2->name, VALUE_NODE))
            {
              my_atk_set_value (ATK_VALUE (child_obj),
                                atof_get_prop (child_node2, MIN_ATTR),
                                atof_get_prop (child_node2, CURRENT_ATTR),
                                atof_get_prop (child_node2, MAX_ATTR),
                                atof_get_prop (child_node2, STEP_ATTR));
            }
          child_node2 = child_node2->next;
        }
      child_node = child_node->next;
    }
  return obj;
}

/*
 * Reads the XML from filename and uses it
 * to create a tree of MyAtkObjects.
 *
 * returns: The root object of the tree.
 */
MyAtkObject *
atk_object_xml_parse (gchar *filename)
{
  xmlDoc *doc;
  xmlNode *root_element;
  MyAtkObject *new_atk_object = NULL;

  doc = xmlReadFile (filename, NULL, 0);
  g_assert_nonnull (doc);

  root_element = xmlDocGetRootElement (doc);

  if (!root_element)
    return NULL;

  new_atk_object = create_atk_object_from_element (root_element);

  xmlFreeDoc (doc);
  return new_atk_object;
}
