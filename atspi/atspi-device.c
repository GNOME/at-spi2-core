/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2020 SUSE LLC.
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

#include "atspi-device.h"
#include "atspi-device-legacy.h"
#include "atspi-device-x11.h"
#include "atspi-private.h"

typedef struct
{
  guint id;
  guint keycode;
  guint keysym;
  guint modifiers;
  AtspiKeyCallback callback;
  void *callback_data;
  GDestroyNotify callback_destroyed;
} AtspiKeyGrab;

typedef struct _AtspiDevicePrivate AtspiDevicePrivate;
struct _AtspiDevicePrivate
{
  GSList *key_watchers;
  GSList *keygrabs;
  guint last_grab_id;
};

GObjectClass *device_parent_class;

static void
atspi_device_init (AtspiDevice *device)
{
}

G_DEFINE_TYPE_WITH_CODE (AtspiDevice, atspi_device, G_TYPE_OBJECT, G_ADD_PRIVATE (AtspiDevice))

static void
atspi_device_finalize (GObject *object)
{
  AtspiDevice *device = (AtspiDevice *) object;
  AtspiDevicePrivate *priv = atspi_device_get_instance_private (device);

  g_slist_free_full (priv->keygrabs, g_free);
  priv->keygrabs = NULL;

  device_parent_class->finalize (object);
}

static gboolean
atspi_device_real_add_key_grab (AtspiDevice *device, AtspiKeyDefinition *kd)
{
  return TRUE;
}

static void
atspi_device_real_remove_key_grab (AtspiDevice *device, guint id)
{
}

static void
atspi_device_class_init (AtspiDeviceClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;

  device_parent_class = g_type_class_peek_parent (klass);
  klass->add_key_grab = atspi_device_real_add_key_grab;
  klass->remove_key_grab = atspi_device_real_remove_key_grab;
  object_class->finalize = atspi_device_finalize;
}

/**
 * atspi_device_new:
 *
 * Creates a new #AtspiDevice with a specified callback function.
 *
 * Returns: (transfer full): a pointer to a newly-created #AtspiDevice.
 *
 **/
AtspiDevice *
atspi_device_new ()
{
#ifdef HAVE_X11
  if (!g_getenv ("WAYLAND_DISPLAY") && !g_getenv ("ATSPI_USE_LEGACY_DEVICE"))
    return ATSPI_DEVICE (atspi_device_x11_new ());
#endif

  return ATSPI_DEVICE (atspi_device_legacy_new ());
}

static gboolean
key_matches_modifiers (gint keycode, guint key_mods, guint grab_mods)
{
  /* The presence or lack thereof of locking modifiers should make no
     difference when testing, but other modifiers should match. If the
     keypress has modifiers that (a) the grab does not check for and (b) are
     not lock modifiers, then we reject the match. Alt + left arrow should not
     match a grab on left arrow, for instance, but whether numlock is on or
     off would be irrelevant. */
  if (_atspi_key_is_on_keypad (keycode))
    key_mods &= ~((1 << ATSPI_MODIFIER_SHIFTLOCK));
  else
    key_mods &= ~((1 << ATSPI_MODIFIER_SHIFTLOCK) | (1 << ATSPI_MODIFIER_NUMLOCK));
  return (key_mods == grab_mods);
}

gboolean
atspi_device_notify_key (AtspiDevice *device, gboolean pressed, int keycode, int keysym, gint state, gchar *text)
{
  AtspiDevicePrivate *priv = atspi_device_get_instance_private (device);
  GSList *l;
  gboolean ret = FALSE;

  for (l = priv->key_watchers; l; l = l->next)
    {
      AtspiKeyGrab *grab = l->data;
      grab->callback (device, pressed, keycode, keysym, state, text, grab->callback_data);
    }

  for (l = priv->keygrabs; l; l = l->next)
    {
      AtspiKeyGrab *grab = l->data;
      if (keycode == grab->keycode && key_matches_modifiers (keycode, state, grab->modifiers))
        {
          if (grab->callback)
            grab->callback (device, pressed, keycode, keysym, state, text, grab->callback_data);
          ret = TRUE;
        }
    }

  return ret;
}

