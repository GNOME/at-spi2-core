/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
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

#include "accessible.h"

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
 * 
 * To enbale this two hash tables are kept. One keeps a list of AtkObjects
 * that have been updated. The other a list of objects that have been
 * removed since the last 'update' signal. The reason they are
 * stored as hash tables is to ensure that if an AtkObject is removed
 * and then added between update signals then the object is easy to delete
 * from the 'update' list without doing a costly lookup.
 *
 * The mappings will be called 'reference lookup tables'
 * The hashtable containing AtkObjects that need updating in the client side
 * cache will be called the 'update list'
 * The hashtable containing the AtkObjects that need removing from the client
 * side cache will be called the 'removal list'
 */

GHashTable *ref2ptr = NULL; /* Used for converting a D-Bus path (Reference) to the object pointer */

GHashTable *update_list = NULL; /* Stores the objects that need a client side cache update */
GHashTable *remove_list = NULL; /* Stores the objects that need to be removed from the client side cache */

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
    return counter;
}

/*---------------------------------------------------------------------------*/

/*
 * Called when a registered AtkObject is deleted.
 *
 * Removes the AtkObject from the reference lookup tables.
 * Adds the reference of the object to the removal list.
 */
static void
deregister_accessible(gpointer data, GObject *accessible)
{
  guint ref;

  g_assert(ATK_IS_OBJECT(accessible));


  ref = GPOINTER_TO_INT(g_object_get_data (accessible, "dbus-id"));

  /* Remove from update list */
  g_hash_table_remove(update_list, GINT_TO_POINTER(ref));

  if (ref != 0)
    {
      g_hash_table_remove(ref2ptr, GINT_TO_POINTER(ref));
      /* Add to removal list */
      /* 
       * TODO
       * Pyatspi client side exceptions have occured indicating
       * that an object has been removed twice.
       * This should not be possible and needs investigation.
       */
      g_hash_table_insert(remove_list, GINT_TO_POINTER(ref), NULL);
    }

  atk_tree_cache_needs_update();
}

/*---------------------------------------------------------------------------*/

/*
 * Registers a new AtkObject.
 *
 * Adds the AtkObject to the reference lookup tables.
 * Adds the AtkObject to the update list.
 */
static guint
register_accessible (AtkObject *accessible)
{
  guint reference;

  g_assert(ATK_IS_OBJECT(accessible));

  reference = assign_reference();

  g_hash_table_insert (ref2ptr, GINT_TO_POINTER(reference), accessible);
  g_object_set_data (G_OBJECT(accessible), "dbus-id", GINT_TO_POINTER(reference));
  g_object_weak_ref(G_OBJECT(accessible), deregister_accessible, NULL);

  /* Add to update list */
  g_hash_table_insert (update_list, GINT_TO_POINTER(reference), accessible);

  atk_tree_cache_needs_update();

  return reference;
}

/*---------------------------------------------------------------------------*/

/* TODO Turn these into an iterator API - Think about the locking */

void
atk_dbus_foreach_registered(GHFunc func, gpointer data)
{
  g_hash_table_foreach(ref2ptr, func, data);
}

/*---------------------------------------------------------------------------*/

void
atk_dbus_foreach_update_list(GHFunc func, gpointer data)
{
  g_hash_table_foreach(update_list, func, data);
  g_hash_table_remove_all(update_list);
}

/*---------------------------------------------------------------------------*/

void
atk_dbus_foreach_remove_list(GHFunc func, gpointer data)
{
  g_hash_table_foreach(remove_list, func, data);
  g_hash_table_remove_all(remove_list);
}

/*---------------------------------------------------------------------------*/

/* 
 * Called on an AtkObject when it has changed in such a way
 * that the client side cache of the object will need updating.
 */
void 
atk_dbus_notify_change(AtkObject *accessible)
{
  guint ref;
  g_assert(ATK_IS_OBJECT(accessible));
  
  if (!g_object_get_data (G_OBJECT (accessible), "dbus-id"))
    {
      register_accessible(accessible);
    }
  else
    {
      ref = g_object_get_data (G_OBJECT (accessible), "dbus-id");
      g_hash_table_insert (update_list, ref, accessible);
      atk_tree_cache_needs_update();
    }
}

