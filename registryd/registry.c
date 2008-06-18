/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
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

/* registry.c: the main accessibility service registry implementation */

#undef SPI_LISTENER_DEBUG
#undef SPI_DEBUG
#undef SPI_QUEUE_DEBUG

#include <config.h>
#ifdef SPI_DEBUG
#  include <stdio.h>
#endif

#include <spi-common/spi-dbus.h>

#include "registry.h"
#include "dbus/dbus-glib-lowlevel.h"

/* Our parent GObject type  */
#define PARENT_TYPE G_OBJECT_TYPE

int _dbg = 0;

typedef enum {
  ETYPE_FOCUS,
  ETYPE_OBJECT,
  ETYPE_PROPERTY,
  ETYPE_WINDOW,
  ETYPE_TOOLKIT,
  ETYPE_KEYBOARD,
  ETYPE_MOUSE,
  ETYPE_LAST_DEFINED
} EventTypeCategory;

typedef struct {
  const char *event_name;
  EventTypeCategory type_cat;
  GQuark major;  /* from string segment[1] */
  GQuark minor;  /* from string segment[1]+segment[2] */
  GQuark detail; /* from string segment[3] (not concatenated) */
} EventTypeStruct;

G_DEFINE_TYPE(SpiRegistry, spi_registry, G_TYPE_OBJECT)

static void
spi_registry_set_debug (const char *debug_flag_string)
{
  if (debug_flag_string) 
    _dbg = (int) g_ascii_strtod (debug_flag_string, NULL);
}

static void emit(SpiRegistry *registry, const char *name, int first_type, ...)
{
  va_list arg;

  va_start(arg, first_type);
  spi_dbus_emit_valist(registry->droute.bus, SPI_DBUS_PATH_DESKTOP, SPI_DBUS_INTERFACE_REGISTRY, name, first_type, arg);
  va_end(arg);
}

static void
desktop_add_application (SpiDesktop *desktop,
			 guint index, gpointer data)
{
  SpiRegistry *registry = SPI_REGISTRY (data);
  const SpiDesktopApplication *app = g_list_nth_data(desktop->applications, index);
  
  emit(registry, "applicationAdd", DBUS_TYPE_UINT32, &index, DBUS_TYPE_STRING, &app->bus_name, DBUS_TYPE_INVALID);
}



static void
desktop_remove_application (SpiDesktop *desktop,
			    guint index, gpointer data)
{
  SpiRegistry *registry = SPI_REGISTRY (data);
  SpiDesktopApplication *app = g_list_nth_data(desktop->applications, index);
  
  spi_dbus_remove_disconnect_match (registry->droute.bus, app->bus_name);
  emit(registry, "applicationRemove", DBUS_TYPE_UINT32, &index, DBUS_TYPE_STRING, &app->bus_name, DBUS_TYPE_INVALID);
}


static void
spi_registry_object_finalize (GObject *object)
{
  DBG (1, g_warning ("spi_registry_object_finalize called\n"));
  g_object_unref (SPI_REGISTRY (object)->de_controller);

  G_OBJECT_CLASS (spi_registry_parent_class)->finalize (object);
}

/**
 * registerApplication:
 * @application: a reference to the requesting @Application
 * return values: void
 *
 * Register a new application with the accessibility broker.
 *
 **/
static DBusMessage *
impl_accessibility_registry_register_application (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  SpiRegistry *registry = SPI_REGISTRY (user_data);
  const char *application = dbus_message_get_sender (message);

#ifdef SPI_DEBUG
  fprintf (stderr, "registering app %s\n", application);
#endif
  spi_desktop_add_application (registry->desktop, application);

  spi_dbus_add_disconnect_match (registry->droute.bus, application);

  /*
   * TODO: change the implementation below to a WM-aware one;
   * e.g. don't add all apps to the SpiDesktop
   */
  return dbus_message_new_method_return (message);
}

/**
 * deregisterApplication:
 * @application: a reference to the @Application
 * to be deregistered.
 * return values: void
 *
 * De-register an application previously registered with the broker.
 *
 **/
static DBusMessage *
impl_accessibility_registry_deregister_application (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  SpiRegistry *registry = SPI_REGISTRY (user_data);
  const char *application = dbus_message_get_sender (message);

  spi_desktop_remove_application (registry->desktop, application);

#ifdef SPI_DEBUG
  fprintf (stderr, "de-registered app %s\n", application);
#endif
  return dbus_message_new_method_return (message);
}

/**
 * getDesktopCount:
 * return values: a short integer indicating the current number of
 * @Desktops.
 *
 * Get the current number of desktops.
 *
 **/
static DBusMessage *
impl_accessibility_registry_get_desktop_count (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  dbus_int16_t n_desktops = 1;
  DBusMessage *reply;

  /* TODO: implement support for multiple virtual desktops */
  reply = dbus_message_new_method_return (message);
  if (reply)
  {
    dbus_message_append_args (reply, DBUS_TYPE_INT16, &n_desktops, DBUS_TYPE_INVALID);
  }
  return reply;
}

/**
 * getDesktop:
 * @n: the index of the requested @Desktop.
 * return values: a reference to the requested @Desktop.
 *
 * Get the nth accessible desktop.
 *
 **/
