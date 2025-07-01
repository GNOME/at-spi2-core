/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
 * Copyright 2008 Codethink Ltd.
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

#include "droute-pairhash.h"
#include "droute.h"

#define CHUNKS_DEFAULT (512)

#define oom() g_error ("D-Bus out of memory, this message will fail anyway")

#if defined DROUTE_DEBUG
#define _DROUTE_DEBUG(format, args...) g_print (format, ##args)
#else
#define _DROUTE_DEBUG(format, args...)
#endif

struct _DRouteContext
{
  GPtrArray *registered_paths;

  gchar *introspect_string;
};

struct _DRoutePath
{
  DRouteContext *cnx;
  gchar *path;
  gboolean prefix;
  GStringChunk *chunks;
  GPtrArray *interfaces;
  GPtrArray *introspection;
  GHashTable *methods;
  GHashTable *properties;

  DRouteIntrospectChildrenFunction introspect_children_cb;
  void *introspect_children_data;
  void *user_data;
  DRouteGetDatumFunction get_datum;
  DRouteQueryInterfaceFunction query_interface_cb;
};

/*---------------------------------------------------------------------------*/

typedef struct PropertyPair
{
  DRoutePropertyFunction get;
  DRoutePropertyFunction set;
} PropertyPair;

/*---------------------------------------------------------------------------*/

static DBusHandlerResult
handle_message (DBusConnection *bus, DBusMessage *message, void *user_data);

static DBusMessage *
droute_object_does_not_exist_error (DBusMessage *message);

/*---------------------------------------------------------------------------*/

static DRoutePath *
path_new (DRouteContext *cnx,
          const char *path,
          gboolean prefix,
          void *user_data,
          DRouteIntrospectChildrenFunction introspect_children_cb,
          void *introspect_children_data,
          DRouteGetDatumFunction get_datum,
          DRouteQueryInterfaceFunction query_interface_cb)
{
  DRoutePath *new_path;

  new_path = g_new0 (DRoutePath, 1);
  new_path->cnx = cnx;
  new_path->path = g_strdup (path);
  new_path->prefix = prefix;
  new_path->chunks = g_string_chunk_new (CHUNKS_DEFAULT);
  new_path->interfaces = g_ptr_array_new ();
  new_path->introspection = g_ptr_array_new ();

  new_path->methods = g_hash_table_new_full ((GHashFunc) str_pair_hash,
                                             str_pair_equal,
                                             g_free,
                                             NULL);

  new_path->properties = g_hash_table_new_full ((GHashFunc) str_pair_hash,
                                                str_pair_equal,
                                                g_free,
                                                g_free);

  new_path->introspect_children_cb = introspect_children_cb;
  new_path->introspect_children_data = introspect_children_data;
  new_path->user_data = user_data;
  new_path->get_datum = get_datum;
  new_path->query_interface_cb = query_interface_cb;

  return new_path;
}

static void
path_free (DRoutePath *path, gpointer user_data)
{
  g_free (path->path);
  g_string_chunk_free (path->chunks);
  g_ptr_array_free (path->interfaces, TRUE);
  g_free (g_ptr_array_free (path->introspection, FALSE));
  g_hash_table_destroy (path->methods);
  g_hash_table_destroy (path->properties);
  g_free (path);
}

static void *
path_get_datum (DRoutePath *path, const gchar *pathstr)
{
  if (path->get_datum != NULL)
    return (path->get_datum) (pathstr, path->user_data);
  else
    return path->user_data;
}

/*---------------------------------------------------------------------------*/

DRouteContext *
droute_new ()
{
  DRouteContext *cnx;

  cnx = g_new0 (DRouteContext, 1);
  cnx->registered_paths = g_ptr_array_new ();

  return cnx;
}

void
droute_free (DRouteContext *cnx)
{
  g_ptr_array_foreach (cnx->registered_paths, (GFunc) path_free, NULL);
  g_ptr_array_free (cnx->registered_paths, TRUE);
  g_free (cnx);
}

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

static DBusObjectPathVTable droute_vtable = {
  NULL,
  &handle_message,
  NULL, NULL, NULL, NULL
};

