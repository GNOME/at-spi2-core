/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2024 Red Hat
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

#include "atspi-device-a11y-manager.h"
#include "atspi-private.h"

#include <gio/gio.h>

typedef struct _AtspiDeviceA11yManagerKey AtspiDeviceA11yManagerKey;
struct _AtspiDeviceA11yManagerKey
{
  guint32 keysym;
  guint32 modifiers;
};

typedef struct _AtspiDeviceA11yManager AtspiDeviceA11yManager;

struct _AtspiDeviceA11yManager
{
  AtspiDevice parent_instance;

  GDBusConnection *session_bus;
  GDBusProxy *keyboard_monitor;
  GSList *grabbed_modifiers;
  GSList *grabbed_keys;
};

G_DEFINE_TYPE (AtspiDeviceA11yManager, atspi_device_a11y_manager, ATSPI_TYPE_DEVICE)

static void
a11y_manager_signal_cb (GDBusProxy *proxy,
                        gchar      *sender_name,
                        gchar      *signal_name,
                        GVariant   *parameters,
                        gpointer    user_data)
{
  if (g_strcmp0 (signal_name, "KeyEvent") != 0)
    return;

  AtspiDeviceA11yManager *device = ATSPI_DEVICE_A11Y_MANAGER (user_data);
  gboolean released;
  gchar *text;
  gint utf8_len;
  guint32 state, keysym, unichar;
  guint16 keycode;

  g_variant_get (parameters, "(buuuq)", &released, &state, &keysym, &unichar, &keycode);
  utf8_len = g_unichar_to_utf8 (unichar, NULL);
  text = g_new (gchar, utf8_len + 1);
  g_unichar_to_utf8 (unichar, text);
  text[utf8_len] = '\0';
  atspi_device_notify_key (ATSPI_DEVICE (device), !released, keycode, keysym, state, text);
  g_free (text);
}

static guint
atspi_device_a11y_manager_get_locked_modifiers (AtspiDevice *device)
{

  return 0;
}

/*static gboolean
check_virtual_modifier (AtspiDeviceA11yManager *manager_device, guint modifier)
{
    GSList *l;

  if (modifier == (1 << ATSPI_MODIFIER_NUMLOCK))
    return TRUE;

  for (l = manager_device->modifiers; l; l = l->next)
    {
      AtspiA11yManagerKeyModifier *entry = l->data;
      if (entry->modifier == modifier)
        return TRUE;
    }

  return FALSE;
}

static guint
get_unused_virtual_modifier (AtspiDeviceA11yManager *manager_device)
{
  guint ret = 0x1000;

  while (ret < 0x10000)
    {
      if (!check_virtual_modifier (manager_device, ret))
        return ret;
      ret <<= 1;
    }

  return 0;
}*/

static guint
atspi_device_a11y_manager_map_modifier (AtspiDevice *device, gint keycode)
{
  g_warning ("Requested to map keycode %d", keycode);
  return 0;
}

static void
atspi_device_a11y_manager_unmap_modifier (AtspiDevice *device, gint keycode)
{
  g_warning ("Requested to unmap keycode %d", keycode);
  /*AtspiDeviceA11yManager *manager_device = ATSPI_DEVICE_LEGACY (device);
  GSList *l;

  for (l = manager_device->modifiers; l; l = l->next)
    {
      AtspiA11yManagerKeyModifier *entry = l->data;
      if (entry->keycode == keycode)
        {
          manager_device->modifiers = g_slist_remove (manager_device->modifiers, entry);
          g_free (entry);
          return;
        }
    }*/
}

static guint
atspi_device_a11y_manager_get_modifier (AtspiDevice *device, gint keycode)
{
  /*AtspiDeviceA11yManager *manager_device = ATSPI_DEVICE_LEGACY (device);

  return find_virtual_mapping (manager_device, keycode);*/
  return 0;
}

static gboolean
atspi_device_a11y_manager_grab_keyboard (AtspiDevice *device)
{
  AtspiDeviceA11yManager *manager_device = ATSPI_DEVICE_A11Y_MANAGER (device);

  g_dbus_proxy_call_sync (manager_device->keyboard_monitor,
                          "GrabKeyboard",
                          NULL,
                          G_DBUS_CALL_FLAGS_NONE,
                          -1,
                          NULL,
                          NULL);
  return TRUE;
}

static void
atspi_device_a11y_manager_ungrab_keyboard (AtspiDevice *device)
{
  AtspiDeviceA11yManager *manager_device = ATSPI_DEVICE_A11Y_MANAGER (device);

  g_dbus_proxy_call_sync (manager_device->keyboard_monitor,
                          "UngrabKeyboard",
                          NULL,
                          G_DBUS_CALL_FLAGS_NONE,
                          -1,
                          NULL,
                          NULL);
}

