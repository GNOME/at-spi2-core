/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008, 2010 Codethink Ltd.
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#include <config.h>
#include <string.h>
#include <ctype.h>

#include "paths.h"
#include "registry.h"
#include "introspection.h"

typedef struct
{
  gchar *listener_bus_name;
  gchar *app_bus_name;
  gchar **data;
  GSList *properties;
} EventData;

/*---------------------------------------------------------------------------*/

typedef struct
{
  gchar *name;
  gchar *path;
} SpiReference;

static SpiReference *
spi_reference_new (const gchar *name, const gchar *path)
{
  SpiReference *ref;

  ref = g_new0 (SpiReference, 1);
  ref->name = g_strdup (name);
  ref->path = g_strdup (path);

  return ref;
}

static void
spi_reference_free (SpiReference *ref)
{
  g_free (ref->name);
  g_free (ref->path);
  g_free (ref);
}

/*---------------------------------------------------------------------------*/

G_DEFINE_TYPE(SpiRegistry, spi_registry, G_TYPE_OBJECT)

static void
spi_registry_finalize (GObject *object)
{
  SpiRegistry *reg = SPI_REGISTRY (object);

  g_clear_pointer (&reg->bus_unique_name, g_free);

  G_OBJECT_CLASS (spi_registry_parent_class)->finalize (object);
}

static void
spi_registry_class_init (SpiRegistryClass *klass)
{
  GObjectClass *gobject_class;

  spi_registry_parent_class = g_type_class_ref (G_TYPE_OBJECT);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = spi_registry_finalize;
}

static void
spi_registry_init (SpiRegistry *registry)
{
  registry->apps = g_ptr_array_new_with_free_func ((GDestroyNotify) spi_reference_free);
}

/*---------------------------------------------------------------------------*/

static dbus_bool_t
return_v_string (DBusMessageIter * iter, const gchar * str)
{
  DBusMessageIter variant;

  if (!dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, "s",
                                    &variant))
    return FALSE;

  dbus_message_iter_append_basic (&variant, DBUS_TYPE_STRING, &str);
  dbus_message_iter_close_container (iter, &variant);
  return TRUE;
}

static dbus_bool_t
append_reference (DBusMessageIter * iter, const char * name, const char * path)
{
  DBusMessageIter iter_struct;

  if (!dbus_message_iter_open_container (iter, DBUS_TYPE_STRUCT, NULL,
                                    &iter_struct))
    return FALSE;
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &name);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path);
  dbus_message_iter_close_container (iter, &iter_struct);
  return TRUE;
}

/*---------------------------------------------------------------------------*/

static gboolean
compare_reference (SpiReference *one, SpiReference *two)
{
  if (g_strcmp0 (one->name, two->name) == 0 &&
      g_strcmp0 (one->path, two->path) == 0)
    return TRUE;
  else
    return FALSE;
}

static gboolean
find_index_of_reference (GPtrArray *arr, const gchar *name, const gchar * path, guint *index)
{
  SpiReference *ref;
  gboolean found = FALSE;
  guint i = 0;

  ref = spi_reference_new (name, path);

  for (i = 0; i < arr->len; i++)
    {
      if (compare_reference (ref, g_ptr_array_index (arr, i)))
        {
          found = TRUE;
          break;
        }
    }

  spi_reference_free (ref);

  *index = i;
  return found;
}

static void
emit_event (DBusConnection *bus,
            const char *iface_name,
            const char *signal_name,
            const char *detail_str,
            dbus_int32_t detail1,
            dbus_int32_t detail2,
            SpiReference *app)
{
  DBusMessage *sig;
  DBusMessageIter iter, iter_variant, iter_array;

  sig = dbus_message_new_signal(SPI_DBUS_PATH_ROOT, iface_name, signal_name);

  dbus_message_iter_init_append(sig, &iter);

  dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &detail_str);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &detail1);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &detail2);

  dbus_message_iter_open_container (&iter, DBUS_TYPE_VARIANT, "(so)",
                                    &iter_variant);
    append_reference (&iter_variant, app->name, app->path);
  dbus_message_iter_close_container (&iter, &iter_variant);

  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "{sv}",
                                    &iter_array);
  dbus_message_iter_close_container (&iter, &iter_array);

  dbus_connection_send(bus, sig, NULL);
  dbus_message_unref(sig);
}

static void
add_application (SpiRegistry *reg, SpiReference *app_root)
{
  gint index;

  g_ptr_array_add (reg->apps, app_root);
  index = reg->apps->len - 1;

  emit_event (reg->bus, SPI_DBUS_INTERFACE_EVENT_OBJECT, "ChildrenChanged", "add", index, 0, app_root);
}

static void
set_id (SpiRegistry *reg, const gchar *name, const gchar *path)
{
  DBusMessage *message;
  DBusMessageIter iter, iter_variant;
  const char *iface_application = "org.a11y.atspi.Application";
  const char *id = "Id";

  message = dbus_message_new_method_call (name, path,
                                          DBUS_INTERFACE_PROPERTIES, "Set");
  if (!message)
    return;
  dbus_message_iter_init_append (message, &iter);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &iface_application);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &id);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_VARIANT, "i", &iter_variant);
  dbus_message_iter_append_basic (&iter_variant, DBUS_TYPE_INT32, &reg->id);
  /* TODO: This will cause problems if we cycle through 2^31 ids */
  reg->id++;
  dbus_message_iter_close_container (&iter, &iter_variant);
  dbus_connection_send (reg->bus, message, NULL);
  dbus_message_unref (message);
}

