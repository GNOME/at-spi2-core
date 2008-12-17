/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
 * Copyright 2008 Codethink Ltd.
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

#include <stdlib.h>
#include <string.h>

#include "droute.h"
#include "droute-pairhash.h"

#define CHUNKS_DEFAULT (512)

#define oom() g_error ("D-Bus out of memory, this message will fail anyway")

#if defined DROUTE_DEBUG
    #define _DROUTE_DEBUG(format, args...) g_print (format , ## args)
#else
    #define _DROUTE_DEBUG
#endif

struct _DRouteContext
{
    DBusConnection       *bus;
    GPtrArray            *registered_paths;

    gchar                *introspect_dir;
};

struct _DRoutePath
{
    DRouteContext        *cnx;
    GStringChunk         *chunks;
    GPtrArray            *interfaces;
    GHashTable           *methods;
    GHashTable           *properties;

    void                   *user_data;
    DRouteGetDatumFunction  get_datum;
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

/*---------------------------------------------------------------------------*/

static DRoutePath *
path_new (DRouteContext *cnx,
          void    *user_data,
          DRouteGetDatumFunction get_datum)
{
    DRoutePath *new_path;

    new_path = g_new0 (DRoutePath, 1);
    new_path->cnx = cnx;
    new_path->chunks = g_string_chunk_new (CHUNKS_DEFAULT);
    new_path->interfaces = g_ptr_array_new ();

    new_path->methods = g_hash_table_new_full ((GHashFunc)str_pair_hash,
                                               str_pair_equal,
                                               g_free,
                                               NULL);

    new_path->properties = g_hash_table_new_full ((GHashFunc)str_pair_hash,
                                                  str_pair_equal,
                                                  g_free,
                                                  NULL);

    new_path->user_data = user_data;
    new_path->get_datum = get_datum;

    return new_path;
}

static void
path_free (DRoutePath *path, gpointer user_data)
{
    g_string_chunk_free  (path->chunks);
    g_ptr_array_free     (path->interfaces, TRUE);
    g_hash_table_destroy (path->methods);
    g_hash_table_destroy (path->properties);
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
droute_new (DBusConnection *bus, const char *introspect_dir)
{
    DRouteContext *cnx;

    cnx = g_new0 (DRouteContext, 1);
    cnx->bus = bus;
    cnx->registered_paths = g_ptr_array_new ();
    cnx->introspect_dir = g_strdup(introspect_dir);
}

void
droute_free (DRouteContext *cnx)
{
    g_ptr_array_foreach (cnx->registered_paths, (GFunc) path_free, NULL);
    g_free (cnx->introspect_dir);
    g_free (cnx);
}

/*---------------------------------------------------------------------------*/

DBusConnection *
droute_get_bus (DRouteContext *cnx)
{
    return cnx->bus;
}

/*---------------------------------------------------------------------------*/

static DBusObjectPathVTable droute_vtable =
{
  NULL,
  &handle_message,
  NULL, NULL, NULL, NULL
};

DRoutePath *
droute_add_one (DRouteContext *cnx,
                const char    *path,
                const void    *data)
{
    DRoutePath *new_path;

    new_path = path_new (cnx, (void *) data, NULL);

    if (!dbus_connection_register_object_path (cnx->bus, path, &droute_vtable, new_path))
        oom();

    g_ptr_array_add (cnx->registered_paths, new_path);
    return new_path;
}

DRoutePath *
droute_add_many (DRouteContext *cnx,
                 const char    *path,
                 const void    *data,
                 const DRouteGetDatumFunction get_datum)
{
    DRoutePath *new_path;

    new_path = path_new (cnx, (void *) data, get_datum);

    if (!dbus_connection_register_fallback (cnx->bus, path, &droute_vtable, new_path))
        oom();

    g_ptr_array_add (cnx->registered_paths, new_path);
    return new_path;
}

/*---------------------------------------------------------------------------*/

void
droute_path_add_interface(DRoutePath *path,
                          const char *name,
                          const DRouteMethod   *methods,
                          const DRouteProperty *properties)
{
    gchar *itf;

    g_return_if_fail (name != NULL);

    itf = g_string_chunk_insert (path->chunks, name);
    g_ptr_array_add (path->interfaces, itf);

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
                  DRoutePath  *path,
                  const char  *pathstr)
{
    DBusMessageIter iter, iter_dict, iter_dict_entry;
    DBusMessage *reply;
    DBusError error;
    GHashTableIter prop_iter;

    StrPair *key;
    PropertyPair *value;
    gchar *iface;

    void  *datum = path_get_datum (path, pathstr);

    dbus_error_init (&error);
    if (!dbus_message_get_args
                (message, &error, DBUS_TYPE_STRING, &iface, DBUS_TYPE_INVALID))
        return dbus_message_new_error (message, DBUS_ERROR_FAILED, error.message);

    reply = dbus_message_new_method_return (message);
    if (!reply)
        oom ();

    dbus_message_iter_init_append (reply, &iter);
    if (!dbus_message_iter_open_container
                (&iter, DBUS_TYPE_ARRAY, "{sv}", &iter_dict))
        oom ();

    g_hash_table_iter_init (&prop_iter, path->properties);
    while (g_hash_table_iter_next (&prop_iter, (gpointer*)&key, (gpointer*)&value))
      {
        if (!g_strcmp0 (key->one, iface))
         {
           if (!value->get)
              continue;
           if (!dbus_message_iter_open_container
                        (&iter_dict, DBUS_TYPE_DICT_ENTRY, NULL, &iter_dict_entry))
              oom ();
           dbus_message_iter_append_basic (&iter_dict_entry, DBUS_TYPE_STRING,
                                           key->two);
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
                  DRoutePath  *path,
                  const char  *pathstr,
                  gboolean     get)
{
    DBusMessage *reply = NULL;
    DBusError error;

    StrPair pair;
    PropertyPair *prop_funcs = NULL;


    dbus_error_init (&error);
    if (!dbus_message_get_args (message,
                                &error,
                                DBUS_TYPE_STRING,
                                &(pair.one),
                                DBUS_TYPE_STRING,
                                &(pair.two),
                                DBUS_TYPE_INVALID))
        return dbus_message_new_error (message, DBUS_ERROR_FAILED, error.message);

    prop_funcs = (PropertyPair *) g_hash_table_lookup (path->properties, &pair);
    if (!prop_funcs)
        return dbus_message_new_error (message, DBUS_ERROR_FAILED, "Property unavailable");

    if (get && prop_funcs->get)
      {
        void *datum = path_get_datum (path, pathstr);
        DBusMessageIter iter;

        _DROUTE_DEBUG ("DRoute (handle prop Get): %s|%s on %s\n", pair.one, pair.two, pathstr);

        reply = dbus_message_new_method_return (message);
        dbus_message_iter_init_append (reply, &iter);
        (prop_funcs->get) (&iter, datum);
      }
    else if (!get && prop_funcs->set)
      {
        void *datum = path_get_datum (path, pathstr);
        DBusMessageIter iter;

        _DROUTE_DEBUG ("DRoute (handle prop Get): %s|%s on %s\n", pair.one, pair.two, pathstr);

        dbus_message_iter_init_append (message, &iter);
        /* Skip the interface and property name */
        dbus_message_iter_next(&iter);
        dbus_message_iter_next(&iter);
        (prop_funcs->set) (&iter, datum);

        reply = dbus_message_new_method_return (message);
      }
    else
      {
        reply = dbus_message_new_error (message, DBUS_ERROR_FAILED, "Getter or setter unavailable");
      }

    return reply;
}

static DBusHandlerResult
handle_properties (DBusConnection *bus,
                   DBusMessage    *message,
                   DRoutePath     *path,
                   const gchar    *iface,
                   const gchar    *member,
                   const gchar    *pathstr)
{
    DBusMessage *reply;
    DBusHandlerResult result = DBUS_HANDLER_RESULT_HANDLED;

    if (!g_strcmp0(member, "GetAll"))
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

static void
append_interface (GString     *str,
                  const gchar *interface,
                  const gchar *directory)
{
    gchar *filename;
    gchar *contents;
    gsize len;

    GError *err = NULL;

    filename = g_build_filename (directory, interface, NULL);

    if (g_file_get_contents (filename, &contents, &len, &err))
      {
        g_string_append_len (str, contents, len);
      }
    else
      {
        g_warning ("AT-SPI: Cannot find introspection XML file %s - %s",
                   filename, err->message);
        g_error_free (err);
      }

    g_string_append (str, "\n");
    g_free (filename);
    g_free (contents);
}

static DBusHandlerResult
handle_introspection (DBusConnection *bus,
                      DBusMessage    *message,
                      DRoutePath     *path,
                      const gchar    *iface,
                      const gchar    *member,
                      const gchar    *pathstr)
{
    GString *output;
    gchar *final;
    gint i;

    DBusMessage *reply;

    _DROUTE_DEBUG ("DRoute (handle introspection): %s\n", pathstr);

    if (g_strcmp0 (member, "Introspect"))
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    output = g_string_new(introspection_header);

    g_string_append_printf(output, introspection_node_element, pathstr);

    for (i=0; i < path->interfaces->len; i++)
      {
        gchar *interface = (gchar *) g_ptr_array_index (path->interfaces, i);
        append_interface(output, interface, path->cnx->introspect_dir);
      }

    g_string_append(output, introspection_footer);
    final = g_string_free(output, FALSE);

    reply = dbus_message_new_method_return (message);
    if (!reply)
        oom ();
    dbus_message_append_args(reply, DBUS_TYPE_STRING, &final,
                             DBUS_TYPE_INVALID);
    dbus_connection_send (bus, reply, NULL);

    dbus_message_unref (reply);
    g_free(final);
    return DBUS_HANDLER_RESULT_HANDLED;
}

/*---------------------------------------------------------------------------*/

static DBusHandlerResult
handle_other (DBusConnection *bus,
              DBusMessage    *message,
              DRoutePath     *path,
              const gchar    *iface,
              const gchar    *member,
              const gchar    *pathstr)
{
    gint result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    StrPair pair;
    DRouteFunction func;
    DBusMessage *reply;

    pair.one = iface;
    pair.two = member;

    _DROUTE_DEBUG ("DRoute (handle other): %s|%s on %s\n", member, iface, pathstr);

    func = (DRouteFunction) g_hash_table_lookup (path->methods, &pair);
    if (func != NULL)
      {
        void *datum = path_get_datum (path, pathstr);

        reply = (func) (bus, message, datum);

        if (reply)
          {
            dbus_connection_send (bus, reply, NULL);
            dbus_message_unref (reply);
          }
        result = DBUS_HANDLER_RESULT_HANDLED;
      }
    return result;
}

/*---------------------------------------------------------------------------*/

static DBusHandlerResult
handle_message (DBusConnection *bus, DBusMessage *message, void *user_data)
{
    DRoutePath *path = (DRoutePath *) user_data;
    const gchar *iface   = dbus_message_get_interface (message);
    const gchar *member  = dbus_message_get_member (message);
    const gint   type    = dbus_message_get_type (message);
    const gchar *pathstr = dbus_message_get_path (message);

    /* Check for basic reasons not to handle */
    if (type   != DBUS_MESSAGE_TYPE_METHOD_CALL ||
        member == NULL ||
        iface  == NULL)
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    if (!strcmp (iface, "org.freedesktop.DBus.Properties"))
        return handle_properties (bus, message, path, iface, member, pathstr);

    if (!strcmp (iface, "org.freedesktop.DBus.Introspectable"))
        return handle_introspection (bus, message, path, iface, member, pathstr);

    return handle_other (bus, message, path, iface, member, pathstr);
}

/*---------------------------------------------------------------------------*/

DBusMessage *
droute_not_yet_handled_error (DBusMessage *message)
{
    DBusMessage *reply;
    gchar       *errmsg;

    errmsg= g_strdup_printf (
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

/*END------------------------------------------------------------------------*/
