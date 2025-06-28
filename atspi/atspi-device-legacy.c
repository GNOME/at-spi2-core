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

#include "atspi-device-legacy.h"
#include "atspi-private.h"

#ifdef HAVE_X11
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XInput2.h>
#endif

typedef struct
{
  guint keycode;
  guint modifier;
} AtspiLegacyKeyModifier;

typedef struct _AtspiDeviceLegacyPrivate AtspiDeviceLegacyPrivate;
struct _AtspiDeviceLegacyPrivate
{
  AtspiDeviceListener *listener;
#ifdef HAVE_X11
  Display *display;
  Window window;
#endif
  GSList *modifiers;
  guint virtual_mods_enabled;
  gboolean keyboard_grabbed;
  unsigned int numlock_physical_mask;
};

GObjectClass *device_legacy_parent_class;

G_DEFINE_TYPE_WITH_CODE (AtspiDeviceLegacy, atspi_device_legacy, ATSPI_TYPE_DEVICE, G_ADD_PRIVATE (AtspiDeviceLegacy))

static guint
find_virtual_mapping (AtspiDeviceLegacy *legacy_device, gint keycode)
{
  AtspiDeviceLegacyPrivate *priv = atspi_device_legacy_get_instance_private (legacy_device);
  GSList *l;

  for (l = priv->modifiers; l; l = l->next)
    {
      AtspiLegacyKeyModifier *entry = l->data;
      if (entry->keycode == keycode)
        return entry->modifier;
    }

  return 0;
}

static void
set_virtual_modifier (AtspiDeviceLegacy *legacy_device, gint keycode, gboolean enabled)
{
  AtspiDeviceLegacyPrivate *priv = atspi_device_legacy_get_instance_private (legacy_device);
  guint modifier = find_virtual_mapping (legacy_device, keycode);

  if (enabled)
    priv->virtual_mods_enabled |= modifier;
  else
    priv->virtual_mods_enabled &= ~modifier;
}

static gboolean
key_cb (AtspiDeviceEvent *event, void *user_data)
{
  AtspiDeviceLegacy *legacy_device = ATSPI_DEVICE_LEGACY (user_data);
  AtspiDeviceLegacyPrivate *priv = atspi_device_legacy_get_instance_private (legacy_device);
  gboolean ret = priv->keyboard_grabbed;
  guint modifiers;

  g_object_ref (legacy_device);
  set_virtual_modifier (legacy_device, event->hw_code,
                        event->type == (AtspiEventType) ATSPI_KEY_PRESS);

  modifiers = event->modifiers | priv->virtual_mods_enabled;
  if (modifiers & (1 << ATSPI_MODIFIER_NUMLOCK))
    modifiers &= ~priv->numlock_physical_mask;

  ret |= atspi_device_notify_key (ATSPI_DEVICE (legacy_device),
                                  event->type == (AtspiEventType) ATSPI_KEY_PRESS,
                                  event->hw_code, event->id,
                                  modifiers,
                                  event->event_string);

  g_boxed_free (ATSPI_TYPE_DEVICE_EVENT, event);
  g_object_unref (legacy_device);
  return ret;
}

static guint
atspi_device_legacy_get_locked_modifiers (AtspiDevice *device)
{
#ifdef HAVE_X11
  AtspiDeviceLegacy *legacy_device = ATSPI_DEVICE_LEGACY (device);
  AtspiDeviceLegacyPrivate *priv = atspi_device_legacy_get_instance_private (legacy_device);
  XkbStateRec state_rec;

  memset (&state_rec, 0, sizeof (state_rec));
  XkbGetState (priv->display, XkbUseCoreKbd, &state_rec);
  return state_rec.locked_mods;
#else
  return 0;
#endif
}

static gboolean
check_virtual_modifier (AtspiDeviceLegacy *legacy_device, guint modifier)
{
  AtspiDeviceLegacyPrivate *priv = atspi_device_legacy_get_instance_private (legacy_device);
  GSList *l;

  if (modifier == (1 << ATSPI_MODIFIER_NUMLOCK))
    return TRUE;

  for (l = priv->modifiers; l; l = l->next)
    {
      AtspiLegacyKeyModifier *entry = l->data;
      if (entry->modifier == modifier)
        return TRUE;
    }

  return FALSE;
}

static guint
get_unused_virtual_modifier (AtspiDeviceLegacy *legacy_device)
{
  guint ret = 0x1000;

  while (ret < 0x10000)
    {
      if (!check_virtual_modifier (legacy_device, ret))
        return ret;
      ret <<= 1;
    }

  return 0;
}