static void
remove_application (SpiRegistry *reg, guint index)
{
  SpiReference *ref = g_ptr_array_index (reg->apps, index);

  spi_remove_device_listeners (reg->dec, ref->name);
  emit_event (reg->bus, SPI_DBUS_INTERFACE_EVENT_OBJECT, "ChildrenChanged", "remove", index, 0, ref);
  g_ptr_array_remove_index (reg->apps, index);
}

static gboolean
event_is_subtype (gchar **needle, gchar **haystack)
{
  while (*haystack && **haystack)
    {
      if (g_strcmp0 (*needle, *haystack))
        return FALSE;
      needle++;
      haystack++;
    }
  return TRUE;
}

static gboolean
needs_mouse_poll (char **event)
{
  if (g_strcmp0 (event [0], "Mouse") != 0)
    return FALSE;
  if (!event [1] || !event [1][0])
    return TRUE;
  return (g_strcmp0 (event [1], "Abs") == 0);
}

static void
remove_events (SpiRegistry *registry, const char *bus_name, const char *event)
{
  gchar **remove_data;
  GList *list;
  gboolean mouse_found = FALSE;
  DBusMessage *signal;

  remove_data = g_strsplit (event, ":", 3);
  if (!remove_data)
    {
      return;
    }

  for (list = registry->events; list;)
    {
      EventData *evdata = list->data;
      list = list->next;
      if (!g_strcmp0 (evdata->listener_bus_name, bus_name) &&
          event_is_subtype (evdata->data, remove_data))
        {
          g_strfreev (evdata->data);
          g_free (evdata->listener_bus_name);
          g_free (evdata->app_bus_name);
          g_slist_free_full (evdata->properties, g_free);
          registry->events = g_list_remove (registry->events, evdata);
          g_free (evdata);
        }
      else
        {
          if (needs_mouse_poll (evdata->data))
            mouse_found = TRUE;
        }
    }

  if (!mouse_found)
    spi_device_event_controller_stop_poll_mouse ();

  g_strfreev (remove_data);

  signal = dbus_message_new_signal (SPI_DBUS_PATH_REGISTRY,
                                    SPI_DBUS_INTERFACE_REGISTRY,
                                    "EventListenerDeregistered");
  dbus_message_append_args (signal, DBUS_TYPE_STRING, &bus_name,
                            DBUS_TYPE_STRING, &event, DBUS_TYPE_INVALID);
  dbus_connection_send (registry->bus, signal, NULL);
  dbus_message_unref (signal);
}

static void
handle_disconnection (SpiRegistry *reg, DBusMessage *message)
{
  char *name, *old, *new;

  if (dbus_message_get_args (message, NULL,
                             DBUS_TYPE_STRING, &name,
                             DBUS_TYPE_STRING, &old,
                             DBUS_TYPE_STRING, &new,
                             DBUS_TYPE_INVALID))
    {
      if (*old != '\0' && *new == '\0')
        {
          /* Remove all children with the application name the same as the disconnected application. */
          guint i;
          for (i = 0; i < reg->apps->len; i++)
            {
              SpiReference *ref  = g_ptr_array_index (reg->apps, i);
              if (!g_strcmp0 (old, ref->name))
                {
                  remove_application (reg, i);
                  i--;
                }
            }

          remove_events (reg, old, "");
        }
    }
}

/*
 * Converts names of the form "active-descendant-changed" to
 *" ActiveDescendantChanged"
 */
static gchar *
ensure_proper_format (const char *name)
{
  gchar *ret = (gchar *) g_malloc (strlen (name) * 2 + 2);
  gchar *p = ret;
  gboolean need_upper = TRUE;

  if (!ret)
    return NULL;
  while (*name)
    {
      if (need_upper)
        {
          *p++ = toupper (*name);
          need_upper = FALSE;
        }
      else if (*name == '-')
        need_upper = TRUE;
      else if (*name == ':')
        {
          need_upper = TRUE;
          *p++ = *name;
        }
      else
        *p++ = *name;
      name++;
    }
  *p = '\0';
  return ret;
}

static DBusHandlerResult
signal_filter (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  SpiRegistry *reg = SPI_REGISTRY (user_data);
  guint res = DBUS_HANDLER_RESULT_HANDLED;

  const gint   type    = dbus_message_get_type (message);
  const char *iface = dbus_message_get_interface (message);
  const char *member = dbus_message_get_member (message);

  if (type != DBUS_MESSAGE_TYPE_SIGNAL)
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  if (!g_strcmp0(iface, DBUS_INTERFACE_DBUS) &&
      !g_strcmp0(member, "NameOwnerChanged"))
      handle_disconnection (reg, message);
  else
      res = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  return res;
}

/* org.at_spi.Socket interface */
/*---------------------------------------------------------------------------*/

