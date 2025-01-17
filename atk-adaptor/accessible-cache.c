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

#include <atk/atk.h>
#include <string.h>

#include "accessible-cache.h"
#include "accessible-register.h"
#include "bridge.h"
#include "event.h"

SpiCache *spi_global_cache = NULL;

static gboolean
child_added_listener (GSignalInvocationHint *signal_hint,
                      guint n_param_values,
                      const GValue *param_values,
                      gpointer data);

static void
toplevel_added_listener (AtkObject *accessible,
                         guint index,
                         AtkObject *child);

static void
remove_object (GObject *source, GObject *gobj, gpointer data);

static void
add_object (SpiCache *cache, GObject *gobj);

static void
add_subtree (SpiCache *cache, AtkObject *accessible);

static gboolean
add_pending_items (gpointer data);

/*---------------------------------------------------------------------------*/

static void
spi_cache_finalize (GObject *object);

/*---------------------------------------------------------------------------*/

enum
{
  OBJECT_ADDED,
  OBJECT_REMOVED,
  LAST_SIGNAL
};
static guint cache_signals[LAST_SIGNAL] = { 0 };

/*---------------------------------------------------------------------------*/

G_DEFINE_TYPE (SpiCache, spi_cache, G_TYPE_OBJECT)

static void
spi_cache_class_init (SpiCacheClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;

  spi_cache_parent_class = g_type_class_ref (G_TYPE_OBJECT);

  object_class->finalize = spi_cache_finalize;

  cache_signals[OBJECT_ADDED] =
      g_signal_new ("object-added",
                    SPI_CACHE_TYPE,
                    G_SIGNAL_ACTION,
                    0,
                    NULL,
                    NULL,
                    g_cclosure_marshal_VOID__OBJECT,
                    G_TYPE_NONE,
                    1,
                    G_TYPE_OBJECT);

  cache_signals[OBJECT_REMOVED] =
      g_signal_new ("object-removed",
                    SPI_CACHE_TYPE,
                    G_SIGNAL_ACTION,
                    0,
                    NULL,
                    NULL,
                    g_cclosure_marshal_VOID__OBJECT,
                    G_TYPE_NONE,
                    1,
                    G_TYPE_OBJECT);
}

static void
spi_cache_init (SpiCache *cache)
{
  cache->objects = g_hash_table_new (g_direct_hash, g_direct_equal);
  cache->add_traversal = g_queue_new ();

#ifdef SPI_ATK_DEBUG
  if (g_thread_supported ())
    g_message ("AT-SPI: Threads enabled");

  g_debug ("AT-SPI: Initial Atk tree regisration");
#endif

  g_signal_connect (spi_global_register,
                    "object-deregistered",
                    (GCallback) remove_object, cache);

  add_subtree (cache, spi_global_app_data->root);

  cache->child_added_listener = atk_add_global_event_listener (child_added_listener,
                                                               "Gtk:AtkObject:children-changed");

  g_signal_connect (G_OBJECT (spi_global_app_data->root),
                    "children-changed::add",
                    (GCallback) toplevel_added_listener, NULL);
}

static void
cache_weak_ref (gpointer data, GObject *gobj)
{
  SpiCache *cache = SPI_CACHE (data);

  g_hash_table_remove (cache->objects, gobj);
}

static void
spi_cache_remove_weak_ref (gpointer key, gpointer val, gpointer cache)
{
  g_object_weak_unref (val, cache_weak_ref, cache);
}

static void
spi_cache_finalize (GObject *object)
{
  SpiCache *cache = SPI_CACHE (object);

  while (!g_queue_is_empty (cache->add_traversal))
    g_object_unref (G_OBJECT (g_queue_pop_head (cache->add_traversal)));
  g_queue_free (cache->add_traversal);
  g_hash_table_foreach (cache->objects, spi_cache_remove_weak_ref, cache);
  g_hash_table_unref (cache->objects);

  g_signal_handlers_disconnect_by_func (spi_global_register,
                                        (GCallback) remove_object, cache);

  g_signal_handlers_disconnect_by_func (G_OBJECT (spi_global_app_data->root),
                                        (GCallback) toplevel_added_listener, NULL);

  atk_remove_global_event_listener (cache->child_added_listener);

  G_OBJECT_CLASS (spi_cache_parent_class)->finalize (object);
}