static gboolean
is_id_used (AtspiDevice *device, guint id)
{
  AtspiDevicePrivate *priv = atspi_device_get_instance_private (device);
  GSList *l;

  for (l = priv->key_watchers; l; l = l->next)
    {
      AtspiKeyGrab *grab = l->data;
      if (grab->id == id)
        return TRUE;
    }

  for (l = priv->keygrabs; l; l = l->next)
    {
      AtspiKeyGrab *grab = l->data;
      if (grab->id == id)
        return TRUE;
    }

  return FALSE;
}

static guint
get_grab_id (AtspiDevice *device)
{
  AtspiDevicePrivate *priv = atspi_device_get_instance_private (device);

  while (is_id_used (device, priv->last_grab_id))
    priv->last_grab_id++;
  return priv->last_grab_id++;
}

/**
 *atspi_device_add_key_grab:
 * @device: the device.
 * @kd: a #AtspiKeyDefinition specifying the key code to grab.
 * @callback: (scope notified) (allow-none): the function to call when the
 *            given key is pressed.
 * @user_data: Data to be passed to @callback.
 * @callback_destroyed: callback function to be called when @callback is
 *                      destroyed.
 *
 * Returns: an identifier that can be later used to remove the grab, or 0
 * if the key/modifier combination could not be grabbed.
 * Add a key grab for the given key/modifier combination.
 */
guint
atspi_device_add_key_grab (AtspiDevice *device, AtspiKeyDefinition *kd, AtspiKeyCallback callback, void *user_data, GDestroyNotify callback_destroyed)
{
  AtspiDevicePrivate *priv = atspi_device_get_instance_private (device);
  AtspiKeyGrab *grab;

  if (!ATSPI_DEVICE_GET_CLASS (device)->add_key_grab (device, kd))
    return 0;

  grab = g_new (AtspiKeyGrab, 1);
  grab->keycode = kd->keycode;
  grab->keysym = kd->keysym;
  grab->modifiers = kd->modifiers;
  grab->callback = callback;
  grab->callback_data = user_data;
  grab->callback_destroyed = callback_destroyed;
  grab->id = get_grab_id (device);
  priv->keygrabs = g_slist_append (priv->keygrabs, grab);

  return grab->id;
}

/**
 * atspi_device_remove_key_grab:
 * @device: the device.
 * @id: the identifier of the grab to be removed.
 *
 * Removes the key grab specified by @id.
 */
void
atspi_device_remove_key_grab (AtspiDevice *device, guint id)
{
  AtspiDevicePrivate *priv = atspi_device_get_instance_private (device);
  GSList *l;

  for (l = priv->keygrabs; l; l = l->next)
    {
      AtspiKeyGrab *grab = l->data;
      if (grab->id == id)
        {
          ATSPI_DEVICE_GET_CLASS (device)->remove_key_grab (device, id);
          priv->keygrabs = g_slist_remove (priv->keygrabs, grab);
          if (grab->callback_destroyed)
            (*grab->callback_destroyed) (grab->callback);
          g_free (grab);
          return;
        }
    }
}

/**
 *atspi_device_add_key_watcher:
 * @device: the device.
 * @callback: (scope notified): the function to call when the given key is
 *            pressed.
 * @user_data: (closure callback): Data to be passed to @callback.
 * @callback_destroyed: (destroy callback): callback function to be called
 *                      when @callback is destroyed.
 *
 * Add a callback that will receive a notification whenever a key is
 * pressed or released.
 */
void
atspi_device_add_key_watcher (AtspiDevice *device, AtspiKeyCallback callback, void *user_data, GDestroyNotify callback_destroyed)
{
  AtspiDevicePrivate *priv = atspi_device_get_instance_private (device);
  AtspiKeyGrab *grab = g_new0 (AtspiKeyGrab, 1);
  grab->id = get_grab_id (device);
  grab->callback = callback;
  grab->callback_data = user_data;
  grab->callback_destroyed = callback_destroyed;
  priv->key_watchers = g_slist_append (priv->key_watchers, grab);
}