DRoutePath *
droute_add_one (DRouteContext *cnx,
                const char *path,
                const void *data)
{
  DRoutePath *new_path;

  new_path = path_new (cnx, path, FALSE, (void *) data, NULL, NULL, NULL,
                       NULL);

  g_ptr_array_add (cnx->registered_paths, new_path);
  return new_path;
}

DRoutePath *
droute_add_many (DRouteContext *cnx,
                 const char *path,
                 const void *data,
                 DRouteIntrospectChildrenFunction introspect_children_cb,
                 void *introspect_children_data,
                 const DRouteGetDatumFunction get_datum,
                 const DRouteQueryInterfaceFunction query_interface_cb)
{
  DRoutePath *new_path;

  new_path = path_new (cnx, path, TRUE, (void *) data,
                       introspect_children_cb, introspect_children_data,
                       get_datum, query_interface_cb);

  g_ptr_array_add (cnx->registered_paths, new_path);
  return new_path;
}

/*---------------------------------------------------------------------------*/

void
droute_path_add_interface (DRoutePath *path,
                           const char *name,
                           const char *introspect,
                           const DRouteMethod *methods,
                           const DRouteProperty *properties)
{
  gchar *itf;

  g_return_if_fail (name != NULL);

  itf = g_string_chunk_insert (path->chunks, name);
  g_ptr_array_add (path->interfaces, itf);
  g_ptr_array_add (path->introspection, (gpointer) introspect);

  for (; methods != NULL && methods->name != NULL; methods++)
    {
      gchar *meth;

      meth = g_string_chunk_insert (path->chunks, methods->name);
      g_hash_table_insert (path->methods, str_pair_new (itf, meth), methods->func);
    }

  for (; properties != NULL && properties->name != NULL; properties++)
    {
      gchar *prop;
      PropertyPair *pair;

      prop = g_string_chunk_insert (path->chunks, properties->name);
      pair = g_new (PropertyPair, 1);
      pair->get = properties->get;
      pair->set = properties->set;
      g_hash_table_insert (path->properties, str_pair_new (itf, prop), pair);
    }
}

/*---------------------------------------------------------------------------*/

/* The data structures don't support an efficient implementation of GetAll
 * and I don't really care.
 */
static DBusMessage *
impl_prop_GetAll (DBusMessage *message,
                  DRoutePath *path,
                  const char *pathstr)
{
  DBusMessageIter iter, iter_dict, iter_dict_entry;
  DBusMessage *reply;
  DBusError error;
  GHashTableIter prop_iter;

  StrPair *key;
  PropertyPair *value;
  gchar *iface;

  void *datum = path_get_datum (path, pathstr);
  if (!datum)
    return droute_object_does_not_exist_error (message);

  dbus_error_init (&error);
  if (!dbus_message_get_args (message, &error, DBUS_TYPE_STRING, &iface, DBUS_TYPE_INVALID))
    {
      DBusMessage *ret;
      ret = dbus_message_new_error (message, DBUS_ERROR_FAILED, error.message);
      dbus_error_free (&error);
      return ret;
    }

  if (path->query_interface_cb &&
      !path->query_interface_cb (datum, iface))
    return dbus_message_new_error (message, DBUS_ERROR_UNKNOWN_PROPERTY, "Property unavailable");

  reply = dbus_message_new_method_return (message);
  if (!reply)
    oom ();

  dbus_message_iter_init_append (reply, &iter);
  if (!dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "{sv}", &iter_dict))
    oom ();

  g_hash_table_iter_init (&prop_iter, path->properties);
  while (g_hash_table_iter_next (&prop_iter, (gpointer *) &key, (gpointer *) &value))
    {
      if (!g_strcmp0 (key->one, iface))
        {
          if (!value->get)
            continue;
          if (!dbus_message_iter_open_container (&iter_dict, DBUS_TYPE_DICT_ENTRY, NULL, &iter_dict_entry))
            oom ();
          dbus_message_iter_append_basic (&iter_dict_entry, DBUS_TYPE_STRING,
                                          &key->two);
          (value->get) (&iter_dict_entry, datum);
          if (!dbus_message_iter_close_container (&iter_dict, &iter_dict_entry))
            oom ();
        }
    }

  if (!dbus_message_iter_close_container (&iter, &iter_dict))
    oom ();
  return reply;
}