static DBusMessage*
impl_Embed (DBusMessage *message, SpiRegistry *registry)
{
  DBusMessageIter iter, iter_struct;
  const gchar *app_name, *obj_path;

  DBusMessage *reply = NULL;
  DBusMessageIter reply_iter;
  SpiReference *app_root;

  dbus_message_iter_init (message, &iter);
  dbus_message_iter_recurse (&iter, &iter_struct);
  if (!(dbus_message_iter_get_arg_type (&iter_struct) == DBUS_TYPE_STRING))
	goto error;
  dbus_message_iter_get_basic (&iter_struct, &app_name);
  if (!app_name)
    app_name = dbus_message_get_sender (message);
  if (!dbus_message_iter_next (&iter_struct))
        goto error;
  if (!(dbus_message_iter_get_arg_type (&iter_struct) == DBUS_TYPE_OBJECT_PATH))
	goto error;
  dbus_message_iter_get_basic (&iter_struct, &obj_path);

  app_root = spi_reference_new (app_name, obj_path);
  add_application (registry, app_root);

  set_id (registry, app_name, obj_path);

  reply = dbus_message_new_method_return (message);
  dbus_message_iter_init_append (reply, &reply_iter);
  append_reference (&reply_iter, 
                    registry->bus_unique_name,
                    SPI_DBUS_PATH_ROOT);

  return reply;
error:
  return dbus_message_new_error (message, DBUS_ERROR_FAILED, "Invalid arguments");
}

static DBusMessage*
impl_Unembed (DBusMessage *message, SpiRegistry *registry)
{
  DBusMessageIter iter, iter_struct;
  gchar *app_name, *obj_path;
  guint index;

  dbus_message_iter_init (message, &iter);
  dbus_message_iter_recurse (&iter, &iter_struct);
  if (!(dbus_message_iter_get_arg_type (&iter_struct) == DBUS_TYPE_STRING))
	goto error;
  dbus_message_iter_get_basic (&iter_struct, &app_name);
  if (!dbus_message_iter_next (&iter_struct))
        goto error;
  if (!(dbus_message_iter_get_arg_type (&iter_struct) == DBUS_TYPE_OBJECT_PATH))
	goto error;
  dbus_message_iter_get_basic (&iter_struct, &obj_path);

  if (find_index_of_reference (registry->apps, app_name, obj_path, &index))
    remove_application (registry, index);

  return NULL;
error:
  return dbus_message_new_error (message, DBUS_ERROR_FAILED, "Invalid arguments");
}

/* org.at_spi.Component interface */
/*---------------------------------------------------------------------------*/

static DBusMessage *
impl_Contains (DBusMessage * message, SpiRegistry *registry)
{
  dbus_bool_t retval = FALSE;
  DBusMessage *reply;

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &retval,
                            DBUS_TYPE_INVALID);
  return reply;
}

static DBusMessage *
impl_GetAccessibleAtPoint (DBusMessage * message, SpiRegistry *registry)
{
  DBusMessage *reply = NULL;
  DBusMessageIter iter;

  reply = dbus_message_new_method_return (message);
  dbus_message_iter_init_append (reply, &iter);
  append_reference (&iter, 
                    registry->bus_unique_name,
                    SPI_DBUS_PATH_NULL);

  return reply;
}

static DBusMessage *
impl_GetExtents (DBusMessage * message, SpiRegistry *registry)
{
  dbus_int32_t x = 0, y = 0, width = 1024, height = 768;
  DBusMessage *reply;
  DBusMessageIter iter, iter_struct;

  reply = dbus_message_new_method_return (message);
  dbus_message_iter_init_append (reply, &iter);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_STRUCT, NULL,
                                    &iter_struct);
    dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, &x);
    dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, &y);
    dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, &width);
    dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, &height);
  dbus_message_iter_close_container (&iter, &iter_struct);
  return reply;
}

static DBusMessage *
impl_GetPosition (DBusMessage * message, SpiRegistry *registry)
{
  DBusMessage *reply;
  dbus_int32_t x = 0, y = 0;

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32,
                            &y, DBUS_TYPE_INVALID);
  return reply;
}

static DBusMessage *
impl_GetSize (DBusMessage * message, SpiRegistry *registry)
{
  /* TODO - Get the screen size */
  DBusMessage *reply;
  dbus_int32_t width = 1024, height = 768;

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_INT32, &width,
                            DBUS_TYPE_INT32, &height, DBUS_TYPE_INVALID);
  return reply;
}

#define LAYER_WIDGET 3;

static DBusMessage *
impl_GetLayer (DBusMessage * message, SpiRegistry *registry)
{
  DBusMessage *reply;
  dbus_uint32_t rv = LAYER_WIDGET;

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_UINT32, &rv,
                            DBUS_TYPE_INVALID);
  return reply;
}

static DBusMessage *
impl_GetMDIZOrder (DBusMessage * message, SpiRegistry *registry)
{
  DBusMessage *reply;
  dbus_int16_t rv = 0;

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_INT16, &rv,
                            DBUS_TYPE_INVALID);
  return reply;
}

static DBusMessage *
impl_GrabFocus (DBusMessage * message, SpiRegistry *registry)
{
  DBusMessage *reply;
  dbus_bool_t retval = FALSE;

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &retval,
                            DBUS_TYPE_INVALID);
  return reply;
}

