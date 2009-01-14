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
 * all of the cacheable data for an Accessible object.
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

/*
 * Returns the reference of the object, or 0 if it is not registered.
 */
static guint
object_to_ref (AtkObject *accessible)
{
  return GPOINTER_TO_INT(g_object_get_data (G_OBJECT (accessible), "dbus-id"));
}

/*
 * Converts the Accessible object reference to its D-Bus object path
 */
static gchar *
ref_to_path (guint ref)
{
  return g_strdup_printf(ATK_BRIDGE_OBJECT_REFERENCE_TEMPLATE, ref);
}

/*---------------------------------------------------------------------------*/

/*
 * Called when a registered AtkObject is deleted.
 * Removes the AtkObject from the reference lookup tables.
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
    }
}

/*
 * Called to register an AtkObject with AT-SPI and expose it over D-Bus.
 */
static void
register_accessible (AtkObject *accessible)
{
  guint ref;
  g_assert(ATK_IS_OBJECT(accessible));

  ref = assign_reference();

  g_hash_table_insert (ref2ptr, GINT_TO_POINTER(ref), accessible);
  g_object_set_data (G_OBJECT(accessible), "dbus-id", GINT_TO_POINTER(ref));
  g_object_weak_ref(G_OBJECT(accessible), deregister_accessible, NULL);
}

/*---------------------------------------------------------------------------*/

typedef void     (*ActionFunc) (AtkObject *);

/* Return true if action should be performed */
typedef gboolean (*FilterFunc) (AtkObject *);

/*
 * This function performs a depth first traversal of a tree of AtkObjects.
 *
 * It uses a FilterFunc to determine if a node needs to have an action
 * performed on it.
 *
 * Nodes that are filtered out become leaves and no recursion is performed on
 * them.
 *
 * Nodes where an action was performed are returned in a GList.
 */
void
traverse_atk_tree (AtkObject *accessible,
                   GList **visited,
                   ActionFunc action,
                   FilterFunc filter)
{
  AtkObject *current, *tmp;
  GQueue    *stack;
  GList     *uplist = NULL;
  guint      i, ref;
  gboolean   recurse;

  if (!filter (accessible))
      return;

  stack = g_queue_new ();
  current = g_object_ref (accessible);
  action (current)
  *visited = g_list_prepend (*visited, current);
  g_queue_push_head (stack, GINT_TO_POINTER (0));

  /*
   * The index held on the stack is the next child node
   * that needs processing at the corresponding level in the tree.
   */
  while (!g_queue_is_empty (stack))
    {
      /* Find the next child node that needs processing */
      i = GPOINTER_TO_INT(g_queue_peek_head (stack));
      recurse = FALSE;
      while (i < atk_object_get_n_accessible_children (current) &&
             recurse == FALSE)
        {
          tmp = atk_object_ref_accessible_child (current, i);
          /* If filter function */
          if (filter (tmp))
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
          /* Push onto stack */
          current = tmp;
          action (current);
          *visited = g_list_prepend (*visited, current);
          g_queue_peek_head_link (stack)->data = GINT_TO_POINTER (i+1);
          g_queue_push_head (stack, GINT_TO_POINTER (0));
        }
      else
        {
          /* Pop from stack */
          tmp = current;
          current = atk_object_get_parent (current);
          g_object_unref (G_OBJECT (tmp));
          g_queue_pop_head (stack);
        }
    }

  return ref;
}

/*---------------------------------------------------------------------------*/

/*
 * Called when an already registered object is updated in such a
 * way that client side cache needs to be updated.
 */
static void
update_accessible (AtkObject *accessible)
{
  guint  ref = 0;
  g_assert(ATK_IS_OBJECT(accessible));

  ref = atk_dbus_object_to_ref (accessible);
  if (ref)
    {
      spi_emit_cache_update (accessible, atk_adaptor_app_data->bus);
    }
}

/*---------------------------------------------------------------------------*/

void
atk_dbus_foreach_registered(GHFunc func, gpointer data)
{
  g_hash_table_foreach(ref2ptr, func, data);
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
 * Events are not evaluated for non-registered accessibles.
 *
 * When a property change is made on a registered accessible
 * the client side cache should be updated.
 *
 * When a parent is changed the subtree is de-registered
 * if the parent is not attached to the root accessible.
 */
static gboolean
tree_update_listener (GSignalInvocationHint *signal_hint,
                      guint                  n_param_values,
                      const GValue          *param_values,
                      gpointer               data)
{
  AtkObject *accessible;
  AtkPropertyValues *values;
  const gchar *pname = NULL;

  accessible = g_value_get_object (&param_values[0]);
  values = (AtkPropertyValues*) g_value_get_pointer (&param_values[1]);

  pname = values[0].property_name;

  if (!atk_dbus_object_to_ref (accessible))
      return TRUE;

  if (strcmp (pname, "accessible-name") == 0 ||
      strcmp (pname, "accessible-description"))
    {
      atk_dbus_update_accessible (accessible);
    }
  else if (strcmp (pname, "accessible-parent"))
    {
      guint ref;

      ref = atk_dbus_object_to_ref;
      if (!ref)
    }
  return TRUE;
}

/*
 * Events are not evaluated for non registered accessibles.
 *
 * When the children of a registered accessible are changed
 * the subtree, rooted at the child is registered.
 */
static gboolean
tree_update_children_listener (GSignalInvocationHint *signal_hint,
                               guint                  n_param_values,
                               const GValue          *param_values,
                               gpointer               data)
{
  AtkObject *accessible;
  const gchar *detail = NULL;
  AtkObject *child;
  gboolean child_needs_unref = FALSE;

  if (signal_hint->detail)
    detail = g_quark_to_string (signal_hint->detail);

  accessible = g_value_get_object (&param_values[0]);
  if (!strcmp (detail, "add"))
    {
      gpointer child;
      int index = g_value_get_uint (param_values + 1);
      child = g_value_get_pointer (param_values + 2);

      if (ATK_IS_OBJECT (child))
          g_object_ref (child);
      else
          child = atk_object_ref_accessible_child (accessible, index);

      atk_dbus_register_subtree (child);
      g_object_unref (child);
    }
  return TRUE;
}

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

  atk_dbus_register_accessible (root);

  atk_add_global_event_listener (tree_update_listener, "Gtk:AtkObject:property-change");
  atk_add_global_event_listener (tree_update_children_listener, "Gtk:AtkObject:children-changed");
}

/*END------------------------------------------------------------------------*/

