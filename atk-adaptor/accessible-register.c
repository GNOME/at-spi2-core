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
 *
 */

/*
 * FIXME
 *
 * While traversing the ATK tree we may modify it unintentionally.
 * This is either a bug in the Gail implementation or this module.
 * If a change is caused that recurses, via a signal into this module
 * we should catch it.
 *
 * Things could also be changed that do not cause signal emission,
 * but do cause a failure. Not sure what these would be.
 *
 * The other option is that there are threads that modify the GUI.
 * This IS A BUG IN THE PROGRAM. But it may happen. If seeing very 
 * odd bugs change this to take the GDK lock. Just to make sure.
 */

static GHashTable *ref2ptr = NULL; /* Used for converting a D-Bus path (Reference) to the object pointer */

static guint counter = 1;

static GStaticRecMutex registration_mutex = G_STATIC_REC_MUTEX_INIT;

/*---------------------------------------------------------------------------*/

static GStaticMutex   recursion_check_guard = G_STATIC_MUTEX_INIT;
static gboolean       recursion_check = FALSE;

static gboolean
recursion_check_and_set ()
{
  gboolean ret;
  g_static_mutex_lock   (&recursion_check_guard);
  ret = recursion_check;
  recursion_check = TRUE;
  g_static_mutex_unlock (&recursion_check_guard);
  return ret;
}

static void
recursion_check_unset ()
{
  g_static_mutex_lock   (&recursion_check_guard);
  recursion_check = FALSE;
  g_static_mutex_unlock (&recursion_check_guard);
}

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
 * Callback for when a registered AtkObject is destroyed.
 *
 * Removes the AtkObject from the reference lookup tables, meaning
 * it is no longer exposed over D-Bus.
 */
