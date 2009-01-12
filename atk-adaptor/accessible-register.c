/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
 * Copyright 2008, 2009 Codethink Ltd.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bridge.h"
#include "accessible-register.h"

/* TODO
 * Need to add concurrency support.
 */

#define ATK_BRIDGE_OBJECT_PATH_PREFIX "/org/freedesktop/atspi/accessible"
#define ATK_BRIDGE_OBJECT_REFERENCE_TEMPLATE ATK_BRIDGE_OBJECT_PATH_PREFIX "/%d"
#define ATK_BRIDGE_PATH_PREFIX_LENGTH 33

/*
 * This module is responsible for keeping track of all the AtkObjects in
 * the application, so that they can be accessed remotely and placed in
 * a client side cache.
 *
 * To access an AtkObject remotely we need to provide a D-Bus object 
 * path for it. The D-Bus object paths used have a standard prefix
 * (ATK_BRIDGE_OBJECT_PATH_PREFIX). Appended to this prefix is a string
 * representation of an integer reference. So to access an AtkObject 
 * remotely we keep a Hashtable that maps the given reference to 
 * the AtkObject pointer. An object in this hash table is said to be 'registered'.
 *
 * The architecture of AT-SPI dbus is such that AtkObjects are not
 * remotely reference counted. This means that we need to keep track of
 * object destruction. When an object is destroyed it must be 'deregistered'
 * To do this lookup we keep a dbus-id attribute on each AtkObject.
 *
 * In addition to accessing the objects remotely we must be able to update
 * the client side cache. This is done using the 'update' signal of the 
 * org.freedesktop.atspi.Tree interface. The update signal should send out
 * all of the cacheable data for each AtkObject that has changed since the
 * last update. It should also provide a list of objects that have been
 * removed since the last update.
 */

GHashTable *ref2ptr = NULL; /* Used for converting a D-Bus path (Reference) to the object pointer */

static guint counter = 1;

/*---------------------------------------------------------------------------*/

/*
 * Each AtkObject must be asssigned a D-Bus path (Reference)
 *
 * This function provides an integer reference for a new
 * AtkObject.
 */
static guint
assign_reference(void)
{
  counter++;
  /* Reference of 0 not allowed as used as direct key in hash table */
  if (counter == 0)
    counter++;
}

/*---------------------------------------------------------------------------*/

void
atk_dbus_foreach_registered(GHFunc func, gpointer data)
{
  g_hash_table_foreach(ref2ptr, func, data);
}

/*---------------------------------------------------------------------------*/

/*
 * Called when a registered AtkObject is deleted.
 * Removes the AtkObject from the reference lookup tables.
 * Sets the client side cache to be updated.
 */
static void
deregister_accessible(gpointer data, GObject *accessible)
{
  guint ref;
  gchar *path;

  g_assert(ATK_IS_OBJECT(accessible));


  ref = atk_dbus_object_to_ref (ATK_OBJECT(accessible));

  if (ref != 0)
    {
      g_hash_table_remove(ref2ptr, GINT_TO_POINTER(ref));
      /*
       * TODO
       * Pyatspi client side exceptions have occured indicating
       * that an object has been removed twice.
       * This should not be possible and needs investigation.
       */
      spi_emit_cache_removal (ref, atk_adaptor_app_data->bus);
    }
}

/*---------------------------------------------------------------------------*/

/*
 * This function registers the object so that it is exported
 * over D-Bus and schedules an update to client side cache.
 */
static guint
export (GList **uplist, AtkObject *accessible)
{
  guint ref;
  gchar *path;

  g_assert(ATK_IS_OBJECT(accessible));

  ref = assign_reference();

  g_hash_table_insert (ref2ptr, GINT_TO_POINTER(ref), accessible);
  g_object_set_data (G_OBJECT(accessible), "dbus-id", GINT_TO_POINTER(ref));
  g_object_weak_ref(G_OBJECT(accessible), deregister_accessible, NULL);

  *uplist = g_list_prepend (*uplist, accessible);

  return ref;
}

/*
 * This does a depth first traversal of a subtree of AtkObject
 * and exports them as Accessible objects if they are not exported
 * already.
 */