static guint
atspi_device_legacy_map_modifier (AtspiDevice *device, gint keycode)
{
  AtspiDeviceLegacy *legacy_device = ATSPI_DEVICE_LEGACY (device);
  AtspiDeviceLegacyPrivate *priv = atspi_device_legacy_get_instance_private (legacy_device);
  guint ret;
  AtspiLegacyKeyModifier *entry;
#ifdef HAVE_X11
  XkbDescPtr desc;

  desc = XkbGetMap (priv->display, XkbModifierMapMask, XkbUseCoreKbd);

  if (desc)
    {
      if (keycode < desc->min_key_code || keycode >= desc->max_key_code)
        {
          XkbFreeKeyboard (desc, XkbModifierMapMask, TRUE);
          g_warning ("Passed invalid keycode %d", keycode);
          return 0;
        }

      ret = desc->map->modmap[keycode];
      XkbFreeKeyboard (desc, XkbModifierMapMask, TRUE);
      if (ret & (ShiftMask | ControlMask))
        return ret;
    }
#endif

  ret = find_virtual_mapping (legacy_device, keycode);
  if (ret)
    return ret;

  ret = get_unused_virtual_modifier (legacy_device);

  entry = g_new (AtspiLegacyKeyModifier, 1);
  entry->keycode = keycode;
  entry->modifier = ret;
  priv->modifiers = g_slist_append (priv->modifiers, entry);

  return ret;
}

static void
atspi_device_legacy_unmap_modifier (AtspiDevice *device, gint keycode)
{
  AtspiDeviceLegacy *legacy_device = ATSPI_DEVICE_LEGACY (device);
  AtspiDeviceLegacyPrivate *priv = atspi_device_legacy_get_instance_private (legacy_device);
  GSList *l;

  for (l = priv->modifiers; l; l = l->next)
    {
      AtspiLegacyKeyModifier *entry = l->data;
      if (entry->keycode == keycode)
        {
          priv->modifiers = g_slist_remove (priv->modifiers, entry);
          g_free (entry);
          return;
        }
    }
}

static guint
atspi_device_legacy_get_modifier (AtspiDevice *device, gint keycode)
{
  AtspiDeviceLegacy *legacy_device = ATSPI_DEVICE_LEGACY (device);
#ifdef HAVE_X11
  AtspiDeviceLegacyPrivate *priv = atspi_device_legacy_get_instance_private (legacy_device);
  XkbDescPtr desc;
  guint ret;

  desc = XkbGetMap (priv->display, XkbModifierMapMask, XkbUseCoreKbd);

  if (desc)
    {
      if (keycode < desc->min_key_code || keycode >= desc->max_key_code)
        {
          XkbFreeKeyboard (desc, XkbModifierMapMask, TRUE);
          g_warning ("Passed invalid keycode %d", keycode);
          return 0;
        }

      ret = desc->map->modmap[keycode];
      XkbFreeKeyboard (desc, XkbModifierMapMask, TRUE);
      if (ret)
        return ret;
    }
#endif

  return find_virtual_mapping (legacy_device, keycode);
}

static gboolean
atspi_device_legacy_grab_keyboard (AtspiDevice *device)
{
  AtspiDeviceLegacy *legacy_device = ATSPI_DEVICE_LEGACY (device);
  AtspiDeviceLegacyPrivate *priv = atspi_device_legacy_get_instance_private (legacy_device);

  priv->keyboard_grabbed = TRUE;
  return TRUE;
}

static void
atspi_device_legacy_ungrab_keyboard (AtspiDevice *device)
{
  AtspiDeviceLegacy *legacy_device = ATSPI_DEVICE_LEGACY (device);
  AtspiDeviceLegacyPrivate *priv = atspi_device_legacy_get_instance_private (legacy_device);

  priv->keyboard_grabbed = FALSE;
}

static void
atspi_device_legacy_generate_mouse_event (AtspiDevice *device, AtspiAccessible *obj, gint x, gint y, const gchar *name, GError **error)
{
  AtspiPoint *p;

  p = atspi_component_get_position (ATSPI_COMPONENT (obj), ATSPI_COORD_TYPE_SCREEN, error);
  if (p->y == -1 && atspi_accessible_get_role (obj, NULL) == ATSPI_ROLE_APPLICATION)
    {
      g_clear_error (error);
      g_free (p);
      AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
      if (child)
        {
          p = atspi_component_get_position (ATSPI_COMPONENT (child), ATSPI_COORD_TYPE_SCREEN, error);
          g_object_unref (child);
        }
    }

  if (p->y == -1 || p->x == -1)
    return;

  x += p->x;
  y += p->y;
  g_free (p);

  atspi_generate_mouse_event (x, y, name, error);
}