static DBusMessage *
impl_prop_GetSet (DBusMessage *message,
                  DRoutePath *path,
                  const char *pathstr,
                  gboolean get)
{
  DBusMessage *reply = NULL;
  DBusError error;

  StrPair pair;
  PropertyPair *prop_funcs = NULL;

  void *datum;

  dbus_error_init (&error);
  if (!dbus_message_get_args (message,
                              &error,
                              DBUS_TYPE_STRING,
                              &(pair.one),
                              DBUS_TYPE_STRING,
                              &(pair.two),
                              DBUS_TYPE_INVALID))
    {
      DBusMessage *ret;
      ret = dbus_message_new_error (message, DBUS_ERROR_FAILED, error.message);
      dbus_error_free (&error);
      return ret;
    }

  _DROUTE_DEBUG ("DRoute (handle prop): %s|%s on %s\n", pair.one, pair.two, pathstr);

  prop_funcs = (PropertyPair *) g_hash_table_lookup (path->properties, &pair);
  if (!prop_funcs)
    {
      DBusMessage *ret;
      ret = dbus_message_new_error (message, DBUS_ERROR_UNKNOWN_PROPERTY, "Property unavailable");
      dbus_error_free (&error);
      return ret;
    }

  datum = path_get_datum (path, pathstr);
  if (!datum)
    return droute_object_does_not_exist_error (message);

  if (get && prop_funcs->get)
    {

      DBusMessageIter iter;

      _DROUTE_DEBUG ("DRoute (handle prop Get): %s|%s on %s\n", pair.one, pair.two, pathstr);

      reply = dbus_message_new_method_return (message);
      dbus_message_iter_init_append (reply, &iter);
      if (!(prop_funcs->get) (&iter, datum))
        {
          dbus_message_unref (reply);
          reply = dbus_message_new_error (message, DBUS_ERROR_FAILED, "Get failed");
        }
    }
  else if (!get && prop_funcs->set)
    {
      DBusMessageIter iter;

      _DROUTE_DEBUG ("DRoute (handle prop Get): %s|%s on %s\n", pair.one, pair.two, pathstr);

      dbus_message_iter_init (message, &iter);
      /* Skip the interface and property name */
      dbus_message_iter_next (&iter);
      dbus_message_iter_next (&iter);
      (prop_funcs->set) (&iter, datum);

      reply = dbus_message_new_method_return (message);
    }
  else if (!get)
    {
      reply = dbus_message_new_error (message, DBUS_ERROR_PROPERTY_READ_ONLY, "Property is read-only");
    }
  else
    {
      reply = dbus_message_new_error (message, DBUS_ERROR_FAILED, "Getter or setter unavailable");
    }

  return reply;
}

