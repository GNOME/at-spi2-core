#include <glib/gmessages.h>
#include <glib/gslist.h>

#include "spi-private.h"

typedef struct {
	GList **list;
	GList  *iterator;
} Iteration;

static GSList *working_list = NULL; /* of Iteration */

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

  g_list_remove_link (NULL, element);

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

	if (!list)
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