static guint
atspi_device_legacy_map_keysym_modifier (AtspiDevice *device, guint keysym)
{
  AtspiDeviceLegacy *legacy_device = ATSPI_DEVICE_LEGACY (device);
  AtspiDeviceLegacyPrivate *priv = atspi_device_legacy_get_instance_private (legacy_device);

  guint resolved_keysym = keysym;
#ifdef HAVE_X11
  resolved_keysym = XKeysymToKeycode (priv->display, keysym);
#endif

  return atspi_device_legacy_map_modifier (device, resolved_keysym);
}

static void
atspi_device_legacy_unmap_keysym_modifier (AtspiDevice *device, guint keysym)
{
  AtspiDeviceLegacy *legacy_device = ATSPI_DEVICE_LEGACY (device);
  AtspiDeviceLegacyPrivate *priv = atspi_device_legacy_get_instance_private (legacy_device);

  guint resolved_keysym = keysym;
#ifdef HAVE_X11
  resolved_keysym = XKeysymToKeycode (priv->display, keysym);
#endif

  atspi_device_legacy_unmap_modifier (device, resolved_keysym);
}

static guint
atspi_device_legacy_get_keysym_modifier (AtspiDevice *device, guint keysym)
{
  AtspiDeviceLegacy *legacy_device = ATSPI_DEVICE_LEGACY (device);
  AtspiDeviceLegacyPrivate *priv = atspi_device_legacy_get_instance_private (legacy_device);

  guint resolved_keysym = keysym;
#ifdef HAVE_X11
  resolved_keysym = XKeysymToKeycode (priv->display, keysym);
#endif

  return atspi_device_legacy_get_modifier (device, resolved_keysym);
}

static void
atspi_device_legacy_init (AtspiDeviceLegacy *device)
{
  AtspiDeviceLegacyPrivate *priv = atspi_device_legacy_get_instance_private (device);
  gint i;

  priv->listener = atspi_device_listener_new (key_cb, device, NULL);
  for (i = 0; i < 256; i++)
    atspi_register_keystroke_listener (priv->listener, NULL, i, 3, ATSPI_KEYLISTENER_SYNCHRONOUS | ATSPI_KEYLISTENER_CANCONSUME, NULL);

#ifdef HAVE_X11
  priv->display = XOpenDisplay ("");
  if (priv->display)
    priv->window = DefaultRootWindow (priv->display);
  priv->numlock_physical_mask = XkbKeysymToModifiers (priv->display,
                                                      XK_Num_Lock);
#endif
}

static void
atspi_device_legacy_finalize (GObject *object)
{
  AtspiDeviceLegacy *device = ATSPI_DEVICE_LEGACY (object);
  AtspiDeviceLegacyPrivate *priv = atspi_device_legacy_get_instance_private (device);

  g_clear_object (&priv->listener);
  device_legacy_parent_class->finalize (object);
}

static void
atspi_device_legacy_class_init (AtspiDeviceLegacyClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;
  AtspiDeviceClass *device_class = ATSPI_DEVICE_CLASS (klass);

  device_legacy_parent_class = g_type_class_peek_parent (klass);
  object_class->finalize = atspi_device_legacy_finalize;
  device_class->map_modifier = atspi_device_legacy_map_modifier;
  device_class->unmap_modifier = atspi_device_legacy_unmap_modifier;
  device_class->get_modifier = atspi_device_legacy_get_modifier;
  device_class->get_locked_modifiers = atspi_device_legacy_get_locked_modifiers;
  device_class->grab_keyboard = atspi_device_legacy_grab_keyboard;
  device_class->ungrab_keyboard = atspi_device_legacy_ungrab_keyboard;
  device_class->generate_mouse_event = atspi_device_legacy_generate_mouse_event;
  device_class->map_keysym_modifier = atspi_device_legacy_map_keysym_modifier;
  device_class->unmap_keysym_modifier = atspi_device_legacy_unmap_keysym_modifier;
  device_class->get_keysym_modifier = atspi_device_legacy_get_keysym_modifier;
}

/**
 * atspi_device_legacy_new_full:
 * @app_id: (nullable): The application id.
 *
 * Creates a new #AtspiDeviceLegacy with the given app id.
 *
 * Returns: (transfer full): a pointer to a newly-created #AtspiDeviceLegacy.
 *
 * Since: 2.55
 */
AtspiDeviceLegacy *
atspi_device_legacy_new_full (const gchar *app_id)
{
  AtspiDeviceLegacy *device = g_object_new (atspi_device_legacy_get_type (), "app-id", app_id, NULL);

  return device;
}

/**
 * atspi_device_legacy_new:
 *
 * Creates a new #AtspiDeviceLegacy.
 *
 * Returns: (transfer full): a pointer to a newly-created #AtspiDeviceLegacy.
 *
 */
AtspiDeviceLegacy *
atspi_device_legacy_new (void)
{
  return atspi_device_legacy_new_full (NULL);
}