static DBusHandlerResult
handle_dbus (DBusConnection *bus,
             DBusMessage *message,
             const gchar *iface,
             const gchar *member,
             const gchar *pathstr)
{
  static int id = 1;
  char *id_str = (char *) g_malloc (40);
  DBusMessage *reply;

  if (strcmp (iface, DBUS_INTERFACE_DBUS) != 0 ||
      strcmp (member, "Hello") != 0)
    {
      g_free (id_str);
      return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

  /* TODO: Fix this hack (we don't handle wrap-around, for instance) */
  sprintf (id_str, ":1.%d", id++);
  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_STRING, &id_str, DBUS_TYPE_INVALID);
  dbus_connection_send (bus, reply, NULL);
  dbus_connection_flush (bus);
  dbus_message_unref (reply);
  g_free (id_str);
  return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult
handle_properties (DBusConnection *bus,
                   DBusMessage *message,
                   DRoutePath *path,
                   const gchar *iface,
                   const gchar *member,
                   const gchar *pathstr)
{
  DBusMessage *reply = NULL;
  DBusHandlerResult result = DBUS_HANDLER_RESULT_HANDLED;

  if (!g_strcmp0 (member, "GetAll"))
    reply = impl_prop_GetAll (message, path, pathstr);
  else if (!g_strcmp0 (member, "Get"))
    reply = impl_prop_GetSet (message, path, pathstr, TRUE);
  else if (!g_strcmp0 (member, "Set"))
    reply = impl_prop_GetSet (message, path, pathstr, FALSE);
  else
    result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  if (reply)
    {
      dbus_connection_send (bus, reply, NULL);
      dbus_message_unref (reply);
    }

  return result;
}

/*---------------------------------------------------------------------------*/

static const char *introspection_header =
    "<?xml version=\"1.0\"?>\n";

static const char *introspection_node_element =
    "<node name=\"%s\">\n";

static const char *introspection_footer =
    "</node>";

static DBusHandlerResult
handle_introspection (DBusConnection *bus,
                      DBusMessage *message,
                      DRoutePath *path,
                      const gchar *iface,
                      const gchar *member,
                      const gchar *pathstr)
{
  GString *output;
  gchar *final;
  gint i;
  void *datum = NULL;

  DBusMessage *reply;

  _DROUTE_DEBUG ("DRoute (handle introspection): %s\n", pathstr);

  if (g_strcmp0 (member, "Introspect"))
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  output = g_string_new (introspection_header);

  g_string_append_printf (output, introspection_node_element, pathstr);

  if (!path->get_datum || (datum = path_get_datum (path, pathstr)) != NULL)
    {
      for (i = 0; i < path->introspection->len; i++)
        {
          gchar *interface = (gchar *) g_ptr_array_index (path->interfaces, i);
          gchar *introspect = (gchar *) g_ptr_array_index (path->introspection, i);
          if (path->query_interface_cb &&
              !path->query_interface_cb (datum, interface))
            continue;
          g_string_append (output, introspect);
        }
    }

  if (path->introspect_children_cb)
    {
      gchar *children = (*path->introspect_children_cb) (pathstr, path->introspect_children_data);
      if (children)
        {
          g_string_append (output, children);
          g_free (children);
        }
    }

  g_string_append (output, introspection_footer);
  final = g_string_free (output, FALSE);

  reply = dbus_message_new_method_return (message);
  if (!reply)
    oom ();
  dbus_message_append_args (reply, DBUS_TYPE_STRING, &final,
                            DBUS_TYPE_INVALID);
  dbus_connection_send (bus, reply, NULL);

  dbus_message_unref (reply);
  g_free (final);
  return DBUS_HANDLER_RESULT_HANDLED;
}

/*---------------------------------------------------------------------------*/

static DBusHandlerResult
handle_other (DBusConnection *bus,
              DBusMessage *message,
              DRoutePath *path,
              const gchar *iface,
              const gchar *member,
              const gchar *pathstr)
{
  gint result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  StrPair pair;
  DRouteFunction func;
  DBusMessage *reply = NULL;

  void *datum;

  pair.one = iface;
  pair.two = member;

  _DROUTE_DEBUG ("DRoute (handle other): %s|%s on %s\n", member, iface, pathstr);

  func = (DRouteFunction) g_hash_table_lookup (path->methods, &pair);
  if (func != NULL)
    {
      datum = path_get_datum (path, pathstr);
      if (!datum)
        reply = droute_object_does_not_exist_error (message);
      else
        reply = (func) (bus, message, datum);

      /* All D-Bus method calls must have a reply.
       * If one is not provided presume that the caller has already
       * sent one.
       */
      if (reply)
        {
          dbus_connection_send (bus, reply, NULL);
          dbus_message_unref (reply);
        }
      result = DBUS_HANDLER_RESULT_HANDLED;
    }

  _DROUTE_DEBUG ("DRoute (handle other) (reply): type %d\n",
                 dbus_message_get_type (reply));
  return result;
}

/*---------------------------------------------------------------------------*/

static DBusHandlerResult
handle_message (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  DRoutePath *path = (DRoutePath *) user_data;
  const gchar *iface = dbus_message_get_interface (message);
  const gchar *member = dbus_message_get_member (message);
  const gint type = dbus_message_get_type (message);
  const gchar *pathstr = dbus_message_get_path (message);

  DBusHandlerResult result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  _DROUTE_DEBUG ("DRoute (handle message): %s|%s of type %d on %s\n", member, iface, type, pathstr);

  /* Check for basic reasons not to handle */
  if (type != DBUS_MESSAGE_TYPE_METHOD_CALL ||
      member == NULL ||
      iface == NULL)
    return result;

  if (!strcmp (pathstr, DBUS_PATH_DBUS))
    result = handle_dbus (bus, message, iface, member, pathstr);
  else if (!strcmp (iface, "org.freedesktop.DBus.Properties"))
    result = handle_properties (bus, message, path, iface, member, pathstr);
  else if (!strcmp (iface, "org.freedesktop.DBus.Introspectable"))
    result = handle_introspection (bus, message, path, iface, member, pathstr);
  else
    result = handle_other (bus, message, path, iface, member, pathstr);
#if 0
    if (result == DBUS_HANDLER_RESULT_NOT_YET_HANDLED)
        g_print ("DRoute | Unhandled message: %s|%s of type %d on %s\n", member, iface, type, pathstr);
#endif

  return result;
}

/*---------------------------------------------------------------------------*/

static DBusMessage *
droute_object_does_not_exist_error (DBusMessage *message)
{
  DBusMessage *reply;
  gchar *errmsg;

  errmsg = g_strdup_printf (
      "Method \"%s\" with signature \"%s\" on interface \"%s\" could not be processed as object %s does not exist\n",
      dbus_message_get_member (message),
      dbus_message_get_signature (message),
      dbus_message_get_interface (message),
      dbus_message_get_path (message));
  reply = dbus_message_new_error (message,
                                  DBUS_ERROR_UNKNOWN_OBJECT,
                                  errmsg);
  g_free (errmsg);
  return reply;
}

/*---------------------------------------------------------------------------*/

DBusMessage *
droute_not_yet_handled_error (DBusMessage *message)
{
  DBusMessage *reply;
  gchar *errmsg;

  errmsg = g_strdup_printf (
      "Method \"%s\" with signature \"%s\" on interface \"%s\" doesn't exist\n",
      dbus_message_get_member (message),
      dbus_message_get_signature (message),
      dbus_message_get_interface (message));
  reply = dbus_message_new_error (message,
                                  DBUS_ERROR_UNKNOWN_METHOD,
                                  errmsg);
  g_free (errmsg);
  return reply;
}

DBusMessage *
droute_out_of_memory_error (DBusMessage *message)
{
  DBusMessage *reply;
  gchar *errmsg;

  errmsg = g_strdup_printf (
      "Method \"%s\" with signature \"%s\" on interface \"%s\" could not be processed due to lack of memory\n",
      dbus_message_get_member (message),
      dbus_message_get_signature (message),
      dbus_message_get_interface (message));
  reply = dbus_message_new_error (message,
                                  DBUS_ERROR_NO_MEMORY,
                                  errmsg);
  g_free (errmsg);
  return reply;
}

DBusMessage *
droute_invalid_arguments_error (DBusMessage *message)
{
  DBusMessage *reply;
  gchar *errmsg;

  errmsg = g_strdup_printf (
      "Method \"%s\" with signature \"%s\" on interface \"%s\" was supplied with invalid arguments\n",
      dbus_message_get_member (message),
      dbus_message_get_signature (message),
      dbus_message_get_interface (message));
  reply = dbus_message_new_error (message,
                                  DBUS_ERROR_INVALID_ARGS,
                                  errmsg);
  g_free (errmsg);
  return reply;
}

void
droute_path_register (DRoutePath *path, DBusConnection *bus)
{
  if (path->prefix)
    dbus_connection_register_fallback (bus, path->path, &droute_vtable, path);
  else
    dbus_connection_register_object_path (bus, path->path,
                                          &droute_vtable, path);
}

void
droute_path_unregister (DRoutePath *path, DBusConnection *bus)
{
  dbus_connection_unregister_object_path (bus, path->path);
}

void
droute_context_register (DRouteContext *cnx, DBusConnection *bus)
{
  g_ptr_array_foreach (cnx->registered_paths, (GFunc) droute_path_register,
                       bus);
}

void
droute_context_unregister (DRouteContext *cnx, DBusConnection *bus)
{
  g_ptr_array_foreach (cnx->registered_paths, (GFunc) droute_path_unregister,
                       bus);
}

void
droute_intercept_dbus (DBusConnection *bus)
{
  dbus_connection_register_object_path (bus, DBUS_PATH_DBUS,
                                        &droute_vtable, NULL);
}

void
droute_unintercept_dbus (DBusConnection *bus)
{
  dbus_connection_unregister_object_path (bus, DBUS_PATH_DBUS);
}

/*END------------------------------------------------------------------------*/
