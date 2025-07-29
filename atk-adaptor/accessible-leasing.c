/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2009, 2010 Codethink Ltd.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "accessible-leasing.h"
#include "event.h"

#ifdef SPI_ATK_DEBUG
#include "accessible-cache.h"
#endif

/*---------------------------------------------------------------------------*/

SpiLeasing *spi_global_leasing;

typedef struct _ExpiryElement
{
  gint64 expiry_s;
  GObject *object;
} ExpiryElement;

static void spi_leasing_dispose (GObject *object);

static void spi_leasing_finalize (GObject *object);

static void add_expiry_timeout (SpiLeasing *leasing);

/*---------------------------------------------------------------------------*/

G_DEFINE_TYPE (SpiLeasing, spi_leasing, G_TYPE_OBJECT)

static void
spi_leasing_class_init (SpiLeasingClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;

  spi_leasing_parent_class = g_type_class_ref (G_TYPE_OBJECT);

  object_class->finalize = spi_leasing_finalize;
  object_class->dispose = spi_leasing_dispose;
}

static void
spi_leasing_init (SpiLeasing *leasing)
{
  leasing->expiry_queue = g_queue_new ();
  leasing->expiry_queue_hash = g_hash_table_new (g_direct_hash, g_direct_equal);
  leasing->expiry_func_id = 0;
}

static void
spi_leasing_finalize (GObject *object)
{
  SpiLeasing *leasing = SPI_LEASING (object);

  if (leasing->expiry_func_id)
    g_source_remove (leasing->expiry_func_id);
  g_hash_table_unref (leasing->expiry_queue_hash);
  g_queue_free (leasing->expiry_queue);
  G_OBJECT_CLASS (spi_leasing_parent_class)->finalize (object);
}

static void
spi_leasing_dispose (GObject *object)
{
  SpiLeasing *leasing = SPI_LEASING (object);

  ExpiryElement *head;
  while ((head = g_queue_pop_head (leasing->expiry_queue)))
    {
      g_object_unref (head->object);
      g_slice_free (ExpiryElement, head);
      g_hash_table_remove (leasing->expiry_queue_hash, object);
    }
  G_OBJECT_CLASS (spi_leasing_parent_class)->dispose (object);
}

/*---------------------------------------------------------------------------*/

/*
  End the lease on all objects whose expiry time has passed.

  Check when the next event is and set the next expiry func.
*/
static gboolean
expiry_func (gpointer data)
{
  SpiLeasing *leasing = SPI_LEASING (data);

  ExpiryElement *head, *current;
  gint64 secs = g_get_monotonic_time () / 1000000;

  head = g_queue_peek_head (leasing->expiry_queue);
  while (head != NULL && head->expiry_s <= secs)
    {
      current = g_queue_pop_head (leasing->expiry_queue);

#ifdef SPI_ATK_DEBUG
      g_debug ("REVOKE - ");
      spi_cache_print_info (current->object);
#endif

      g_hash_table_remove (leasing->expiry_queue_hash, current->object);
      g_object_unref (current->object);
      g_slice_free (ExpiryElement, current);

      head = g_queue_peek_head (leasing->expiry_queue);
    }

  leasing->expiry_func_id = 0;
  add_expiry_timeout (leasing);

  return FALSE;
}

/*---------------------------------------------------------------------------*/

/*
  Checks if an expiry timeout is already scheduled, if so returns.
  This is becasue events will always be added to the end of the queue.
  Events are always later in time to previoulsy added events.

  Otherwise calculate the next wake time using the top of the queue
  and add the next expiry function.

  This function is called when a lease is added or at the end of the
  expiry function to add the next expiry timeout.
*/
static void
add_expiry_timeout (SpiLeasing *leasing)
{
  ExpiryElement *elem;
  gint64 secs = g_get_monotonic_time () / 1000000;
  guint next_expiry;

  if (leasing->expiry_func_id != 0)
    return;

  elem = (ExpiryElement *) g_queue_peek_head (leasing->expiry_queue);
  if (elem == NULL)
    return;

  next_expiry = elem->expiry_s - secs;
  leasing->expiry_func_id = spi_timeout_add_seconds (next_expiry,
                                                     expiry_func, leasing);
}

/*---------------------------------------------------------------------------*/

/*
  The lease time is expected to be in seconds, the rounding is going to be to
  intervals of 1 second.

  The lease time is going to be rounded up, as the lease time should be
  considered a MINIMUM that the object will be leased for.
*/
#define LEASE_TIME_S 15
#define EXPIRY_TIME_S (LEASE_TIME_S + 1)

GObject *
spi_leasing_take (SpiLeasing *leasing, GObject *object)
{
  GList *old_entry;
  ExpiryElement *elem;

  /*
     Get the current time.
     Quantize the time.
     Remove any earlier lease for the given object.
     Add the release event to the queue.
     Check the next expiry.
   */

  gint64 secs = g_get_monotonic_time () / 1000000;
  guint expiry_s;

  expiry_s = secs + EXPIRY_TIME_S;

  old_entry = g_hash_table_lookup (leasing->expiry_queue_hash, object);
  if (old_entry)
    {
      elem = old_entry->data;
      g_queue_delete_link (leasing->expiry_queue, old_entry);
      g_hash_table_remove (leasing->expiry_queue_hash, object);
    }
  else
    {
      elem = g_slice_new (ExpiryElement);
      elem->object = g_object_ref (object);
    }

  elem->expiry_s = expiry_s;

  g_queue_push_tail (leasing->expiry_queue, elem);
  g_hash_table_insert (leasing->expiry_queue_hash, object, leasing->expiry_queue->tail);

  add_expiry_timeout (leasing);

#ifdef SPI_ATK_DEBUG
  g_debug ("LEASE - ");
  spi_cache_print_info (object);
#endif

  return object;
}

/*END------------------------------------------------------------------------*/
