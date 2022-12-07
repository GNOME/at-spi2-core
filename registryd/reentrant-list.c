/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#include <config.h>
#include <glib.h>

#include "reentrant-list.h"

typedef struct
{
  GList **list;
  GList *iterator;
} Iteration;

static GSList *working_list = NULL; /* of Iteration */

/*
 *   deletes an element from the list - in a re-entrant
 * safe fashion; advances the element pointer to the next
 * element.
 */
void
spi_re_entrant_list_delete_link (GList *const *element_ptr)
{
  GSList *l;
  GList *next;
  GList *element;
  gboolean first_item;
  GList *dummy G_GNUC_UNUSED;

  g_return_if_fail (element_ptr != NULL);

  element = *element_ptr;
  g_return_if_fail (element != NULL);

  next = element->next;
  first_item = (element->prev == NULL);

  dummy = g_list_remove_link (NULL, element);

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
spi_re_entrant_list_foreach (GList **list,
                             SpiReEntrantFn func,
                             gpointer user_data)
{
  Iteration i;

  if (!list || !*list)
    {
      return;
    }

  i.list = list;
  i.iterator = *list;

  working_list = g_slist_prepend (working_list, &i);

  while (i.iterator)
    {
      GList *l = i.iterator;

      func (&i.iterator, user_data);

      if (i.iterator == l)
        i.iterator = i.iterator->next;
    }

  working_list = g_slist_remove (working_list, &i);
}
