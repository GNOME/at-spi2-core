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
#include <spi-common/spi-dbus.h>
#include <dbus/dbus-glib-lowlevel.h>

#include "registry.h"

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

static void emit(SpiRegistry *reg, const char *itf, const char *name, int ftype, ...)
{
  va_list arg;

  va_start(arg, ftype);
  spi_dbus_emit_valist(reg->bus, SPI_DBUS_PATH_DEC, itf, name, ftype, arg);
  va_end(arg);
}

/*---------------------------------------------------------------------------*/

static void
add_bus_name_cb (gpointer item, gpointer data)
{
  DBusMessageIter *iter_array = (DBusMessageIter *) data;

  dbus_message_iter_append_basic (iter_array, DBUS_TYPE_STRING, (gchar **) &item);
}

static DBusMessage *
impl_getApplications (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  DBusMessage *reply;
  DBusMessageIter iter, iter_array;
  SpiRegistry *reg = SPI_REGISTRY (user_data);

  reply = dbus_message_new_method_return (message);

  dbus_message_iter_init_append (reply, &iter);
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "s", &iter_array);
  g_sequence_foreach (reg->apps, add_bus_name_cb, &iter_array);
  dbus_message_iter_close_container(&iter, &iter_array);
  return reply;
}

static DBusHandlerResult
message_handler (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  DBusMessage *reply = NULL;

  if (dbus_message_is_method_call (message, SPI_DBUS_INTERFACE_REGISTRY, "getApplications"))
    {
      reply = impl_getApplications (bus, message, user_data);
      if (reply)
        {
          dbus_connection_send (bus, reply, NULL);
          dbus_message_unref (reply);
        }
      return DBUS_HANDLER_RESULT_HANDLED;
    }
  else
    {
      return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
}

/*---------------------------------------------------------------------------*/

static gint
data_str_cmp (gpointer a, gpointer b, gpointer data)
{
  return g_strcmp0(a, b);
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
handle_register_application (SpiRegistry *reg, DBusMessage *message)
{
  gchar *app_name;

  if (dbus_message_get_args (message, NULL, DBUS_TYPE_STRING, &app_name, DBUS_TYPE_INVALID))
      g_sequence_insert_sorted (reg->apps, app_name, (GCompareDataFunc) data_str_cmp, NULL);
}

static void
handle_deregister_application (SpiRegistry *reg, DBusMessage *message)
{
  gchar *app_name;

  if (dbus_message_get_args (message, NULL, DBUS_TYPE_STRING, &app_name, DBUS_TYPE_INVALID))
      seq_remove_string (reg->apps, app_name);
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
      if (seq_remove_string (reg->apps, old))
        {
          /*Emit deregistered signal here*/
          emit (reg, SPI_DBUS_INTERFACE_TREE, "deregisterApplication", DBUS_TYPE_STRING, old);
          /*TODO spi_remove_device_listeners (registry->de_controller, old);*/
        }
    }
  }
}

static DBusHandlerResult
signal_handler (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  SpiRegistry *registry = SPI_REGISTRY (user_data);
  guint res = DBUS_HANDLER_RESULT_HANDLED;
  const char *iface = dbus_message_get_interface (message);
  const char *member = dbus_message_get_member (message);

  g_print ("\n%s", iface);
  g_print ("\n%d", dbus_message_get_type (message));
  g_print ("\n%s\n", member);

#if 0
  if (dbus_message_is_signal (message, DBUS_INTERFACE_DBUS, "NameOwnerChanged"))
      handle_disconnection (registry, message);
  else if (dbus_message_is_signal (message, SPI_DBUS_INTERFACE_TREE, "registerApplication"))
      handle_register_application (registry, message);
  else if (dbus_message_is_signal (message, SPI_DBUS_INTERFACE_TREE, "deregisterApplication"))
      handle_deregister_application (registry, message);
  else
      res = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
#endif

  if (!g_strcmp0(iface, DBUS_INTERFACE_DBUS) && !g_strcmp0(member, "NameOwnerChanged"))
      handle_disconnection (registry, message);
  else if (!g_strcmp0(iface, SPI_DBUS_INTERFACE_TREE) && !g_strcmp0(member, "registerApplication"))
      handle_register_application (registry, message);
  else if (!g_strcmp0(iface, SPI_DBUS_INTERFACE_TREE) && !g_strcmp0(member, "deregisterApplication"))
      handle_deregister_application (registry, message);
  else
      res = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  return res;
}

/*---------------------------------------------------------------------------*/

static gchar *app_reg_sig_match = "type='signal', interface='org.freedesktop.atspi.Tree', member='registerApplication'";
static gchar *app_dereg_sig_match = "type='signal', interface='org.freedesktop.atspi.Tree', member='deregisterApplication'";

static gchar *app_sig_match_blank = "";

static DBusObjectPathVTable reg_vtable =
{
  NULL,
  &message_handler,
  NULL, NULL, NULL, NULL
};

SpiRegistry *
spi_registry_new (DBusConnection *bus)
{
  SpiRegistry *reg = g_object_new (SPI_REGISTRY_TYPE, NULL);

  reg->bus = bus;

  dbus_connection_register_object_path(bus, SPI_DBUS_PATH_REGISTRY, &reg_vtable, reg);

  //dbus_bus_add_match (bus, app_reg_sig_match, NULL);
  //dbus_bus_add_match (bus, app_dereg_sig_match, NULL);
  dbus_bus_add_match (bus, app_sig_match_blank, NULL);
  dbus_connection_add_filter (bus, signal_handler, reg, NULL);

  return reg;
}