static DBusMessage *
impl_GetAlpha (DBusMessage * message, SpiRegistry *registry)
{
  double rv = 1.0;
  DBusMessage *reply;

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_DOUBLE, &rv,
                            DBUS_TYPE_INVALID);
  return reply;
}

/* org.at_spi.Accessible interface */
/*---------------------------------------------------------------------------*/

static dbus_bool_t
impl_get_Name (DBusMessageIter * iter, SpiRegistry *registry)
{
  const gchar *name = "main";
  return return_v_string (iter, name);
}

static dbus_bool_t
impl_get_Description (DBusMessageIter * iter, SpiRegistry *registry)
{
  const gchar *description = "";
  return return_v_string (iter, description);
}

static dbus_bool_t
impl_get_Parent (DBusMessageIter * iter, SpiRegistry *registry)
{
  const gchar *name = "";
  DBusMessageIter iter_variant;

  dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, "(so)",
                                    &iter_variant);
  append_reference (&iter_variant, 
                    name,
                    SPI_DBUS_PATH_NULL);
  dbus_message_iter_close_container (iter, &iter_variant);
  return TRUE;
}

static dbus_bool_t
impl_get_ChildCount (DBusMessageIter * iter, SpiRegistry *registry)
{
  dbus_int32_t rv = registry->apps->len;
  dbus_bool_t result;
  DBusMessageIter iter_variant;

  if (!dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, "i",
                                         &iter_variant))
    return FALSE;
  result = dbus_message_iter_append_basic (&iter_variant, DBUS_TYPE_INT32, &rv);
  dbus_message_iter_close_container (iter, &iter_variant);
  return result;
}

static dbus_bool_t
impl_get_ToolkitName (DBusMessageIter * iter, SpiRegistry *registry)
{
  return return_v_string (iter, "at-spi-registry");
}

static dbus_bool_t
impl_get_ToolkitVersion (DBusMessageIter * iter, SpiRegistry *registry)
{
  return return_v_string (iter, "2.0");
}

static DBusMessage *
impl_GetChildAtIndex (DBusMessage * message, SpiRegistry *registry)
{
  DBusMessage *reply;
  DBusMessageIter iter;
  DBusError error;
  SpiReference *ref;
  dbus_int32_t i;

  dbus_error_init (&error);
  if (!dbus_message_get_args 
       (message, &error, DBUS_TYPE_INT32, &i, DBUS_TYPE_INVALID))
    {
      return dbus_message_new_error (message, DBUS_ERROR_FAILED, "Invalid arguments");
    }

  reply = dbus_message_new_method_return (message);
  dbus_message_iter_init_append (reply, &iter);

  if (i < 0 || i >= registry->apps->len)
    append_reference (&iter, SPI_DBUS_NAME_REGISTRY, SPI_DBUS_PATH_NULL);
  else
    {
      ref = g_ptr_array_index (registry->apps, i);
      append_reference (&iter, ref->name, ref->path);
    }

  return reply;
}

static DBusMessage *
impl_GetChildren (DBusMessage * message, SpiRegistry *registry)
{
  DBusMessage *reply = NULL;
  DBusMessageIter iter, iter_array;
  int i;

  reply = dbus_message_new_method_return (message);

  dbus_message_iter_init_append (reply, &iter);
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "(so)", &iter_array);
  for (i=0; i < registry->apps->len; i++)
    {
      SpiReference *current = g_ptr_array_index (registry->apps, i);
      append_reference (&iter_array, current->name, current->path);
    }
  dbus_message_iter_close_container(&iter, &iter_array);
  return reply;
}

static DBusMessage *
impl_GetIndexInParent (DBusMessage * message, SpiRegistry *registry)
{
  DBusMessage *reply;
  dbus_uint32_t rv = 0;

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_INT32, &rv, DBUS_TYPE_INVALID);
  return reply;
}

static DBusMessage *
impl_GetRelationSet (DBusMessage * message, SpiRegistry *registry)
{
  DBusMessage *reply;
  DBusMessageIter iter, iter_array;

  reply = dbus_message_new_method_return (message);
  dbus_message_iter_init_append (reply, &iter);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "(ua(so))", &iter_array);
  dbus_message_iter_close_container (&iter, &iter_array);

  return reply;
}

static DBusMessage *
impl_GetRole (DBusMessage * message, SpiRegistry *registry)
{
  DBusMessage *reply;
  dbus_uint32_t rv = 14;	/* TODO: Get DESKTOP_FRAME from somewhere */

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_UINT32, &rv, DBUS_TYPE_INVALID);
  return reply;
}

static DBusMessage *
impl_GetRoleName (DBusMessage * message, SpiRegistry *registry)
{
  DBusMessage *reply;
  const char *role_name = "desktop frame";

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_STRING, &role_name,
                            DBUS_TYPE_INVALID);
  return reply;
}

static DBusMessage *
impl_GetLocalizedRoleName (DBusMessage * message, SpiRegistry *registry)
{
  /* TODO - Localize this */
  DBusMessage *reply;
  const char *role_name = "desktop frame";

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_STRING, &role_name,
                            DBUS_TYPE_INVALID);
  return reply;
}

