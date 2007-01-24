/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#include <config.h>
#include <glib/gmessages.h>
#include <glib/gslist.h>
#include <Accessibility.h>

#include "spi-private.h"

typedef struct {
	GList **list;
	GList  *iterator;
} Iteration;

static GSList *working_list = NULL; /* of Iteration */

static char *spi_atk_bridge_null_string = "";

Accessibility_Role spi_accessible_role_from_atk_role (AtkRole role);

Accessibility_Role
spi_role_from_atk_role (AtkRole role)
{
    return spi_accessible_role_from_atk_role (role);
}

/*
 *   deletes an element from the list - in a re-entrant
 * safe fashion; advances the element pointer to the next
 * element.
 */
void
spi_re_entrant_list_delete_link (GList * const *element_ptr)
{
  GSList    *l;
  GList     *next;
  GList     *element;
  gboolean   first_item;

  g_return_if_fail (element_ptr != NULL);

  element = *element_ptr;
  g_return_if_fail (element != NULL);

  next = element->next;
  first_item = (element->prev == NULL);

  element = g_list_remove_link (NULL, element);

  for (l = working_list; l; l = l->next)
    {
       Iteration *i = l->data;

       if (i->iterator == element)
         {
           i->iterator = next;
         }

       if (first_item && *(i->list) == element)
         {
           *(i->list) = next;
         }
    }

  g_list_free_1 (element);
}

void
spi_re_entrant_list_foreach (GList         **list,
			     SpiReEntrantFn  func,
			     gpointer        user_data)
{
	Iteration i;

	if (!list || !*list)
	  {
            return;
	  }

	i.list = list;
	i.iterator = *list;

	working_list = g_slist_prepend (working_list, &i);

	while (i.iterator) {
		GList *l = i.iterator;

		func (&i.iterator, user_data);

		if (i.iterator == l)
			i.iterator = i.iterator->next;
	}

	working_list = g_slist_remove (working_list, &i);
}

void 
spi_init_any_nil (CORBA_any *any_details, 
		  Accessibility_Application app, 
		  Accessibility_Role role,
		  CORBA_string name)
{
  Accessibility_EventDetails *details = Accessibility_EventDetails__alloc();

  any_details->_type = TC_Accessibility_EventDetails;
  any_details->_value = details;
  any_details->_release = TRUE;

  details->host_application = app;
  details->source_role = role;
  details->source_name = CORBA_string_dup (name);

  details->any_data._type = TC_null;
  details->any_data._value = NULL;
  details->any_data._release = TRUE;
}

void 
spi_init_any_object (CORBA_any *any_details, Accessibility_Application app, 
		     Accessibility_Role role,
		     CORBA_string name, 
		     CORBA_Object *o)
{
  Accessibility_EventDetails *details = Accessibility_EventDetails__alloc();

  any_details->_type = TC_Accessibility_EventDetails;
  any_details->_value = details;
  any_details->_release = TRUE;

  details->host_application = app;
  details->source_role = role;
  details->source_name = CORBA_string_dup (name);
  
  details->any_data._type = TC_CORBA_Object;
  details->any_data._value = ORBit_copy_value (o, TC_CORBA_Object);
  details->any_data._release = TRUE;
}

void
spi_init_any_string (CORBA_any *any_details, Accessibility_Application app, 
		     Accessibility_Role role,
		     CORBA_string name, 
		     char **string_pointer)
{  
  Accessibility_EventDetails *details = Accessibility_EventDetails__alloc();

  any_details->_type = TC_Accessibility_EventDetails;
  any_details->_value = details;
  any_details->_release = TRUE;

  details->host_application = app;
  details->source_role = role;
  details->source_name = CORBA_string_dup (name);
  
  details->any_data._type = (CORBA_TypeCode) TC_CORBA_string;
  if (string_pointer && *string_pointer)
    details->any_data._value = string_pointer;
  else
    details->any_data._value = &spi_atk_bridge_null_string;
  details->any_data._release = FALSE;
}

void
spi_init_any_rect (CORBA_any *any_details, 
		   Accessibility_Application app, 
		   Accessibility_Role role,
		   CORBA_string name, 
		   AtkRectangle *rect)
{
  Accessibility_EventDetails *details = Accessibility_EventDetails__alloc();
  Accessibility_BoundingBox *box = Accessibility_BoundingBox__alloc ();

  any_details->_type = TC_Accessibility_EventDetails;
  any_details->_value = details;
  any_details->_release = TRUE;

  details->host_application = app;
  details->source_role = role;
  details->source_name = CORBA_string_dup (name);
  
  box->x = rect->x;
  box->y = rect->y;
  box->width = rect->width;
  box->height = rect->height;
  details->any_data._type = TC_Accessibility_BoundingBox;
  details->any_data._value = box;
  details->any_data._release = TRUE;
}
