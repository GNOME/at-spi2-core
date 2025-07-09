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
#include "atspi-device-a11y-manager.h"
#include "atspi-device-legacy.h"
#include "atspi-device-x11.h"
#include "atspi-private.h"

enum {
  PROP_0,
  PROP_APP_ID,
  NUM_PROPERTIES
};

static GParamSpec *obj_props[NUM_PROPERTIES];

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
  gchar *app_id;
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

#if !GLIB_CHECK_VERSION(2, 76, 0)
static inline gboolean
g_set_str (char **str_pointer,
           const char *new_str)
{
  char *copy;

  if (*str_pointer == new_str ||
      (*str_pointer && new_str && strcmp (*str_pointer, new_str) == 0))
    return FALSE;

  copy = g_strdup (new_str);
  g_free (*str_pointer);
  *str_pointer = copy;

  return TRUE;
}
#endif

static void
atspi_device_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  AtspiDevice *device = ATSPI_DEVICE (object);

  switch (prop_id)
    {
    case PROP_APP_ID:
      g_value_set_string (value, atspi_device_get_app_id (device));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
atspi_device_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  AtspiDevice *device = ATSPI_DEVICE (object);

  switch (prop_id)
    {
    case PROP_APP_ID:
      atspi_device_set_app_id (device, g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
atspi_device_class_init (AtspiDeviceClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;

  device_parent_class = g_type_class_peek_parent (klass);
  klass->add_key_grab = atspi_device_real_add_key_grab;
  klass->remove_key_grab = atspi_device_real_remove_key_grab;
  object_class->finalize = atspi_device_finalize;
  object_class->get_property = atspi_device_get_property;
  object_class->set_property = atspi_device_set_property;

  /**
   * AtspiDevice:app-id:
   *
   * The application ID of the application that created this device.
   * The ID might be used for access control purposes
   * by some device backends.
   */
  obj_props[PROP_APP_ID] =
    g_param_spec_string ("app-id",
                         "Application ID",
                         "The application ID of the application that created this device",
                         NULL,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, NUM_PROPERTIES, obj_props);
                        }

/**
 * atspi_device_new_full:
 * @app_id: (nullable): The application ID of the application that created this device.
 *
 * Creates a new #AtspiDevice with a specified app id.
 *
 * Returns: (transfer full): a pointer to a newly-created #AtspiDevice.
 *
 * Since: 2.55
 */
AtspiDevice *
atspi_device_new_full (const gchar *app_id)
{
#ifdef HAVE_X11
  if (!g_getenv ("WAYLAND_DISPLAY") && !g_getenv ("ATSPI_USE_LEGACY_DEVICE") && !g_getenv ("ATSPI_USE_A11Y_MANAGER_DEVICE"))
    return ATSPI_DEVICE (atspi_device_x11_new_full (app_id));
#endif

  AtspiDeviceA11yManager *a11y_manager_device = NULL;
  if (!g_getenv ("ATSPI_USE_LEGACY_DEVICE"))
    a11y_manager_device = atspi_device_a11y_manager_try_new_full (app_id);

  if (!a11y_manager_device)
    {
      if (g_getenv ("ATSPI_USE_A11Y_MANAGER_DEVICE"))
        g_critical ("ATSPI_USE_A11Y_MANAGER_DEVICE is set, but no a11y manager device could be created. Falling back to legacy device.");
      return ATSPI_DEVICE (atspi_device_legacy_new_full (app_id));
    }
  else
    {
      return ATSPI_DEVICE (a11y_manager_device);
    }
}

/**
 * atspi_device_new:
 *
 * Creates a new #AtspiDevice.
 *
 * Returns: (transfer full): a pointer to a newly-created #AtspiDevice.
 */
AtspiDevice *
atspi_device_new ()
{
  return atspi_device_new_full (NULL);
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
atspi_device_notify_key (AtspiDevice *device, gboolean pressed, int keycode, int keysym, gint state, const gchar *text)
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
 * @callback: (scope notified) (closure user_data) (destroy callback_destroyed): the
 *   function to call when the given key is pressed.
 * @user_data: Data to be passed to @callback.
 * @callback_destroyed: callback function to be called when @callback is destroyed.
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
          kd->keysym = grab->keysym;
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

/**
 * atspi_device_generate_mouse_event:
 * @device: the device.
 * @obj: The #AtspiAccessible that should receive the click.
 * @x: a #gint indicating the x coordinate of the mouse event, relative to
 *     @obj..
 * @y: a #gint indicating the y coordinate of the mouse event, relative to
 *     @obj..
 * @name: a string indicating which mouse event to be synthesized
 *        (e.g. "b1p", "b1c", "b2r", "rel", "abs").
 * @error: (allow-none): a pointer to a %NULL #GError pointer, or %NULL
 *
 * Synthesizes a mouse event at a specific screen coordinate.
 * Most AT clients should use the #AccessibleAction interface when
 * tempted to generate mouse events, rather than this method.
 * Event names: b1p = button 1 press; b2r = button 2 release;
 *              b3c = button 3 click; b2d = button 2 double-click;
 *              abs = absolute motion; rel = relative motion.
 *
 * Since: 2.52
 **/
void
atspi_device_generate_mouse_event (AtspiDevice *device, AtspiAccessible *obj, gint x, gint y, const gchar *name, GError **error)
{
  if (ATSPI_DEVICE_GET_CLASS (device)->generate_mouse_event)
    ATSPI_DEVICE_GET_CLASS (device)->generate_mouse_event (device, obj, x, y, name, error);
}

/**
 * atspi_device_map_keysym_modifier:
 * @device: the device.
 * @keysym: the XKB keysym to map.
 *
 * Maps the specified keysym to a modifier so that it can be used in
 * conjunction with other keys to create a key grab. If the given keysym is
 * already mapped, then this function will return the modifier that is
 * currently mapped to the keysym, without doing anything else. Otherwise,
 * it will use the last modifier that AT-SPI used to map a keysym. If no keys
 * have yet been mapped using this device, then it will look for a modifier
 * that is not currently being used. If no unused modifier can be found,
 * then it will use the first modifier by default.
 *
 * Returns: the modifier that is now mapped to this keysym. This return
 * value can be passed to atspi_device_add_key_grab.
 *
 * Since: 2.55
 */
guint
atspi_device_map_keysym_modifier (AtspiDevice *device, guint keysym)
{
  if (ATSPI_DEVICE_GET_CLASS (device)->map_keysym_modifier)
    return ATSPI_DEVICE_GET_CLASS (device)->map_keysym_modifier (device, keysym);

  return 0;
}

/**
 * atspi_device_unmap_keysym_modifier:
 * @device: the device.
 * @keysym: the XKB keysym to unmap.
 *
 * Removes a mapped modifier from the given keysym.
 *
 * Since: 2.55
 */
void
atspi_device_unmap_keysym_modifier (AtspiDevice *device, guint keysym)
{
  if (ATSPI_DEVICE_GET_CLASS (device)->unmap_keysym_modifier)
    ATSPI_DEVICE_GET_CLASS (device)->unmap_keysym_modifier (device, keysym);
}

/**
 * atspi_device_get_keysym_modifier:
 * @device: the device.
 * @keysym: the XKB keysym to map.
 *
 * Gets the modifier for a given keysym, if one exists. Does not create a new
 * mapping. This function should be used when the intention is to query a
 * locking modifier such as num lock via atspi_device_get_locked_modifiers,
 * rather than to add key grabs.
 *
 * Returns: the modifier that is mapped to this keysym.
 *
 * Since: 2.55
 */
guint
atspi_device_get_keysym_modifier (AtspiDevice *device, guint keysym)
{
  if (ATSPI_DEVICE_GET_CLASS (device)->get_keysym_modifier)
    return ATSPI_DEVICE_GET_CLASS (device)->get_keysym_modifier (device, keysym);

  return 0;
}

/**
 * atspi_device_get_app_id:
 * @device: the device.
 * 
 * Returns the application ID of the device.
 * 
 * Since: 2.55
 */
const gchar *
atspi_device_get_app_id (AtspiDevice *device)
{
  AtspiDevicePrivate *priv = atspi_device_get_instance_private (device);
  return priv->app_id;
}

/**
 * atspi_device_set_app_id:
 * @device: the device.
 * @app_id: the application ID.
 * 
 * Sets the application ID of the device.
 * 
 * Since: 2.55
 */
void
atspi_device_set_app_id (AtspiDevice *device, const gchar *app_id)
{
  AtspiDevicePrivate *priv = atspi_device_get_instance_private (device);
  if (g_set_str (&priv->app_id, app_id))
    g_object_notify_by_pspec (G_OBJECT (device), obj_props[PROP_APP_ID]);
}

/*
 * atspi_device_clear_key_grabs:
 *
 * Removes all key grabs from a device.
 *
 * Since: 2.58
 */
void
atspi_device_clear_key_grabs (AtspiDevice *device)
{
  AtspiDevicePrivate *priv = atspi_device_get_instance_private (device);

  while (priv->keygrabs != NULL)
    {
      AtspiKeyGrab *grab = priv->keygrabs->data;
      atspi_device_remove_key_grab (device, grab->id);
    }
}
