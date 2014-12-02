/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; https://wiki.gnome.org/Accessibility)
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "my-atk.h"

#define ACCESSIBLE_NODE ((const xmlChar *) "accessible")
#define INTERFACE_NODE  ((const xmlChar *) "interface")
#define RELATION_NODE ((const xmlChar *) "relation")
#define STATE_NODE ((const xmlChar *) "state")

#define NAME_ATTR ((const xmlChar *) "name")
#define DESC_ATTR ((const xmlChar *) "description")
#define ROLE_ATTR ((const xmlChar *) "role")
#define TYPE_ATTR ((const xmlChar *) "type")
#define RELATION_TYPE_ATTR ((const xmlChar *) "relation_type")
#define RELATION_TARGET_NAME_ATTR ((const xmlChar *) "target_name")
#define STATE_TYPE_ATTR ((const xmlChar *) "state_enum")

MyAtkObject *relation_target = NULL;

static MyAtkObject *
create_atk_object_from_element (xmlNode *element)
{
  xmlNode *child_node;
  xmlNode *child_node2;

  MyAtkObject *obj = NULL;
  MyAtkObject *child_obj = NULL;
  AtkRelationSet *relation_set = NULL;
  AtkObject *array[1];
  AtkRelation *relation;
  AtkStateSet *state_set = NULL;
  AtkStateType state_type;

  xmlChar *name;
  xmlChar *description;
  xmlChar *type_text;
  xmlChar *relation_type_text;
  xmlChar *state_enum;
  xmlChar *role;
  gint relation_type;
  xmlChar *relation_target_name;
  GType type = MY_TYPE_ATK_OBJECT;

  name = xmlGetProp (element, NAME_ATTR);
  description = xmlGetProp (element, DESC_ATTR);
  role = xmlGetProp (element, ROLE_ATTR);
  type_text = xmlGetProp (element, TYPE_ATTR);

  obj = MY_ATK_OBJECT (g_object_new (type,
                                     "accessible-name", name,
                                     "accessible-description", description,
                                     "accessible-role", atk_role_for_name (role),
                                     NULL));

  child_node = element->xmlChildrenNode;
  while (child_node != NULL) {
    if (!xmlStrcmp(child_node->name, ACCESSIBLE_NODE)) {
      child_obj = create_atk_object_from_element(child_node);
      my_atk_object_add_child(obj, child_obj);

      child_node2 = child_node->xmlChildrenNode;
      while (child_node2 != NULL) {
        if (!xmlStrcmp (child_node2->name, RELATION_NODE)) {
          relation_type_text = xmlGetProp (child_node2, RELATION_TYPE_ATTR);
          relation_type = atoi (relation_type_text);
          relation_target_name = xmlGetProp (child_node2, RELATION_TARGET_NAME_ATTR);
          relation_set = atk_object_ref_relation_set (ATK_OBJECT(child_obj));
          array[0] = ATK_OBJECT (obj);
          relation = atk_relation_new (array, 1, relation_type);
          atk_relation_new (array, 1, relation_type);
          atk_relation_set_add (relation_set, relation);
          g_object_unref (relation);
          g_object_unref (relation_set);
          xmlFree (relation_target_name);
          xmlFree (relation_type_text);
        }
        if (!xmlStrcmp (child_node2->name, STATE_NODE)) {
          state_set = atk_object_ref_state_set (ATK_OBJECT(child_obj));
          state_enum = xmlGetProp (child_node2, STATE_TYPE_ATTR);
          state_type = atk_state_type_for_name (state_enum);
          atk_state_set_add_state (state_set, state_type);
          g_object_unref (state_set);
          xmlFree (state_enum);
        }
        child_node2 = child_node2->next;
      }
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
  g_assert (doc != NULL);

  root_element = xmlDocGetRootElement (doc);

  if (!xmlStrcmp (root_element->name, ACCESSIBLE_NODE))
    new_atk_object = create_atk_object_from_element (root_element);

  xmlFreeDoc (doc);
  return new_atk_object;
}
