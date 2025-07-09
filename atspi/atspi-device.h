/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2020 SUSE LLC.
 *
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

#pragma once

#include "glib-object.h"

#include "atspi-types.h"

G_BEGIN_DECLS

#define ATSPI_TYPE_DEVICE (atspi_device_get_type ())
G_DECLARE_DERIVABLE_TYPE (AtspiDevice, atspi_device, ATSPI, DEVICE, GObject)

struct _AtspiDeviceClass
{
  GObjectClass parent_class;

  gboolean (*add_key_grab) (AtspiDevice *device, AtspiKeyDefinition *kd);
  void (*remove_key_grab) (AtspiDevice *device, guint id);
  guint (*map_modifier) (AtspiDevice *device, gint keycode);
  void (*unmap_modifier) (AtspiDevice *device, gint keycode);
  guint (*get_modifier) (AtspiDevice *device, gint keycode);
  gboolean (*grab_keyboard) (AtspiDevice *device);
  void (*ungrab_keyboard) (AtspiDevice *device);
  guint (*get_locked_modifiers) (AtspiDevice *device);
  void (*generate_mouse_event) (AtspiDevice *device, AtspiAccessible *obj, gint x, gint y, const gchar *name, GError **error);
  guint (*map_keysym_modifier) (AtspiDevice *device, guint keysym);
  void (*unmap_keysym_modifier) (AtspiDevice *device, guint keysym);
  guint (*get_keysym_modifier) (AtspiDevice *device, guint keysym);
};

/**
 * AtspiKeyCallback:
 * @device: the device.
 * @pressed: TRUE if the key is being pressed, FALSE if being released.
 * @keycode: the hardware code for the key.
 * @keysym: the keysym for the key.
 * @modifiers: a bitflag indicating which key modifiers are active.
 * @keystring: the text corresponding to the keypress.
 * @user_data: (closure): user-supplied data
 *
 * A callback that will be invoked when a key is pressed.
 */
typedef void (*AtspiKeyCallback) (AtspiDevice *device, gboolean pressed, guint keycode, guint keysym, guint modifiers, const gchar *keystring, void *user_data);

AtspiDevice *atspi_device_new ();

gboolean atspi_device_notify_key (AtspiDevice *device, gboolean pressed, int keycode, int keysym, gint state, const gchar *text);

guint atspi_device_add_key_grab (AtspiDevice *device, AtspiKeyDefinition *kd, AtspiKeyCallback callback, void *user_data, GDestroyNotify callback_destroyed);

void atspi_device_remove_key_grab (AtspiDevice *device, guint id);

void atspi_device_add_key_watcher (AtspiDevice *device, AtspiKeyCallback callback, void *user_data, GDestroyNotify callback_destroyed);

AtspiKeyDefinition *atspi_device_get_grab_by_id (AtspiDevice *device, guint id);

guint atspi_device_map_modifier (AtspiDevice *device, gint keycode);

void atspi_device_unmap_modifier (AtspiDevice *device, gint keycode);

guint atspi_device_get_modifier (AtspiDevice *device, gint keycode);

guint atspi_device_get_locked_modifiers (AtspiDevice *device);

gboolean atspi_device_grab_keyboard (AtspiDevice *device);

void atspi_device_ungrab_keyboard (AtspiDevice *device);

void atspi_device_generate_mouse_event (AtspiDevice *device, AtspiAccessible *obj, gint x, gint y, const gchar *name, GError **error);

guint atspi_device_map_keysym_modifier (AtspiDevice *device, guint keysym);

void atspi_device_unmap_keysym_modifier (AtspiDevice *device, guint keysym);

guint atspi_device_get_keysym_modifier (AtspiDevice *device, guint keysym);

const gchar *atspi_device_get_app_id (AtspiDevice *device);

void atspi_device_set_app_id (AtspiDevice *device, const gchar *app_id);

AtspiDevice *atspi_device_new_full (const gchar *app_id);

void atspi_device_clear_key_grabs (AtspiDevice *device);
G_END_DECLS