/*---------------------------------------------------------------------------*/

static void
remove_object (GObject *source, GObject *gobj, gpointer data)
{
  SpiCache *cache = SPI_CACHE (data);

  if (spi_cache_in (cache, gobj))
    {
#ifdef SPI_ATK_DEBUG
      g_debug ("CACHE REM - %s - %d - %s\n", atk_object_get_name (ATK_OBJECT (gobj)),
               atk_object_get_role (ATK_OBJECT (gobj)),
               spi_register_object_to_path (spi_global_register, gobj));
#endif
      g_signal_emit (cache, cache_signals[OBJECT_REMOVED], 0, gobj);
      g_object_weak_unref (G_OBJECT (gobj), cache_weak_ref, cache);
      g_hash_table_remove (cache->objects, gobj);
    }
  else if (g_queue_remove (cache->add_traversal, gobj))
    {
      g_object_unref (gobj);
    }
}

static void
add_object (SpiCache *cache, GObject *gobj)
{
  g_return_if_fail (G_IS_OBJECT (gobj));

  g_hash_table_insert (cache->objects, gobj, NULL);
  g_object_weak_ref (G_OBJECT (gobj), cache_weak_ref, cache);

#ifdef SPI_ATK_DEBUG
  g_debug ("CACHE ADD - %s - %d - %s\n", atk_object_get_name (ATK_OBJECT (gobj)),
           atk_object_get_role (ATK_OBJECT (gobj)),
           spi_register_object_to_path (spi_global_register, gobj));
#endif

  g_signal_emit (cache, cache_signals[OBJECT_ADDED], 0, gobj);
}

/*---------------------------------------------------------------------------*/

static GRecMutex cache_mutex;

#ifdef SPI_ATK_DEBUG
static GStaticMutex recursion_check_guard = G_STATIC_MUTEX_INIT;
static gboolean recursion_check = FALSE;

static gboolean
recursion_check_and_set ()
{
  gboolean ret;
  g_static_mutex_lock (&recursion_check_guard);
  ret = recursion_check;
  recursion_check = TRUE;
  g_static_mutex_unlock (&recursion_check_guard);
  return ret;
}

static void
recursion_check_unset ()
{
  g_static_mutex_lock (&recursion_check_guard);
  recursion_check = FALSE;
  g_static_mutex_unlock (&recursion_check_guard);
}
#endif /* SPI_ATK_DEBUG */

/*---------------------------------------------------------------------------*/

static void
append_children (AtkObject *accessible, GQueue *traversal)
{
  AtkObject *current;
  guint i;
  gint count = atk_object_get_n_accessible_children (accessible);

  if (count < 0)
    count = 0;
  for (i = 0; i < count; i++)
    {
      current = atk_object_ref_accessible_child (accessible, i);
      if (current)
        {
          g_queue_push_tail (traversal, current);
        }
    }
}

/*
 * Adds a subtree of accessible objects
 * to the cache at the accessible object provided.
 *
 * The leaf nodes do not have their children
 * registered. A node is considered a leaf
 * if it has the state "manages-descendants"
 * or if it has already been registered.
 */
static void
add_subtree (SpiCache *cache, AtkObject *accessible)
{
  g_return_if_fail (ATK_IS_OBJECT (accessible));

  g_object_ref (accessible);
  g_queue_push_tail (cache->add_traversal, accessible);
  add_pending_items (cache);
}

static gboolean
add_pending_items (gpointer data)
{
  SpiCache *cache = SPI_CACHE (data);
  AtkObject *current;
  GQueue *to_add;

  to_add = g_queue_new ();

  while (!g_queue_is_empty (cache->add_traversal))
    {
      AtkStateSet *set;

      /* cache->add_traversal holds a ref to current */
      current = g_queue_pop_head (cache->add_traversal);
      set = atk_object_ref_state_set (current);

      if (set && !atk_state_set_contains_state (set, ATK_STATE_TRANSIENT))
        {
          /* transfer the ref into to_add */
          g_queue_push_tail (to_add, current);
          if (!spi_cache_in (cache, G_OBJECT (current)) &&
              !atk_state_set_contains_state (set, ATK_STATE_MANAGES_DESCENDANTS) &&
              !atk_state_set_contains_state (set, ATK_STATE_DEFUNCT))
            {
              append_children (current, cache->add_traversal);
            }
        }
      else
        {
          /* drop the ref for the removed object */
          g_object_unref (current);
        }

      if (set)
        g_object_unref (set);
    }

  while (!g_queue_is_empty (to_add))
    {
      current = g_queue_pop_head (to_add);

      add_object (cache, G_OBJECT (current));
      g_object_unref (G_OBJECT (current));
    }

  g_queue_free (to_add);
  cache->add_pending_idle = 0;
  return FALSE;
}