/*---------------------------------------------------------------------------*/

/*
 * Used to lookup an AtkObject from its D-Bus path.
 */
AtkObject *
atk_dbus_get_object (const char *path)
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

/*---------------------------------------------------------------------------*/

gchar *
atk_dbus_get_path_from_ref(guint ref)
{
  return g_strdup_printf(ATK_BRIDGE_OBJECT_REFERENCE_TEMPLATE, ref);
}

/*---------------------------------------------------------------------------*/

/*
 * Used to lookup a D-Bus path from the AtkObject.
 *
 * Objects without a path are registered and provided with one.
 */
gchar *
atk_dbus_get_path (AtkObject *accessible)
{
  guint index;

  g_assert (accessible);

  index = GPOINTER_TO_INT(g_object_get_data (G_OBJECT (accessible), "dbus-id"));
  if (!index)
    index = register_accessible(G_OBJECT(accessible));

  return g_strdup_printf(ATK_BRIDGE_OBJECT_REFERENCE_TEMPLATE, index);
}

/*---------------------------------------------------------------------------*/

/*
 * Used to recursively register accessibles.
 *
 * When children are added to an accessible we need to 
 * iterate over the new subtree provided to register new accessibles.
 */
guint
atk_dbus_register_subtree(AtkObject *accessible)
{
  AtkObject *child;
  guint i, n_children;
  guint ref;

  ref = GPOINTER_TO_INT(g_object_get_data (G_OBJECT (accessible), "dbus-id"));
  if (!ref)
     ref = register_accessible(accessible);

  n_children = atk_object_get_n_accessible_children(accessible);
  for (i=0; i < n_children; i++)
    {
      child = atk_object_ref_accessible_child(accessible, i);
      atk_dbus_register_subtree(child);
    } 
  return ref;
} 

/*---------------------------------------------------------------------------*/

/* 
 * Marshals a single object into a D-Bus message.
 *
 * Unrefs the AtkObject if unref is true.
 */
DBusMessage *
spi_dbus_return_object (DBusMessage *message, AtkObject *obj, gboolean unref)
{
  DBusMessage *reply;
  gchar *path;
  
  path = atk_dbus_get_path (obj);

  if (unref)
    g_object_unref (obj);

  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_OBJECT_PATH, path,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

/*---------------------------------------------------------------------------*/

/*
 * Marshals a variant containing the object reference into the message iter
 * provided.
 *
 * Unrefs the object if unref is true.
 */
dbus_bool_t
spi_dbus_return_v_object (DBusMessageIter *iter, AtkObject *obj, int unref)
{
  char *path;
  
  path = atk_dbus_get_path (obj);

  if (unref)
    g_object_unref (obj);

  return droute_return_v_object (iter, path);
}

/*---------------------------------------------------------------------------*/

/*
 * Initializes required global data. The update and removal lists
 * and the reference lookup tables.
 *
 * Initializes all of the required D-Bus interfaces.
 */
void
atk_dbus_initialize (DRouteData * data)
{
  if (!ref2ptr)
    ref2ptr = g_hash_table_new(g_direct_hash, g_direct_equal);
  if (!update_list)
    update_list = g_hash_table_new(g_direct_hash, g_direct_equal);
  if (!remove_list)
    remove_list = g_hash_table_new(g_direct_hash, g_direct_equal);

  /* Get the root accessible and add */
  atk_dbus_register_subtree(atk_get_root());

  spi_initialize_accessible (data);
  spi_initialize_action(data);
  spi_initialize_collection (data);
  spi_initialize_component (data);
  spi_initialize_document (data);
  spi_initialize_editabletext (data);
  spi_initialize_hyperlink (data);
  spi_initialize_hypertext (data);
  spi_initialize_image (data);
  spi_initialize_selection (data);
  spi_initialize_table (data);
  spi_initialize_text (data);
  spi_initialize_value (data);
  spi_initialize_introspectable(data, (DRouteGetDatumFunction) atk_dbus_get_object);
}

/*END------------------------------------------------------------------------*/

