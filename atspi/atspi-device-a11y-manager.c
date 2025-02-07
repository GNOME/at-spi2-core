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

#define REFRESH_KEYS_TIMEOUT 10

typedef struct _AtspiDeviceA11yManagerKey AtspiDeviceA11yManagerKey;
struct _AtspiDeviceA11yManagerKey
{
  guint32 keysym;
  guint32 modifiers;
};

typedef struct _AtspiDeviceA11yManagerVirtualModifier AtspiDeviceA11yManagerVirtualModifier;
struct _AtspiDeviceA11yManagerVirtualModifier
{
  guint32 modifier;
  guint32 keysym;
};

typedef struct _AtspiDeviceA11yManager AtspiDeviceA11yManager;

struct _AtspiDeviceA11yManager
{
  AtspiDevice parent_instance;

  GDBusConnection *session_bus;
  GDBusProxy *keyboard_monitor;
  GSList *grabbed_modifiers;
  GSList *grabbed_keys;
  GSList *virtual_modifiers;
  guint enabled_virtual_modifiers;

  guint refresh_timeout_id;
};

G_DEFINE_TYPE (AtspiDeviceA11yManager, atspi_device_a11y_manager, ATSPI_TYPE_DEVICE)

static guint
atspi_device_a11y_manager_get_locked_modifiers (AtspiDevice *device)
{

  return 0;
}

static guint
get_unused_virtual_modifier (AtspiDeviceA11yManager *manager_device)
{
  guint prev_mod = ATSPI_DEVICE_A11Y_MANAGER_VIRTUAL_MOD_START - 1;
  guint cur_mod = ATSPI_DEVICE_A11Y_MANAGER_VIRTUAL_MOD_START - 1;
  GSList *l;

  for (l = manager_device->virtual_modifiers; l; l = l->next)
    {
      AtspiDeviceA11yManagerVirtualModifier *entry = l->data;
      cur_mod = entry->modifier;
      if (cur_mod - prev_mod > 1) /* Found a hole from a previous unmap, reuse it */
        return prev_mod + 1;
      prev_mod = entry->modifier;
    }

  if (cur_mod < ATSPI_DEVICE_A11Y_MANAGER_VIRTUAL_MOD_END)
    return cur_mod + 1;
  return 0;
}

static GSList *
find_insertion_point_for_modifier (AtspiDeviceA11yManager *manager_device, guint32 modifier)
{
  GSList *l;

  for (l = manager_device->virtual_modifiers; l; l = l->next)
    {
      AtspiDeviceA11yManagerVirtualModifier *entry = l->data;
      if (entry->modifier == modifier - 1)
        return l->next;
    }
  return NULL;
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
                          "SetKeyGrabs",
                          g_variant_builder_end (&builder),
                          G_DBUS_CALL_FLAGS_NONE,
                          -1,
                          NULL,
                          NULL);
}

static gboolean
refresh_keys_timeout_cb (gpointer user_data)
{
  AtspiDeviceA11yManager *device = ATSPI_DEVICE_A11Y_MANAGER (user_data);
  refresh_grabs (device);
  device->refresh_timeout_id = 0;
  return G_SOURCE_REMOVE;
}

static void
schedule_refresh_grabs (AtspiDeviceA11yManager *device)
{
  if (device->refresh_timeout_id)
    return;
  device->refresh_timeout_id = g_timeout_add (REFRESH_KEYS_TIMEOUT, refresh_keys_timeout_cb, device);
}

static guint
atspi_device_a11y_manager_map_modifier (AtspiDevice *device, gint keycode)
{
  g_warning ("Mapping of keycode %d is not supported, use the keysym instead", keycode);
  return 0;
}

static void
atspi_device_a11y_manager_unmap_modifier (AtspiDevice *device, gint keycode)
{
  g_warning ("Unmapping of keycode %d is not supported, use the keysym instead", keycode);
}

static guint
atspi_device_a11y_manager_get_modifier (AtspiDevice *device, gint keycode)
{
  g_warning ("Querying of keycode %d is not supported, use the keysym instead", keycode);
  return 0;
}

static guint
atspi_device_a11y_manager_get_keysym_modifier (AtspiDevice *device, guint keysym)
{
  AtspiDeviceA11yManager *manager_device = ATSPI_DEVICE_A11Y_MANAGER (device);
  GSList *l;

  for (l = manager_device->virtual_modifiers; l; l = l->next)
    {
      AtspiDeviceA11yManagerVirtualModifier *entry = l->data;
      if (entry->keysym == keysym)
        return 1 << entry->modifier;
    }
  return 0;
}

static guint
atspi_device_a11y_manager_map_keysym_modifier (AtspiDevice *device, guint keysym)
{
  AtspiDeviceA11yManager *manager_device = ATSPI_DEVICE_A11Y_MANAGER (device);
  guint modifier = atspi_device_a11y_manager_get_keysym_modifier (device, keysym);
  if (modifier)
    return modifier;
  modifier = get_unused_virtual_modifier (manager_device);

  if (!modifier)
    {
      g_warning ("No more virtual modifiers available");
      return 0;
    }

  AtspiDeviceA11yManagerVirtualModifier *entry = g_new (AtspiDeviceA11yManagerVirtualModifier, 1);
  entry->modifier = modifier;
  entry->keysym = keysym;
  GSList *insertion_point = find_insertion_point_for_modifier (manager_device, modifier);
  manager_device->virtual_modifiers = g_slist_insert_before (manager_device->virtual_modifiers, insertion_point, entry);

  manager_device->grabbed_modifiers = g_slist_append (manager_device->grabbed_modifiers, GUINT_TO_POINTER (keysym));
  schedule_refresh_grabs (manager_device);
  return 1 << modifier;
}

