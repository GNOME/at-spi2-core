/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2007 IBM Corp.
 * Copyright 2010, 2011 Novell, Inc.
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

#include "atspi-private.h"

/* TODO: Improve documentation and implement some missing functions */

/**
 * AtspiCollection
 *
 * An interface designed to allow accessibles which satisfy a set of
 * criteria to be returned.
 *
 * An interface designed to allow accessibles which satisfy a set of
 * criteria to be returned. This interface can be used to avoid iteration
 * or client-side search of the object tree.
 */

/**
 * atspi_collection_is_ancestor_of:
 *
 * Not yet implemented.
 *
 **/
gboolean
atspi_collection_is_ancestor_of (AtspiCollection *collection,
                                 AtspiAccessible *test,
                                 GError **error)
{
  g_warning ("AT-SPI: TODO: Implement is_ancestor_of");
  return FALSE;
}

static DBusMessage *
new_message (AtspiCollection *collection, char *method)
{
  AtspiAccessible *accessible;

  g_assert (collection != NULL);

  accessible = ATSPI_ACCESSIBLE (collection);
  if (!accessible->parent.app)
    return NULL;
  return dbus_message_new_method_call (accessible->parent.app->bus_name,
                                       accessible->parent.path,
                                       atspi_interface_collection,
                                       method);
}

static gboolean
append_match_rule (DBusMessage *message, AtspiMatchRule *rule)
{
  DBusMessageIter iter;

  dbus_message_iter_init_append (message, &iter);
  return _atspi_match_rule_marshal (rule, &iter);
}

static gboolean
append_accessible (DBusMessage *message, AtspiAccessible *accessible)
{
  DBusMessageIter iter;

  dbus_message_iter_init_append (message, &iter);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_OBJECT_PATH,
                                  &accessible->parent.path);
  return TRUE; /* TODO: Check for out-of-memory */
}

static GArray *
return_accessibles (DBusMessage *message)
{
  DBusMessageIter iter, iter_array;
  GArray *ret = g_array_new (TRUE, TRUE, sizeof (AtspiAccessible *));

  _ATSPI_DBUS_CHECK_SIG (message, "a(so)", NULL, NULL);

  dbus_message_iter_init (message, &iter);
  dbus_message_iter_recurse (&iter, &iter_array);

  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
    {
      AtspiAccessible *accessible;
      accessible = _atspi_dbus_consume_accessible (&iter_array);
      ret = g_array_append_val (ret, accessible);
      /* Iter was moved already, so no need to call dbus_message_iter_next */
    }
  dbus_message_unref (message);
  return ret;
}

/**
 * atspi_collection_get_matches:
 * @collection: A pointer to the #AtspiCollection to query.
 * @rule: An #AtspiMatchRule describing the match criteria.
 * @sortby: An #AtspiCollectionSortOrder specifying the way the results are to
 *          be sorted.
 * @count: The maximum number of results to return, or 0 for no limit.
 * @traverse: Whether to traverse the accessible subtree (in case
 *         of #TRUE) or only the direct children (on case of #FALSE).
 *
 * Gets all #AtspiAccessible objects from the @collection matching a given
 * @rule.
 *
 * Returns: (element-type AtspiAccessible*) (transfer full): All
 *          #AtspiAccessible objects matching the given match rule.
 **/
GArray *
atspi_collection_get_matches (AtspiCollection *collection,
                              AtspiMatchRule *rule,
                              AtspiCollectionSortOrder sortby,
                              gint count,
                              gboolean traverse,
                              GError **error)
{
  DBusMessage *message = new_message (collection, "GetMatches");
  DBusMessage *reply;
  dbus_int32_t d_sortby = sortby;
  dbus_int32_t d_count = count;
  dbus_bool_t d_traverse = traverse;

  if (!message)
    return NULL;

  if (!append_match_rule (message, rule))
    return NULL;
  dbus_message_append_args (message, DBUS_TYPE_UINT32, &d_sortby,
                            DBUS_TYPE_INT32, &d_count,
                            DBUS_TYPE_BOOLEAN, &d_traverse,
                            DBUS_TYPE_INVALID);
  reply = _atspi_dbus_send_with_reply_and_block (message, error);
  if (!reply)
    return NULL;
  return return_accessibles (reply);
}

/**
 * atspi_collection_get_matches_to:
 * @collection: A pointer to the #AtspiCollection to query.
 * @current_object: The object at which to start searching.
 * @rule: An #AtspiMatchRule describing the match criteria.
 * @sortby: An #AtspiCollectionSortOrder specifying the way the results are to
 *          be sorted.
 * @tree: An #AtspiCollectionTreeTraversalType specifying restrictions on
 *          the objects to be traversed.
 * @limit_scope: If #TRUE, only descendants of @current_object's parent
 *          will be returned. Otherwise (if #FALSE), any accessible may be
 *          returned if it would preceed @current_object in a flattened
 *          hierarchy.
 * @count: The maximum number of results to return, or 0 for no limit.
 * @traverse: Whether to traverse the accessible subtree (in case
 *         of #TRUE) or only the direct children (on case of #FALSE).
 *
 * Gets all #AtspiAccessible objects from the @collection, after
 * @current_object, matching a given @rule.
 *
 * Returns: (element-type AtspiAccessible*) (transfer full): All
 *          #AtspiAccessible objects matching the given match rule after
 *          @current_object.
 **/
