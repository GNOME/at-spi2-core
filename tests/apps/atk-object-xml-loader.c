/*
 * Copyright 2008 Codethink Ltd.
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

#include <my-atk.h>

#define ACCESSIBLE_NODE ((const xmlChar *) "accessible")
#define INTERFACE_NODE  ((const xmlChar *) "interface")

#define NAME_ATTR ((const xmlChar *) "name")
#define DESC_ATTR ((const xmlChar *) "description")
#define ROLE_ATTR ((const xmlChar *) "role")

static MyAtkObject *
create_atk_object_from_element(xmlNode *element)
{
  xmlNode *child_node;

  MyAtkObject *obj = NULL;
  MyAtkObject *child_obj;

  xmlChar *name;
  xmlChar *description;
  xmlChar *role_text; 
  gint role;

  name = xmlGetProp(element, NAME_ATTR);
  description = xmlGetProp(element, DESC_ATTR);
  role_text = xmlGetProp(element, ROLE_ATTR);
  role = atoi(role_text);

  obj = MY_ATK_OBJECT(g_object_new(MY_TYPE_ATK_OBJECT,
				   "accessible-name", name,
				   "accessible-description", description,
				   "accessible-role", role,
				   NULL));

  child_node = element->xmlChildrenNode;
  while (child_node != NULL)
    {
      if (!xmlStrcmp(child_node->name, ACCESSIBLE_NODE))  
	{
	  child_obj = create_atk_object_from_element(child_node); 
	  my_atk_object_add_child(obj, child_obj);
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
atk_object_xml_parse(gchar *filename)
{
  xmlDoc *doc;
  xmlNode *root_element;
  MyAtkObject *new_atk_object = NULL;

  doc = xmlReadFile(filename, NULL, 0);
  g_assert(doc != NULL);

  root_element = xmlDocGetRootElement(doc);

  if (!xmlStrcmp(root_element->name, ACCESSIBLE_NODE))  
    new_atk_object = create_atk_object_from_element(root_element); 

  xmlFreeDoc(doc);
  return new_atk_object;
}