static DBusMessage *
impl_GetState (DBusMessage * message, SpiRegistry *registry)
{
  DBusMessage *reply = NULL;
  DBusMessageIter iter, iter_array;

  dbus_uint32_t states[2] = {0, 0};
  guint count;

  reply = dbus_message_new_method_return (message);
  dbus_message_iter_init_append (reply, &iter);

  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "u", &iter_array);
  for (count = 0; count < 2; count++)
    {
      dbus_message_iter_append_basic (&iter_array, DBUS_TYPE_UINT32,
                                      &states[count]);
    }
  dbus_message_iter_close_container (&iter, &iter_array);
  return reply;
}

static DBusMessage *
impl_GetAttributes (DBusMessage * message, SpiRegistry *registry)
{
  DBusMessage *reply = NULL;
  DBusMessageIter iter, array;

  reply = dbus_message_new_method_return (message);
  dbus_message_iter_init_append (reply, &iter);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "{ss}", &array);
  dbus_message_iter_close_container (&iter, &array);

  return reply;
}

static DBusMessage *
impl_GetApplication (DBusMessage * message, SpiRegistry *registry)
{
  DBusMessage *reply = NULL;
  DBusMessageIter iter;

  reply = dbus_message_new_method_return (message);
  dbus_message_iter_init_append (reply, &iter);
  append_reference (&iter,
                    registry->bus_unique_name,
                    SPI_DBUS_PATH_NULL);

  return reply;
}

static DBusMessage *
impl_GetInterfaces (DBusMessage * message, SpiRegistry *registry)
{
  DBusMessage *reply;
  DBusMessageIter iter, iter_array;

  const char *acc = SPI_DBUS_INTERFACE_ACCESSIBLE;
  const char *com = SPI_DBUS_INTERFACE_COMPONENT;

  reply = dbus_message_new_method_return (message);

  dbus_message_iter_init_append (reply, &iter);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "s",
                                    &iter_array);
    dbus_message_iter_append_basic (&iter_array, DBUS_TYPE_STRING, &acc);
    dbus_message_iter_append_basic (&iter_array, DBUS_TYPE_STRING, &com);
  dbus_message_iter_close_container (&iter, &iter_array);

  return reply;
}

static DBusMessage *
impl_GetItems (DBusMessage * message, SpiRegistry *registry)
{
  DBusMessage *reply;
  DBusMessageIter iter, iter_array;

  reply = dbus_message_new_method_return (message);

  dbus_message_iter_init_append (reply, &iter);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY,
                                    "((so)(so)(so)a(so)assusau)", &iter_array);
  dbus_message_iter_close_container (&iter, &iter_array);
  return reply;
}

/* I would rather these two be signals, but I'm not sure that dbus-python
 * supports emitting signals except for a service, so implementing as both
 * a method call and signal for now.
 */
static DBusMessage *
impl_RegisterEvent (DBusMessage *message, SpiRegistry *registry)
{
  const char *orig_name;
  gchar *name;
  EventData *evdata;
  gchar **data;
  DBusMessage *signal;
  const char *sender = dbus_message_get_sender (message);
  DBusMessageIter iter, iter_array;
  const char *signature = dbus_message_get_signature (message);

  if (strcmp (signature, "sas") != 0 &&
      strcmp (signature, "s") != 0 &&
      strcmp (signature, "sass") != 0)
  {
    g_warning ("got RegisterEvent with invalid signature '%s'", signature);
    return NULL;
  }

  dbus_message_iter_init (message, &iter);
  dbus_message_iter_get_basic (&iter, &orig_name);
  dbus_message_iter_next (&iter);
  name = ensure_proper_format (orig_name);

  evdata = g_new0 (EventData, 1);
  data = g_strsplit (name, ":", 3);
  evdata->listener_bus_name = g_strdup (sender);
  evdata->data = data;

  if (dbus_message_iter_get_arg_type (&iter) == DBUS_TYPE_ARRAY)
  {
    dbus_message_iter_recurse (&iter, &iter_array);
    while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
    {
      const char *property;
      dbus_message_iter_get_basic (&iter_array, &property);
      evdata->properties = g_slist_append (evdata->properties,
                                           g_strdup (property));
      dbus_message_iter_next (&iter_array);
    }
    dbus_message_iter_next (&iter);
  }

  if (dbus_message_iter_get_arg_type (&iter) == DBUS_TYPE_STRING)
  {
    const char *app;
    dbus_message_iter_get_basic (&iter, &app);
    if (app[0])
      evdata->app_bus_name = g_strdup (app);
    dbus_message_iter_next (&iter);
  }

  registry->events = g_list_append (registry->events, evdata);

  if (needs_mouse_poll (evdata->data))
    {
      spi_device_event_controller_start_poll_mouse (registry);
    }

  signal = dbus_message_new_signal (SPI_DBUS_PATH_REGISTRY,
                                    SPI_DBUS_INTERFACE_REGISTRY,
                                    "EventListenerRegistered");
  if (signal)
  {
    GSList *ls = evdata->properties;
    if (evdata->app_bus_name)
      dbus_message_set_destination (message, evdata->app_bus_name);
    dbus_message_iter_init_append (signal, &iter);
    dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &sender);
    dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &name);
    dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "s", &iter_array);
    while (ls)
    {
      dbus_message_iter_append_basic (&iter_array, DBUS_TYPE_STRING, &ls->data);
      ls = g_slist_next (ls);
    }
    dbus_message_iter_close_container (&iter, &iter_array);
    dbus_connection_send (registry->bus, signal, NULL);
    dbus_message_unref (signal);
  }

  g_free (name);
  return dbus_message_new_method_return (message);
}