/*---------------------------------------------------------------------------*/

static gboolean
child_added_listener (GSignalInvocationHint *signal_hint,
                      guint n_param_values,
                      const GValue *param_values,
                      gpointer data)
{
  SpiCache *cache = spi_global_cache;
  AtkObject *accessible;

  const gchar *detail = NULL;

  /*
   * Ensure that only accessibles already in the cache
   * have their signals processed.
   */
  accessible = ATK_OBJECT (g_value_get_object (&param_values[0]));
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), TRUE);

  g_rec_mutex_lock (&cache_mutex);

  if (spi_cache_in (cache, G_OBJECT (accessible)))
    {
#ifdef SPI_ATK_DEBUG
      if (recursion_check_and_set ())
        g_warning ("AT-SPI: Recursive use of cache module");

      g_debug ("AT-SPI: Tree update listener");
#endif
      if (signal_hint->detail)
        detail = g_quark_to_string (signal_hint->detail);

      if (detail && !strncmp (detail, "add", 3))
        {
          gpointer child;
          child = g_value_get_pointer (param_values + 2);
          if (!child)
            {
              g_rec_mutex_unlock (&cache_mutex);
              return TRUE;
            }

          g_object_ref (child);
          g_queue_push_tail (cache->add_traversal, child);

          if (cache->add_pending_idle == 0)
            cache->add_pending_idle = spi_idle_add (add_pending_items, cache);
        }
#ifdef SPI_ATK_DEBUG
      recursion_check_unset ();
#endif
    }

  g_rec_mutex_unlock (&cache_mutex);

  return TRUE;
}

/*---------------------------------------------------------------------------*/

static void
toplevel_added_listener (AtkObject *accessible,
                         guint index,
                         AtkObject *child)
{
  SpiCache *cache = spi_global_cache;

  g_return_if_fail (ATK_IS_OBJECT (accessible));

  g_rec_mutex_lock (&cache_mutex);

  if (spi_cache_in (cache, G_OBJECT (accessible)))
    {
#ifdef SPI_ATK_DEBUG
      if (recursion_check_and_set ())
        g_warning ("AT-SPI: Recursive use of registration module");

      g_debug ("AT-SPI: Toplevel added listener");
#endif
      if (!ATK_IS_OBJECT (child))
        {
          child = atk_object_ref_accessible_child (accessible, index);
        }
      else
        g_object_ref (child);

      g_queue_push_tail (cache->add_traversal, child);

      if (cache->add_pending_idle == 0)
        cache->add_pending_idle = spi_idle_add (add_pending_items, cache);
#ifdef SPI_ATK_DEBUG
      recursion_check_unset ();
#endif
    }

  g_rec_mutex_unlock (&cache_mutex);
}

/*---------------------------------------------------------------------------*/

void
spi_cache_foreach (SpiCache *cache, GHFunc func, gpointer data)
{
  g_hash_table_foreach (cache->objects, func, data);
}

gboolean
spi_cache_in (SpiCache *cache, GObject *object)
{
  if (!cache)
    return FALSE;

  if (g_hash_table_lookup_extended (cache->objects,
                                    object,
                                    NULL,
                                    NULL))
    return TRUE;
  else
    return FALSE;
}

#ifdef SPI_ATK_DEBUG
void
spi_cache_print_info (GObject *obj)
{
  char *path = spi_register_object_to_path (spi_global_register, obj);

  if (spi_cache_in (spi_global_cache, obj))
    g_printf ("%s IC\n", path);
  else
    g_printf ("%s NC\n", path);

  if (path)
    g_free (path);
}
#endif

/*END------------------------------------------------------------------------*/