static void
deregister_accessible (gpointer data, GObject *accessible)
{
  guint ref;
  g_assert (ATK_IS_OBJECT (accessible));

  ref = object_to_ref (ATK_OBJECT(accessible));
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

/*
 * This function checks that the ref-count of an accessible
 * is greater than 1.
 *
 * There is not currently any remote reference counting
 * in AT-SPI D-Bus so objects that are remotely owned are not
 * allowed.
 *
 * HACK - Needs permanent soltion.
 * TODO Add debug wrapper
 */
static gboolean
non_owned_accessible (AtkObject *accessible)
{
   if ((G_OBJECT (accessible))->ref_count <= 1)
     {
       g_critical ("AT-SPI: Child referenced that is not owned by its parent");
       return TRUE;
     }
   else
     {
       return FALSE;
     }
}

/*---------------------------------------------------------------------------*/

static gboolean
has_manages_descendants (AtkObject *accessible)
{
   AtkStateSet *state;
   gboolean result = FALSE;

   /* This is dangerous, refing the state set
    * seems to do wierd things to the tree & cause recursion
    * by modifying the tree alot.
    */
   state = atk_object_ref_state_set (accessible);
   if (atk_state_set_contains_state (state, ATK_STATE_MANAGES_DESCENDANTS))
     {
       g_message ("AT-SPI: Object with 'Manages descendants' states not currently handled by AT-SPI");
       result = TRUE;
     }
   g_object_unref (state);

   return result;
}

/*
 * Registers a subtree of accessible objects
 * rooted at the accessible object provided.
 *
 * The leaf nodes do not have their children
 * registered. A node is considered a leaf
 * if it has the state "manages-descendants"
 * or if it has already been registered.
 */
void
register_subtree (AtkObject *accessible)
{
  AtkObject *current, *tmp;
  GQueue    *stack;
  guint      i;
  gboolean   recurse;


  current = g_object_ref (accessible);
  if (has_manages_descendants (current))
    {
      g_object_unref (current);
      return;
    }

  stack = g_queue_new ();

  register_accessible (current);
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

          /* TODO Add debug wrapper */
          if (non_owned_accessible (tmp))
            {
              i++;
              continue;
            }

          if (object_to_ref (tmp))
            {
              /* If its already registered, just update */
              spi_emit_cache_update (tmp, atk_adaptor_app_data->bus);
            }
          else if (has_manages_descendants (tmp))
            {
              /* If it has manages descendants, just register and update */
              register_accessible (tmp);
              spi_emit_cache_update (tmp, atk_adaptor_app_data->bus);
            }
          else
            {
              recurse = TRUE;
            }

          if (!recurse)
            {
              i++;
              g_object_unref (G_OBJECT (tmp));
            }
        }

      if (recurse)
        {
          /* Push onto stack */
          current = tmp;
          register_accessible (current);

          g_queue_peek_head_link (stack)->data = GINT_TO_POINTER (i+1);
          g_queue_push_head (stack, GINT_TO_POINTER (0));
        }
      else
        {
          /* Pop from stack */
          spi_emit_cache_update (current, atk_adaptor_app_data->bus);
          tmp = current;
          current = atk_object_get_parent (current);
          g_object_unref (G_OBJECT (tmp));
          g_queue_pop_head (stack);
        }
    }
    g_queue_free (stack);
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

  ref = object_to_ref (accessible);
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

  ref = object_to_ref (accessible);
  if (!ref)
      return NULL;
  else
      return ref_to_path (ref);
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

  g_static_rec_mutex_lock (&registration_mutex);

  /* Ensure that only registered accessibles
   * have their signals processed.
   */
  accessible = g_value_get_object (&param_values[0]);
  if (object_to_ref (accessible))
    {
      /* TODO Add debug wrapper */
      if (recursion_check_and_set ())
          g_critical ("AT-SPI: Recursive use of registration module");

      if (!ATK_IS_OBJECT (accessible))
          g_critical ("AT-SPI: Object data updated when not a valid AtkObject");

      values = (AtkPropertyValues*) g_value_get_pointer (&param_values[1]);
      pname = values[0].property_name;
      if (strcmp (pname, "accessible-name") == 0 ||
          strcmp (pname, "accessible-description"))
        {
          update_accessible (accessible);
        }
      /* Parent updates not used */
      /* Parent value us updated buy child-add signal of parent object */
      /*
      else if (strcmp (pname, "accessible-parent"))
        {
          update_accessible (accessible);
        }
       */
      recursion_check_unset ();
    }

  g_static_rec_mutex_unlock (&registration_mutex);

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

  g_static_rec_mutex_lock (&registration_mutex);

  /* Ensure that only registered accessibles
   * have their signals processed.
   */
  accessible = g_value_get_object (&param_values[0]);
  if (object_to_ref (accessible))
    {
      /* TODO Add debug wrapper */
      if (recursion_check_and_set ())
          g_warning ("AT-SPI: Recursive use of registration module");

      if (!ATK_IS_OBJECT (accessible))
          g_critical ("AT-SPI: Object children updated when not a valid AtkObject");

      if (signal_hint->detail)
        detail = g_quark_to_string (signal_hint->detail);
      if (!strcmp (detail, "add"))
        {
          gpointer child;
          int index = g_value_get_uint (param_values + 1);
          child = g_value_get_pointer (param_values + 2);

          if (!ATK_IS_OBJECT (child))
            {
              child = atk_object_ref_accessible_child (accessible, index);
              /* TODO Add debug wrapper */
              if (!non_owned_accessible (child))
                {
                  register_subtree (child);
                }
            }
          else
            {
              register_subtree (child);
            }

        }

      recursion_check_unset ();
    }

  g_static_rec_mutex_unlock (&registration_mutex);

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

  if (g_thread_supported ())
      g_message ("AT-SPI: Threads enabled");

  register_subtree (root);

  atk_add_global_event_listener (tree_update_listener, "Gtk:AtkObject:property-change");
  atk_add_global_event_listener (tree_update_children_listener, "Gtk:AtkObject:children-changed");
}

/*END------------------------------------------------------------------------*/

