/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008, Codethink Ltd.
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#include <config.h>
#include <dbus/dbus-glib-lowlevel.h>

#include "paths.h"
#include "registry.h"

enum
{
  REGISTRY_APPLICATION_REMOVE = 0,
  REGISTRY_APPLICATION_ADD = 1
};

/*---------------------------------------------------------------------------*/

G_DEFINE_TYPE(SpiRegistry, spi_registry, G_TYPE_OBJECT)

static void
spi_registry_class_init (SpiRegistryClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;

  spi_registry_parent_class = g_type_class_ref (G_TYPE_OBJECT);
}

static void
spi_registry_init (SpiRegistry *registry)
{
  registry->apps = g_sequence_new (g_free);
}

/*---------------------------------------------------------------------------*/

static void emit_update_applications(SpiRegistry *reg, guint sigtype, const char *app)
{
  DBusMessage *msg = NULL;
  DBusError error;

  dbus_error_init (&error);

  if ((msg = dbus_message_new_signal (SPI_DBUS_PATH_REGISTRY,
                                      SPI_DBUS_INTERFACE_REGISTRY,
                                      "UpdateApplications"))) {
    dbus_message_append_args(msg, DBUS_TYPE_INT32, &sigtype,
                                  DBUS_TYPE_STRING, &app, DBUS_TYPE_INVALID);

    dbus_connection_send (reg->bus, msg, NULL);

    dbus_message_unref (msg);
  }

  dbus_error_free (&error);
}


/*---------------------------------------------------------------------------*/

static gint
data_str_cmp (gpointer a, gpointer b, gpointer data)
{
  return g_strcmp0(a, b);
}

static gboolean
seq_add_string (GSequence *seq, gchar *str)
{
  GSequenceIter *iter;
  gchar *item;
  gboolean res = FALSE;

  iter = g_sequence_search (seq, str, (GCompareDataFunc) data_str_cmp, NULL);
  iter = g_sequence_iter_prev (iter);

  if (!g_sequence_iter_is_end (iter))
    {
      item = g_sequence_get (iter);
      if (g_strcmp0 (item, str))
        {
          g_sequence_insert_sorted (seq, g_strdup(str), (GCompareDataFunc) data_str_cmp, NULL);
          res = TRUE;
        }
    }
  else
    {
      g_sequence_insert_sorted (seq, g_strdup(str), (GCompareDataFunc) data_str_cmp, NULL);
      res = TRUE;
    }

  return res;
}

static gboolean
seq_remove_string (GSequence *seq, gchar *str)
{
  GSequenceIter *iter;
  gchar *item;
  gboolean res = FALSE;

  iter = g_sequence_search (seq, str, (GCompareDataFunc) data_str_cmp, NULL);
  iter = g_sequence_iter_prev (iter);

  if (!g_sequence_iter_is_end (iter))
    {
      item = g_sequence_get (iter);
      if (!g_strcmp0 (item, str))
        {
          g_sequence_remove (iter);
          res = TRUE;
        }
    }
  return res;
}

static void
add_application (DBusConnection *bus, SpiRegistry *reg, gchar *app)
{
  if (seq_add_string (reg->apps, app))
    {
      emit_update_applications (reg, REGISTRY_APPLICATION_ADD, app);
    }
}

static void
remove_application (DBusConnection *bus, SpiRegistry *reg, gchar *app)
{
  if (seq_remove_string (reg->apps, app))
    {
      /*TODO spi_remove_device_listeners (registry->de_controller, old);*/
      emit_update_applications (reg, REGISTRY_APPLICATION_REMOVE, app);
    }
}

/*---------------------------------------------------------------------------*/

static void
add_bus_name_cb (gpointer item, gpointer data)
{
  DBusMessageIter *iter_array = (DBusMessageIter *) data;

  dbus_message_iter_append_basic (iter_array, DBUS_TYPE_STRING, (gchar **) &item);
}

static DBusMessage *
impl_GetApplications (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  DBusMessage *reply = NULL;
  DBusMessageIter iter, iter_array;
  SpiRegistry *reg = SPI_REGISTRY (user_data);

  reply = dbus_message_new_method_return (message);

  dbus_message_iter_init_append (reply, &iter);
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "s", &iter_array);
  g_sequence_foreach (reg->apps, add_bus_name_cb, &iter_array);
  dbus_message_iter_close_container(&iter, &iter_array);
  return reply;
}

/*---------------------------------------------------------------------------*/

static DBusMessage*
impl_RegisterApplication (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  gchar *app_name;
  SpiRegistry *reg = SPI_REGISTRY (user_data);

  if (dbus_message_get_args (message, NULL, DBUS_TYPE_STRING, &app_name, DBUS_TYPE_INVALID))
      add_application(bus, reg, app_name);
  return NULL;
}

static DBusMessage*
impl_DeRegisterApplication (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  gchar *app_name;
  SpiRegistry *reg = SPI_REGISTRY (user_data);

  if (dbus_message_get_args (message, NULL, DBUS_TYPE_STRING, &app_name, DBUS_TYPE_INVALID))
      remove_application(bus, reg, app_name);
  return NULL;
}

/*---------------------------------------------------------------------------*/

static void
handle_disconnection (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  char *name, *old, *new;
  SpiRegistry *reg = SPI_REGISTRY (user_data);

  if (dbus_message_get_args (message, NULL,
                             DBUS_TYPE_STRING, &name,
                             DBUS_TYPE_STRING, &old,
                             DBUS_TYPE_STRING, &new,
                             DBUS_TYPE_INVALID))
    {
      if (*old != '\0' && *new == '\0')
        {
          remove_application(bus, reg, old);
        }
    }
}

/*---------------------------------------------------------------------------*/

static DBusHandlerResult
signal_filter (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  SpiRegistry *registry = SPI_REGISTRY (user_data);
  guint res = DBUS_HANDLER_RESULT_HANDLED;
  const char *iface = dbus_message_get_interface (message);
  const char *member = dbus_message_get_member (message);

  if (!g_strcmp0(iface, DBUS_INTERFACE_DBUS) && !g_strcmp0(member, "NameOwnerChanged"))
      handle_disconnection (bus, message, user_data);
  else
      res = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  return res;
}

/*---------------------------------------------------------------------------*/

static gchar *app_sig_match_name_owner =
       "type='signal', interface='org.freedesktop.DBus', member='NameOwnerChanged'";

static DRouteMethod dev_methods[] =
{
  { impl_GetApplications, "GetApplications" },
  { impl_RegisterApplication, "RegisterApplication" },
  { impl_DeRegisterApplication, "DeregisterApplication" },
  { NULL, NULL }
};

SpiRegistry *
spi_registry_new (DBusConnection *bus, DRouteContext *droute)
{
  SpiRegistry *reg = g_object_new (SPI_REGISTRY_TYPE, NULL);
  DRoutePath *path;

  reg->bus = bus;

  dbus_bus_add_match (bus, app_sig_match_name_owner, NULL);
  dbus_connection_add_filter (bus, signal_filter, reg, NULL);

  path = droute_add_one (droute,
                         SPI_DBUS_PATH_REGISTRY,
                         reg);

  droute_path_add_interface (path,
                             SPI_DBUS_INTERFACE_REGISTRY,
                             dev_methods,
                             NULL);

  return reg;
}

/*END------------------------------------------------------------------------*/