static DBusMessage *
impl_accessibility_registry_get_desktop (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  DBusError error;
  dbus_int16_t n;
  const char *path;
  DBusMessage *reply;

  /* TODO: implement support for multiple virtual desktops */
  dbus_error_init (&error);
  if (!dbus_message_get_args (message, &error, DBUS_TYPE_INT16, &n, DBUS_TYPE_INVALID))
  {
    return spi_dbus_general_error (message);
  }
  path = (n == 0? SPI_DBUS_PATH_DESKTOP: "/");
  reply = dbus_message_new_method_return (message);
  if (reply)
  {
    dbus_message_append_args (reply, DBUS_TYPE_STRING, &path, DBUS_TYPE_INVALID);
  }
  return reply;
}


/**
 * getDesktopList:
 * return values: a sequence containing references to
 * the @Desktops.
 *
 * Get a list of accessible desktops.
 *
 **/
static DBusMessage *
impl_accessibility_registry_get_desktop_list (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  DBusMessage *reply;
  DBusMessageIter iter, iter_array;
  const char *path = SPI_DBUS_PATH_DESKTOP;

  reply = dbus_message_new_method_return (message);
  if (!reply) return NULL;
  dbus_message_iter_init_append(reply, &iter);
  if (!dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "o", &iter_array)) goto oom;
  dbus_message_iter_append_basic(&iter_array, DBUS_TYPE_OBJECT_PATH, &path);
  if (!dbus_message_iter_close_container (&iter, &iter_array)) goto oom;
  return reply;
oom:
  // TODO: handle out-of-memory
  return reply;
}


static DBusMessage *
impl_accessibility_registry_get_device_event_controller (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  DBusMessage *reply;
  const char *path = SPI_DBUS_PATH_DEC;

  reply = dbus_message_new_method_return (message);
  if (reply)
  {
    dbus_message_append_args (reply, DBUS_TYPE_STRING, &path, DBUS_TYPE_INVALID);
  }
  return reply;
}


static void
spi_registry_class_init (SpiRegistryClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;

  spi_registry_parent_class = g_type_class_ref (G_TYPE_OBJECT);
  
  object_class->finalize = spi_registry_object_finalize;
}

static DBusObjectPathVTable droute_vtable =
{
  NULL,
  &droute_message,
  NULL, NULL, NULL, NULL
};

DBusHandlerResult
disconnect_watch (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  SpiRegistry *registry = SPI_REGISTRY (user_data);
  const char *name, *old, *new;

  if (!dbus_message_is_signal (message, DBUS_INTERFACE_DBUS, "NameOwnerChanged"))
  {
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_STRING, &name, DBUS_TYPE_STRING, &old, DBUS_TYPE_STRING, &new, DBUS_TYPE_INVALID))
  {
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }
  if (*old != '\0' && *new == '\0')
  {
    spi_desktop_remove_application (registry->desktop, old);
    spi_remove_device_listeners (registry->de_controller, old);
  }
  return DBUS_HANDLER_RESULT_HANDLED;
}

static void
spi_registry_init (SpiRegistry *registry)
{
  DBusError error;

  spi_registry_set_debug (g_getenv ("AT_SPI_DEBUG"));
  /*
   * TODO: FIXME, this module makes the foolish assumptions that
   * registryd uses the same display as the apps, and that the
   * DISPLAY environment variable is set.
   */
  gdk_init (NULL, NULL);

  registry->exit_notify_timeout = 200;
  registry->queue_handler_id  = 0;

  dbus_error_init (&error);
  registry->droute.bus = dbus_bus_get(DBUS_BUS_SESSION, &error);
  if (!registry->droute.bus)
  {
    g_warning("Couldn't connect to dbus: %s\n", error.message);
    return;
  }
  registry->droute.user_data = registry;
  spi_registry_initialize_registry_interface (&registry->droute);
  spi_registry_initialize_desktop_interface (&registry->droute);
  spi_registry_initialize_dec_interface (&registry->droute);
  // todo: initialize accessible and component interfaces, for desktop?
  if (!dbus_connection_try_register_fallback (registry->droute.bus, "/org/freedesktop/atspi", &droute_vtable, &registry->droute, &error))
  {
    g_warning("Couldn't register droute.\n");
  }
  dbus_connection_setup_with_g_main(registry->droute.bus, g_main_context_default());

  // TODO: decide whether focus_object is still relevant
  registry->desktop = spi_desktop_new ();
  /* Register callback notification for application addition and removal */
  g_signal_connect (G_OBJECT (registry->desktop),
		    "application_added",
		    G_CALLBACK (desktop_add_application),
		    registry);

  g_signal_connect (G_OBJECT (registry->desktop),
		    "application_removed",
		    G_CALLBACK (desktop_remove_application),
		    registry);

  registry->de_controller = spi_device_event_controller_new (registry);

  dbus_connection_add_filter (registry->droute.bus, disconnect_watch, registry, NULL);
}

SpiRegistry *
spi_registry_new (void)
{
  SpiRegistry *retval = g_object_new (SPI_REGISTRY_TYPE, NULL);
  return retval;
}

static DRouteMethod methods[] =
{
  { impl_accessibility_registry_register_application , "registerApplication" },
  { impl_accessibility_registry_deregister_application, "deregisterApplication" },
  { impl_accessibility_registry_get_desktop_count, "getDesktopCount" },
  { impl_accessibility_registry_get_desktop, "getDesktop" },
  { impl_accessibility_registry_get_desktop_list, "getDesktopList" },
  { impl_accessibility_registry_get_device_event_controller, "getDeviceEventController" },
  { NULL, NULL }
};

void
spi_registry_initialize_registry_interface (DRouteData * data)
{
  droute_add_interface (data, "org.freedesktop.atspi.Registry", methods,
			NULL, NULL, NULL);
};