static void
refresh_grabs (AtspiDeviceA11yManager *manager_device)
{
  GVariantBuilder builder;
  GSList *l;

  g_variant_builder_init (&builder, G_VARIANT_TYPE ("(aua(uu))"));
  g_variant_builder_open (&builder, G_VARIANT_TYPE ("au"));
  for (l = manager_device->grabbed_modifiers; l; l = l->next)
    {
      guint32 modifier = GPOINTER_TO_UINT (l->data);
      g_variant_builder_add (&builder, "u", modifier);
    }
  g_variant_builder_close (&builder);
  g_variant_builder_open (&builder, G_VARIANT_TYPE ("a(uu)"));
  for (l = manager_device->grabbed_keys; l; l = l->next)
    {
      AtspiDeviceA11yManagerKey *entry = l->data;
      g_variant_builder_open (&builder, G_VARIANT_TYPE ("(uu)"));
      g_variant_builder_add (&builder, "u", entry->keysym);
      g_variant_builder_add (&builder, "u", entry->modifiers);
      g_variant_builder_close (&builder);
    }
  g_variant_builder_close (&builder);
  g_dbus_proxy_call_sync (manager_device->keyboard_monitor,
                          "SetGrabs",
                          g_variant_builder_end (&builder),
                          G_DBUS_CALL_FLAGS_NONE,
                          -1,
                          NULL,
                          NULL);
}

static gboolean
atspi_device_a11y_manager_add_key_grab (AtspiDevice *device, AtspiKeyDefinition *kd)
{
  AtspiDeviceA11yManager *manager_device = ATSPI_DEVICE_A11Y_MANAGER (device);

  g_warning ("Requested to grab keysym %d with modifiers %d and keycode %d", kd->keysym, kd->modifiers, kd->keycode);
  AtspiDeviceA11yManagerKey *entry = g_new (AtspiDeviceA11yManagerKey, 1);
  entry->keysym = kd->keysym;
  entry->modifiers = kd->modifiers;
  manager_device->grabbed_keys = g_slist_append (manager_device->grabbed_keys, entry);
  refresh_grabs (manager_device);
  return TRUE;
}

static void
atspi_device_a11y_manager_remove_key_grab (AtspiDevice *device, guint id)
{
  AtspiDeviceA11yManager *manager_device = ATSPI_DEVICE_A11Y_MANAGER (device);
  GSList *l;

  AtspiKeyDefinition *kd = atspi_device_get_grab_by_id (device, id);
  for (l = manager_device->grabbed_keys; l; l = l->next)
    {
      AtspiDeviceA11yManagerKey *entry = l->data;
      if (entry->keysym == kd->keysym && entry->modifiers == kd->modifiers)
        {
          manager_device->grabbed_keys = g_slist_remove (manager_device->grabbed_keys, entry);
          g_free (entry);
          refresh_grabs (manager_device);
          return;
        }
    }
}

static void
atspi_device_a11y_manager_init (AtspiDeviceA11yManager *device)
{
  g_signal_connect_object (device->keyboard_monitor, "g-signal", G_CALLBACK (a11y_manager_signal_cb), device, 0);

  device->grabbed_modifiers = NULL;
  device->grabbed_keys = NULL;
  }

static void
atspi_device_a11y_manager_finalize (GObject *object)
{
  AtspiDeviceA11yManager *device = ATSPI_DEVICE_A11Y_MANAGER (object);

  g_clear_object (&device->keyboard_monitor);
  g_clear_object (&device->session_bus);
  G_OBJECT_CLASS (atspi_device_a11y_manager_parent_class)->finalize (object);
}

static void
atspi_device_a11y_manager_class_init (AtspiDeviceA11yManagerClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;
  AtspiDeviceClass *device_class = ATSPI_DEVICE_CLASS (klass);

  object_class->finalize = atspi_device_a11y_manager_finalize;
  device_class->map_modifier = atspi_device_a11y_manager_map_modifier;
  device_class->unmap_modifier = atspi_device_a11y_manager_unmap_modifier;
  device_class->get_modifier = atspi_device_a11y_manager_get_modifier;
  device_class->get_locked_modifiers = atspi_device_a11y_manager_get_locked_modifiers;
  device_class->grab_keyboard = atspi_device_a11y_manager_grab_keyboard;
  device_class->ungrab_keyboard = atspi_device_a11y_manager_ungrab_keyboard;
  device_class->add_key_grab = atspi_device_a11y_manager_add_key_grab;
  device_class->remove_key_grab = atspi_device_a11y_manager_remove_key_grab;
}

/**
 * atspi_device_a11y_manager_try_new:
 *
 * Tries to create a new #AtspiDeviceA11yManager.
 *
 * Returns: (transfer full): a pointer to a newly-created #AtspiDeviceA11yManager
 * if it could establish the required DBus connection, NULL otherwise.
 *
 **/
AtspiDeviceA11yManager *
atspi_device_a11y_manager_try_new ()
{
  GError *error = NULL;
  GDBusConnection *session_bus = g_bus_get_sync (G_BUS_TYPE_SESSION, NULL, NULL);
  GDBusProxy *keyboard_monitor = g_dbus_proxy_new_sync (session_bus,
                                                    G_DBUS_PROXY_FLAGS_NONE,
                                                    NULL,
                                                    ATSPI_DBUS_NAME_A11Y_MANAGER,
                                                    ATSPI_DBUS_PATH_A11Y_MANAGER,
                                                    ATSPI_DBUS_INTERFACE_KEYBOARD_MONITOR,
                                                    NULL,
                                                    &error);

  if (error)
    {
      g_clear_object (&session_bus);
      g_error_free (error);
      return NULL;
    }

  AtspiDeviceA11yManager *device = g_object_new (atspi_device_a11y_manager_get_type (), NULL);
  device->session_bus = session_bus;
  device->keyboard_monitor = keyboard_monitor;

  return device;
}