static DBusMessage *
impl_DeregisterEvent (DBusMessage *message, SpiRegistry *registry)
{
  const char *orig_name;
  gchar *name;
  const char *sender = dbus_message_get_sender (message);

  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_STRING, &orig_name,
    DBUS_TYPE_INVALID))
    return NULL;
  name = ensure_proper_format (orig_name);

  remove_events (registry, sender, name);

  g_free (name);
  return dbus_message_new_method_return (message);
}

static DBusMessage *
impl_GetRegisteredEvents (DBusMessage *message, SpiRegistry *registry)
{
  EventData *evdata;
  DBusMessage *reply;
  DBusMessageIter iter, iter_struct, iter_array;
  GList *list;
  const char *sender = dbus_message_get_sender (message);

  reply = dbus_message_new_method_return (message);
  if (!reply)
    return NULL;

  dbus_message_iter_init_append (reply, &iter);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "(ss)", &iter_array);
  for (list = registry->events; list; list = list->next)
    {
      gchar *str;
      evdata = list->data;
      if (evdata->app_bus_name && strcmp (evdata->app_bus_name, sender) != 0)
        continue;

      str = g_strconcat (evdata->data [0],
                         ":", (evdata->data [1]? evdata->data [1]: ""),
                         ":", (evdata->data [1] && evdata->data [2]? evdata->data [2]: ""), NULL);
      dbus_message_iter_open_container (&iter_array, DBUS_TYPE_STRUCT, NULL, &iter_struct);
      dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &evdata->listener_bus_name);
      dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &str);
      dbus_message_iter_close_container (&iter_array, &iter_struct);
      g_free (str);
    }
  dbus_message_iter_close_container (&iter, &iter_array);
  return reply;
}

/*---------------------------------------------------------------------------*/

static void 
emit_Available (DBusConnection * bus)
{
  DBusMessage *sig;
  DBusMessageIter iter;
  
  sig = dbus_message_new_signal(SPI_DBUS_PATH_ROOT, SPI_DBUS_INTERFACE_SOCKET, "Available");

  dbus_message_iter_init_append(sig, &iter);
  append_reference (&iter, SPI_DBUS_NAME_REGISTRY, SPI_DBUS_PATH_ROOT);

  dbus_connection_send(bus, sig, NULL);
  dbus_message_unref(sig);
}

/*---------------------------------------------------------------------------*/

static const char *introspection_header =
"<?xml version=\"1.0\"?>\n";

static const char *introspection_node_element =
"<node name=\"%s\">\n";

static const char *introspection_footer =
"</node>";

static DBusMessage *
impl_Introspect_root (DBusMessage * message, SpiRegistry *registry)
{
  GString *output;
  gchar *final;

  const gchar *pathstr = SPI_DBUS_PATH_ROOT;

  DBusMessage *reply;

  output = g_string_new(introspection_header);

  g_string_append_printf(output, introspection_node_element, pathstr);

  g_string_append (output, spi_org_a11y_atspi_Accessible);
  g_string_append (output, spi_org_a11y_atspi_Component);

  g_string_append(output, introspection_footer);
  final = g_string_free(output, FALSE);

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args(reply, DBUS_TYPE_STRING, &final, DBUS_TYPE_INVALID);

  g_free(final);
  return reply;
}

static DBusMessage *
impl_Introspect_registry (DBusMessage * message, SpiRegistry *registry)
{
  GString *output;
  gchar *final;

  const gchar *pathstr = SPI_DBUS_PATH_REGISTRY;

  DBusMessage *reply;

  output = g_string_new(introspection_header);

  g_string_append_printf(output, introspection_node_element, pathstr);

  g_string_append (output, spi_org_a11y_atspi_Registry);

  g_string_append(output, introspection_footer);
  final = g_string_free(output, FALSE);

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args(reply, DBUS_TYPE_STRING, &final, DBUS_TYPE_INVALID);

  g_free(final);
  return reply;
}

/*---------------------------------------------------------------------------*/