AtspiKeyDefinition *
atspi_device_get_grab_by_id (AtspiDevice *device, guint id)
{
  AtspiDevicePrivate *priv = atspi_device_get_instance_private (device);
  GSList *l;

  for (l = priv->keygrabs; l; l = l->next)
    {
      AtspiKeyGrab *grab = l->data;
      if (grab->id == id)
        {
          AtspiKeyDefinition *kd = g_new0 (AtspiKeyDefinition, 1);
          kd->keycode = grab->keycode;
          kd->modifiers = grab->modifiers;
          return kd;
        }
    }
  return NULL;
}

/**
 * atspi_device_map_modifier:
 * @device: the device.
 * @keycode: the keycode to map.
 *
 * Maps the specified key code to a modifier so that it can be used in
 * conjunction with other keys to create a key grab. If the given keycode is
 * already mapped, then this function will return the modifier that is
 * currently mapped to the keycode, without doing anything else. Otherwise,
 * it will use the last modifier that AT-SPI used to map a key. If no keys
 * have yet been mapped using this device, then it will look for a modifier
 * that is not currently being used. If no unused modifier can be found,
 * then it will use the first modifier by default.
 *
 * Returns: the modifier that is now mapped to this keycode. This return
 * value can be passed to atspi_device_add_key_grab.
 */
guint
atspi_device_map_modifier (AtspiDevice *device, gint keycode)
{
  if (ATSPI_DEVICE_GET_CLASS (device)->map_modifier)
    return ATSPI_DEVICE_GET_CLASS (device)->map_modifier (device, keycode);

  return 0;
}

/**
 * atspi_device_unmap_modifier:
 * @device: the device.
 * @keycode: the keycode to unmap.
 *
 * Removes a mapped modifier from the given keycode.
 */
void
atspi_device_unmap_modifier (AtspiDevice *device, gint keycode)
{
  if (ATSPI_DEVICE_GET_CLASS (device)->unmap_modifier)
    ATSPI_DEVICE_GET_CLASS (device)->unmap_modifier (device, keycode);
}

/**
 * atspi_device_get_modifier:
 * @device: the device.
 * @keycode: the keycode to map.
 *
 * Gets the modifier for a given keycode, if one exists. Does not create a new
 * mapping. This function should be used when the intention is to query a
 * locking modifier such as num lock via atspi_device_get_locked_modifiers,
 * rather than to add key grabs.
 *
 * Returns: the modifier that is mapped to this keycode.
 */
guint
atspi_device_get_modifier (AtspiDevice *device, gint keycode)
{
  if (ATSPI_DEVICE_GET_CLASS (device)->get_modifier)
    return ATSPI_DEVICE_GET_CLASS (device)->get_modifier (device, keycode);

  return 0;
}

/**
 * atspi_device_get_locked_modifiers:
 * @device: the device.
 *
 * Returns the locked modifiers (ie, num lock, caps lock) associated with this
 * keyboard.
 *
 * Returns: a guint of modifier flags.
 */
guint
atspi_device_get_locked_modifiers (AtspiDevice *device)
{
  if (ATSPI_DEVICE_GET_CLASS (device)->get_locked_modifiers)
    return ATSPI_DEVICE_GET_CLASS (device)->get_locked_modifiers (device);

  return 0;
}

/**
 * atspi_device_grab_keyboard:
 *
 * Attempts to grab the entire keyboard. This should only be done
 * temporarily, as it may conflict with other applications that also want to
 * grab the keyboard.
 *
 * Returns: #TRUE if successful, #FALSE otherwise.
 */
gboolean
atspi_device_grab_keyboard (AtspiDevice *device)
{
  if (ATSPI_DEVICE_GET_CLASS (device)->grab_keyboard)
    return ATSPI_DEVICE_GET_CLASS (device)->grab_keyboard (device);

  return FALSE;
}

/**
 * atspi_device_ungrab_keyboard:
 *
 * Removes a keyboard grab added via a call to atspi_device_add_keyboard.
 */
void
atspi_device_ungrab_keyboard (AtspiDevice *device)
{
  if (ATSPI_DEVICE_GET_CLASS (device)->ungrab_keyboard)
    ATSPI_DEVICE_GET_CLASS (device)->ungrab_keyboard (device);
}