static guint
export_subtree (AtkObject *accessible)
{
  AtkObject *current, *tmp;
  GQueue    *stack;
  GList     *uplist = NULL;
  guint      i, ref;
  gboolean   recurse;

  stack = g_queue_new ();

  current = g_object_ref (accessible);
  ref = export (&uplist, current);
  g_queue_push_head (stack, GINT_TO_POINTER (0));

  /*
   * The index held on the stack is the next child node
   * that needs processing at the corresponding level in the tree.
   */
  while (!g_queue_is_empty (stack))
    {
      /* This while loop finds the next node that needs processing,
       * if one exists.
       */
      i = GPOINTER_TO_INT(g_queue_peek_head (stack));
      recurse = FALSE;
      while (i < atk_object_get_n_accessible_children (current) &&
             recurse == FALSE)
        {
          tmp = atk_object_ref_accessible_child (current, i);
          if (!atk_dbus_object_to_ref (tmp))
            {
              recurse = TRUE;
            }
          else
            {
              i++;
              g_object_unref (G_OBJECT (tmp));
            }
        }
      if (recurse)
        {
          /* Still children to process */
          current = tmp;
          export (&uplist, current);
          /* Update parent nodes next child index */
          g_queue_peek_head_link (stack)->data = GINT_TO_POINTER (i+1);
          /* Push a new child index for the current node */
          g_queue_push_head (stack, GINT_TO_POINTER (0));
        }
      else
        {
          /* No more children, move to parent */
          tmp = current;
          current = atk_object_get_parent (current);
          g_object_unref (G_OBJECT (tmp));
          g_queue_pop_head (stack);
        }
    }
  spi_emit_cache_update (uplist, atk_adaptor_app_data->bus);
  g_list_free (uplist);
  return ref;
}

/*---------------------------------------------------------------------------*/

/* Called to register an AtkObject with AT-SPI and expose it over D-Bus. */
guint
atk_dbus_register_accessible (AtkObject *accessible)
{
  guint ref;
  g_assert(ATK_IS_OBJECT(accessible));

  ref = atk_dbus_object_to_ref (accessible);
  if (!ref)
      return export_subtree (accessible);
  else
      return ref;
}

/* Called when an already registered object is updated in such a
 * way that client side cache needs to be updated.
 */
guint
atk_dbus_update_accessible (AtkObject *accessible)
{
  guint ref = 0;
  g_assert(ATK_IS_OBJECT(accessible));

  ref = atk_dbus_object_to_ref (accessible);
  if (ref)
    {
      spi_emit_cache_update (accessible, atk_adaptor_app_data->bus);
    }
  return ref;
}

/*---------------------------------------------------------------------------*/

/*
 * Returns the reference of the object, or 0 if it is not exported over D-Bus.
 */
guint
atk_dbus_object_to_ref (AtkObject *accessible)
{
  return GPOINTER_TO_INT(g_object_get_data (G_OBJECT (accessible), "dbus-id"));
}

/*
 * Converts the Accessible object reference to its D-Bus object path
 */
gchar *
atk_dbus_ref_to_path (guint ref)
{
  return g_strdup_printf(ATK_BRIDGE_OBJECT_REFERENCE_TEMPLATE, ref);
}

/*
 * Used to lookup an AtkObject from its D-Bus path.
 */
AtkObject *
atk_dbus_path_to_object (const char *path)
{
  guint index;
  void *data;

  g_assert (path);

  if (strncmp(path, ATK_BRIDGE_OBJECT_PATH_PREFIX, ATK_BRIDGE_PATH_PREFIX_LENGTH) != 0) 
    return NULL;

  path += ATK_BRIDGE_PATH_PREFIX_LENGTH; /* Skip over the prefix */

  if (path[0] == '\0')
     return atk_get_root();
  if (path[0] != '/')
     return NULL;

  path++;
  index = atoi (path);
  data = g_hash_table_lookup (ref2ptr, GINT_TO_POINTER(index));
  if (data)
    return ATK_OBJECT (data);
  else
    return NULL;
}


/*
 * Used to lookup a D-Bus path from the AtkObject.
 */
gchar *
atk_dbus_object_to_path (AtkObject *accessible)
{
  guint ref;
  g_assert(ATK_IS_OBJECT(accessible));

  ref = atk_dbus_object_to_ref (accessible);
  if (!ref)
      return NULL;
  else
      return atk_dbus_ref_to_path (ref);
}

/*---------------------------------------------------------------------------*/

/*
 * Initializes required global data. The update and removal lists
 * and the reference lookup tables.
 *
 * Initializes all of the required D-Bus interfaces.
 */
void
atk_dbus_initialize (AtkObject *root)
{
  if (!ref2ptr)
    ref2ptr = g_hash_table_new(g_direct_hash, g_direct_equal);

  /* Get the root accessible and add */
  atk_dbus_register_accessible (root);
}

/*END------------------------------------------------------------------------*/