static DBusHandlerResult
handle_method_root (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  SpiRegistry *registry = SPI_REGISTRY (user_data);
  DBusHandlerResult result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  const gchar *iface   = dbus_message_get_interface (message);
  const gchar *member  = dbus_message_get_member (message);
  const gint   type    = dbus_message_get_type (message);

  DBusMessage *reply = NULL;

  /* Check for basic reasons not to handle */
  if (type   != DBUS_MESSAGE_TYPE_METHOD_CALL ||
      member == NULL ||
      iface  == NULL)
      return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  if (!strcmp (iface, "org.freedesktop.DBus.Properties"))
    {
      if (!strcmp (member, "Get"))
        {
          const gchar *prop_iface;
          const gchar *prop_member;
          DBusError error;

          dbus_error_init (&error);
          if (dbus_message_get_args (message,
                                     &error,
                                     DBUS_TYPE_STRING,
                                     &prop_iface,
                                     DBUS_TYPE_STRING,
                                     &prop_member,
                                     DBUS_TYPE_INVALID))
            {
              DBusMessageIter iter;

              reply = dbus_message_new_method_return (message);
              dbus_message_iter_init_append (reply, &iter);


              if (!strcmp (prop_iface, SPI_DBUS_INTERFACE_ACCESSIBLE))
                {
                  if      (!strcmp (prop_member, "Name"))
                    impl_get_Name (&iter, registry);
                  else if (!strcmp (prop_member, "Description"))
                    impl_get_Description (&iter, registry);
                  else if (!strcmp (prop_member, "Parent"))
                    impl_get_Parent (&iter, registry);
                  else if (!strcmp (prop_member, "ChildCount"))
                    impl_get_ChildCount (&iter, registry);
                  else
                    {
                      dbus_message_unref (reply); 
                      reply = dbus_message_new_error (message, DBUS_ERROR_FAILED, "Property unavailable");
                    }
                }
              else if (!strcmp (prop_iface, SPI_DBUS_INTERFACE_APPLICATION))
                {
                  if (!strcmp (prop_member, "ToolkitName"))
                    impl_get_ToolkitName (&iter, registry);
                  else if (!strcmp (prop_member, "ToolkitVersion"))
                    impl_get_ToolkitVersion (&iter, registry);
                  else
                    {
                      dbus_message_unref (reply); 
                      reply = dbus_message_new_error (message, DBUS_ERROR_FAILED, "Property unavailable");
                    }
                }
              else
                {
                  dbus_message_unref (reply); 
                  reply = dbus_message_new_error (message, DBUS_ERROR_FAILED, "Property unavailable");
                }
            }
          else
            {
              reply = dbus_message_new_error (message, DBUS_ERROR_FAILED, error.message);
            }
          result = DBUS_HANDLER_RESULT_HANDLED;
        }
      else if (!strcmp (member, "GetAll"))
        {
          result = DBUS_HANDLER_RESULT_HANDLED;
        }
    }

  if (!strcmp (iface, SPI_DBUS_INTERFACE_ACCESSIBLE))
    {
      result = DBUS_HANDLER_RESULT_HANDLED;
      if      (!strcmp (member, "GetChildAtIndex"))
          reply = impl_GetChildAtIndex (message, registry);
      else if (!strcmp (member, "GetChildren"))
          reply = impl_GetChildren (message, registry);
      else if (!strcmp (member, "GetIndexInParent"))
          reply = impl_GetIndexInParent (message, registry);
      else if (!strcmp (member, "GetRelationSet"))
          reply = impl_GetRelationSet (message, registry);
      else if (!strcmp (member, "GetRole"))
          reply = impl_GetRole (message, registry);
      else if (!strcmp (member, "GetRoleName"))
          reply = impl_GetRoleName (message, registry);
      else if (!strcmp (member, "GetLocalizedRoleName"))
          reply = impl_GetLocalizedRoleName (message, registry);
      else if (!strcmp (member, "GetState"))
          reply = impl_GetState (message, registry);
      else if (!strcmp (member, "GetAttributes"))
          reply = impl_GetAttributes (message, registry);
      else if (!strcmp (member, "GetApplication"))
          reply = impl_GetApplication (message, registry);
      else if (!strcmp (member, "GetInterfaces"))
          reply = impl_GetInterfaces (message, registry);
      else
         result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

  if (!strcmp (iface, SPI_DBUS_INTERFACE_COMPONENT))
    {
      result = DBUS_HANDLER_RESULT_HANDLED;
      if      (!strcmp (member, "Contains"))
          reply = impl_Contains (message, registry);
      else if (!strcmp (member, "GetAccessibleAtPoint"))
          reply = impl_GetAccessibleAtPoint (message, registry);
      else if (!strcmp (member, "GetExtents"))
          reply = impl_GetExtents (message, registry);
      else if (!strcmp (member, "GetPosition"))
          reply = impl_GetPosition (message, registry);
      else if (!strcmp (member, "GetSize"))
          reply = impl_GetSize (message, registry);
      else if (!strcmp (member, "GetLayer"))
          reply = impl_GetLayer (message, registry);
      else if (!strcmp (member, "GetMDIZOrder"))
          reply = impl_GetMDIZOrder (message, registry);
      else if (!strcmp (member, "GrabFocus"))
          reply = impl_GrabFocus (message, registry);
      else if (!strcmp (member, "GetAlpha"))
          reply = impl_GetAlpha (message, registry);
      else
         result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

  if (!strcmp (iface, SPI_DBUS_INTERFACE_SOCKET))
    {
      result = DBUS_HANDLER_RESULT_HANDLED;
      if      (!strcmp (member, "Embed"))
          reply = impl_Embed (message, registry);
      else if (!strcmp (member, "Unembed"))
          reply = impl_Unembed (message, registry);
      else
          result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

  if (!strcmp (iface, "org.freedesktop.DBus.Introspectable"))
    {
      result = DBUS_HANDLER_RESULT_HANDLED;
      if      (!strcmp (member, "Introspect"))
          reply = impl_Introspect_root (message, registry);
      else
          result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

  if (result == DBUS_HANDLER_RESULT_HANDLED)
    {
      if (!reply)
        {
          reply = dbus_message_new_method_return (message);
        }

      dbus_connection_send (bus, reply, NULL);
      dbus_message_unref (reply);
    }
#if 0
  else
    {
      g_print ("Registry | Unhandled message : %s|%s\n", iface, member);
    }
#endif
  
  return result;
}

static DBusHandlerResult
handle_method_cache (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  SpiRegistry *registry = SPI_REGISTRY (user_data);
  DBusHandlerResult result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  const gchar *iface   = dbus_message_get_interface (message);
  const gchar *member  = dbus_message_get_member (message);
  const gint   type    = dbus_message_get_type (message);

  DBusMessage *reply = NULL;

  /* Check for basic reasons not to handle */
  if (type   != DBUS_MESSAGE_TYPE_METHOD_CALL ||
      member == NULL ||
      iface  == NULL)
      return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  if (!strcmp (iface, SPI_DBUS_INTERFACE_CACHE))
    {
      result = DBUS_HANDLER_RESULT_HANDLED;
      if      (!strcmp (member, "GetItems"))
          reply = impl_GetItems (message, registry);
      else
         result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

  if (result == DBUS_HANDLER_RESULT_HANDLED)
    {
      if (!reply)
        {
          reply = dbus_message_new_method_return (message);
        }

      dbus_connection_send (bus, reply, NULL);
      dbus_message_unref (reply);
    }
  return result;
}

static DBusHandlerResult
handle_method_registry (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  SpiRegistry *registry = SPI_REGISTRY (user_data);
  DBusHandlerResult result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  const gchar *iface   = dbus_message_get_interface (message);
  const gchar *member  = dbus_message_get_member (message);
  const gint   type    = dbus_message_get_type (message);

  DBusMessage *reply = NULL;

  /* Check for basic reasons not to handle */
  if (type   != DBUS_MESSAGE_TYPE_METHOD_CALL ||
      member == NULL ||
      iface  == NULL)
      return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  if (!strcmp (iface, SPI_DBUS_INTERFACE_REGISTRY))
    {
      result = DBUS_HANDLER_RESULT_HANDLED;
      if (!strcmp(member, "RegisterEvent"))
      reply = impl_RegisterEvent (message, registry);
      else if (!strcmp(member, "DeregisterEvent"))
        reply = impl_DeregisterEvent (message, registry);
      else if (!strcmp(member, "GetRegisteredEvents"))
        reply = impl_GetRegisteredEvents (message, registry);
      else
          result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

  if (!strcmp (iface, "org.freedesktop.DBus.Introspectable"))
    {
      result = DBUS_HANDLER_RESULT_HANDLED;
      if      (!strcmp (member, "Introspect"))
          reply = impl_Introspect_registry (message, registry);
      else
          result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

  if (result == DBUS_HANDLER_RESULT_HANDLED)
    {
      if (!reply)
        {
          reply = dbus_message_new_method_return (message);
        }

      dbus_connection_send (bus, reply, NULL);
      dbus_message_unref (reply);
    }
#if 0
  else
    {
      g_print ("Registry | Unhandled message : %s|%s\n", iface, member);
    }
#endif
  
  return result;
}

/*---------------------------------------------------------------------------*/

static DBusObjectPathVTable root_vtable =
{
  NULL,
  &handle_method_root,
  NULL, NULL, NULL, NULL
};

static DBusObjectPathVTable registry_vtable =
{
  NULL,
  &handle_method_registry,
  NULL, NULL, NULL, NULL
};

static DBusObjectPathVTable cache_vtable =
{
  NULL,
  &handle_method_cache,
  NULL, NULL, NULL, NULL
};

static gchar *app_sig_match_name_owner =
       "type='signal', interface='org.freedesktop.DBus', member='NameOwnerChanged'";

SpiRegistry *
spi_registry_new (DBusConnection *bus)
{
  SpiRegistry *reg = g_object_new (SPI_REGISTRY_TYPE, NULL);
  const char *bus_unique_name;

  bus_unique_name = dbus_bus_get_unique_name (bus);
  g_assert (bus_unique_name != NULL);

  reg->bus = bus;
  reg->bus_unique_name = g_strdup (bus_unique_name);

  dbus_bus_add_match (bus, app_sig_match_name_owner, NULL);
  dbus_connection_add_filter (bus, signal_filter, reg, NULL);

  dbus_connection_register_object_path (bus, SPI_DBUS_PATH_ROOT, &root_vtable, reg);

  dbus_connection_register_object_path (bus, SPI_DBUS_PATH_CACHE, &cache_vtable, reg);

  dbus_connection_register_object_path (bus, SPI_DBUS_PATH_REGISTRY, &registry_vtable, reg);

  emit_Available (bus);

  reg->events = NULL;

  return reg;
}

/*END------------------------------------------------------------------------*/