GArray *
atspi_collection_get_matches_to (AtspiCollection *collection,
                                 AtspiAccessible *current_object,
                                 AtspiMatchRule *rule,
                                 AtspiCollectionSortOrder sortby,
                                 AtspiCollectionTreeTraversalType tree,
                                 gboolean limit_scope,
                                 gint count,
                                 gboolean traverse,
                                 GError **error)
{
  DBusMessage *message = new_message (collection, "GetMatchesTo");
  DBusMessage *reply;
  dbus_int32_t d_sortby = sortby;
  dbus_int32_t d_tree = tree;
  dbus_bool_t d_limit_scope = limit_scope;
  dbus_int32_t d_count = count;
  dbus_bool_t d_traverse = traverse;

  if (!message)
    return NULL;

  if (!append_accessible (message, current_object))
    return NULL;
  if (!append_match_rule (message, rule))
    return NULL;
  dbus_message_append_args (message, DBUS_TYPE_UINT32, &d_sortby,
                            DBUS_TYPE_UINT32, &d_tree,
                            DBUS_TYPE_BOOLEAN, &d_limit_scope,
                            DBUS_TYPE_INT32, &d_count,
                            DBUS_TYPE_BOOLEAN, &d_traverse,
                            DBUS_TYPE_INVALID);
  reply = _atspi_dbus_send_with_reply_and_block (message, error);
  if (!reply)
    return NULL;
  return return_accessibles (reply);
}

/**
 * atspi_collection_get_matches_from:
 * @collection: A pointer to the #AtspiCollection to query.
 * @current_object: Upon reaching this object, searching should stop.
 * @rule: An #AtspiMatchRule describing the match criteria.
 * @sortby: An #AtspiCollectionSortOrder specifying the way the results are to
 *          be sorted.
 * @tree: An #AtspiCollectionTreeTraversalType specifying restrictions on
 *          the objects to be traversed.
 * @count: The maximum number of results to return, or 0 for no limit.
 * @traverse: Whether to traverse the accessible subtree (in case
 *         of #TRUE) or only the direct children (on case of #FALSE).
 *
 * Gets all #AtspiAccessible objects from the @collection, before
 * @current_object, matching a given @rule.
 *
 * Returns: (element-type AtspiAccessible*) (transfer full): All
 *          #AtspiAccessible objects matching the given match rule that preceed
 *          @current_object.
 **/
GArray *
atspi_collection_get_matches_from (AtspiCollection *collection,
                                   AtspiAccessible *current_object,
                                   AtspiMatchRule *rule,
                                   AtspiCollectionSortOrder sortby,
                                   AtspiCollectionTreeTraversalType tree,
                                   gint count,
                                   gboolean traverse,
                                   GError **error)
{
  DBusMessage *message = new_message (collection, "GetMatchesFrom");
  DBusMessage *reply;
  dbus_int32_t d_sortby = sortby;
  dbus_int32_t d_tree = tree;
  dbus_int32_t d_count = count;
  dbus_bool_t d_traverse = traverse;

  if (!message)
    return NULL;

  if (!append_accessible (message, current_object))
    return NULL;
  if (!append_match_rule (message, rule))
    return NULL;
  dbus_message_append_args (message, DBUS_TYPE_UINT32, &d_sortby,
                            DBUS_TYPE_UINT32, &d_tree,
                            DBUS_TYPE_INT32, &d_count,
                            DBUS_TYPE_BOOLEAN, &d_traverse,
                            DBUS_TYPE_INVALID);
  reply = _atspi_dbus_send_with_reply_and_block (message, error);
  if (!reply)
    return NULL;
  return return_accessibles (reply);
}

/**
 * atspi_collection_get_active_descendant:
 *
 * Returns: (transfer full): The active descendant of the given object.
 * Not yet implemented.
 **/
AtspiAccessible *
atspi_collection_get_active_descendant (AtspiCollection *collection, GError **error)
{
  g_warning ("AT-SPI: TODO: Implement get_active_descendants");
  return NULL;
}

static void
atspi_collection_base_init (AtspiCollection *klass)
{
}

GType
atspi_collection_get_type (void)
{
  static GType type = 0;

  if (!type)
    {
      static const GTypeInfo tinfo = {
        sizeof (AtspiCollection),
        (GBaseInitFunc) atspi_collection_base_init,
        (GBaseFinalizeFunc) NULL,
      };

      type = g_type_register_static (G_TYPE_INTERFACE, "AtspiCollection", &tinfo, 0);
    }
  return type;
}