static void
atspi_device_a11y_manager_unmap_keysym_modifier (AtspiDevice *device, guint modifier)
{
  AtspiDeviceA11yManager *manager_device = ATSPI_DEVICE_A11Y_MANAGER (device);
  GSList *l;

  for (l = manager_device->virtual_modifiers; l; l = l->next)
    {
      AtspiDeviceA11yManagerVirtualModifier *entry = l->data;
      if (entry->modifier == modifier)
        {
          manager_device->virtual_modifiers = g_slist_remove (manager_device->virtual_modifiers, entry);
          g_free (entry);
          break;
        }
    }

  manager_device->grabbed_modifiers = g_slist_remove (manager_device->grabbed_modifiers, GUINT_TO_POINTER (modifier));
  schedule_refresh_grabs (manager_device);
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

static gboolean
has_key_grab (AtspiDeviceA11yManager *device, guint32 keysym, guint32 modifiers)
{
  GSList *l;

  for (l = device->grabbed_keys; l; l = l->next)
    {
      AtspiDeviceA11yManagerKey *entry = l->data;
      if (entry->keysym == keysym && entry->modifiers == modifiers)
        return TRUE;
    }
  return FALSE;
}

static gboolean
atspi_device_a11y_manager_add_key_grab (AtspiDevice *device, AtspiKeyDefinition *kd)
{
  AtspiDeviceA11yManager *manager_device = ATSPI_DEVICE_A11Y_MANAGER (device);

  if (has_key_grab (manager_device, kd->keysym, kd->modifiers))
    return FALSE;
  AtspiDeviceA11yManagerKey *entry = g_new (AtspiDeviceA11yManagerKey, 1);
  entry->keysym = kd->keysym;
  entry->modifiers = kd->modifiers;
  manager_device->grabbed_keys = g_slist_append (manager_device->grabbed_keys, entry);
  schedule_refresh_grabs (manager_device);
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
          schedule_refresh_grabs (manager_device);
          return;
        }
    }
}

static void
a11y_manager_signal_cb (GDBusProxy *proxy,
                        gchar *sender_name,
                        gchar *signal_name,
                        GVariant *parameters,
                        gpointer user_data)
{
  if (g_strcmp0 (signal_name, "KeyEvent") != 0)
    return;

  AtspiDeviceA11yManager *device = ATSPI_DEVICE_A11Y_MANAGER (user_data);
  gboolean released;
  gchar *text;
  gint utf8_len;
  guint32 state, keysym, unichar, virtual_modifier;
  guint16 keycode;

  g_variant_get (parameters, "(buuuq)", &released, &state, &keysym, &unichar, &keycode);
  utf8_len = g_unichar_to_utf8 (unichar, NULL);
  text = g_new (gchar, utf8_len + 1);
  g_unichar_to_utf8 (unichar, text);
  text[utf8_len] = '\0';
  virtual_modifier = atspi_device_a11y_manager_get_keysym_modifier (ATSPI_DEVICE (device), keysym);
  if (virtual_modifier)
    {
      if (released)
        device->enabled_virtual_modifiers &= ~virtual_modifier;
      else
        device->enabled_virtual_modifiers |= virtual_modifier;
    }
  state = state | device->enabled_virtual_modifiers;
  atspi_device_notify_key (ATSPI_DEVICE (device), !released, keycode, keysym, state, text);
  g_free (text);
}

static void
atspi_device_a11y_manager_init (AtspiDeviceA11yManager *device)
{
  device->grabbed_modifiers = NULL;
  device->grabbed_keys = NULL;
  device->refresh_timeout_id = 0;
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
atspi_device_a11y_manager_dispose (GObject *object)
{
  AtspiDeviceA11yManager *device = ATSPI_DEVICE_A11Y_MANAGER (object);

  g_slist_free_full (device->grabbed_modifiers, g_free);
  g_slist_free_full (device->grabbed_keys, g_free);
  g_slist_free_full (device->virtual_modifiers, g_free);
  if (device->refresh_timeout_id)
    g_source_remove (device->refresh_timeout_id);
  G_OBJECT_CLASS (atspi_device_a11y_manager_parent_class)->dispose (object);
}

static void
atspi_device_a11y_manager_class_init (AtspiDeviceA11yManagerClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;
  AtspiDeviceClass *device_class = ATSPI_DEVICE_CLASS (klass);

  object_class->dispose = atspi_device_a11y_manager_dispose;
  object_class->finalize = atspi_device_a11y_manager_finalize;
  device_class->map_modifier = atspi_device_a11y_manager_map_modifier;
  device_class->unmap_modifier = atspi_device_a11y_manager_unmap_modifier;
  device_class->get_modifier = atspi_device_a11y_manager_get_modifier;
  device_class->get_locked_modifiers = atspi_device_a11y_manager_get_locked_modifiers;
  device_class->map_keysym_modifier = atspi_device_a11y_manager_map_keysym_modifier;
  device_class->unmap_keysym_modifier = atspi_device_a11y_manager_unmap_keysym_modifier;
  device_class->get_keysym_modifier = atspi_device_a11y_manager_get_keysym_modifier;
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

  g_dbus_proxy_call_sync (device->keyboard_monitor,
                     "WatchKeyboard",
                     NULL,
                     G_DBUS_CALL_FLAGS_NONE,
                     -1,
                     NULL,
                     NULL);

  g_signal_connect_object (device->keyboard_monitor, "g-signal", G_CALLBACK (a11y_manager_signal_cb), device, 0);

  return device;
}
