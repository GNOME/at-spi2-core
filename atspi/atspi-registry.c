
/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
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

/* atspi_registry.c: Global functions wrapping the registry */

#include "atspi-private.h"

/**
 * AtspiRegistry:
 *
 * A service through which applications providing accessibility services
 * can rendezvous with consumers of those services.
 *
 * A service through which applications providing accessibility services (servers)
 * can rendezvous with consumers of those services (Assistive Technologies). The
 * atspi-registry is the first "port of call" for accessible applications and for
 * assistive technologies wishing to query and interact with those applications.
 */

typedef struct
{
  AtspiDeviceListener *listener;
  GArray *key_set;
  AtspiKeyMaskType modmask;
  AtspiKeyEventMask event_types;
  gint sync_type;
} DeviceListenerEntry;

static GList *device_listeners;

/**
 * atspi_get_desktop_count:
 *
 * Gets the number of virtual desktops.
 * NOTE: multiple virtual desktops are not implemented yet; as a
 * consequence, this function always returns 1.
 *
 * Returns: a #gint indicating the number of active virtual desktops.
 **/
gint
atspi_get_desktop_count ()
{
  return 1;
}

/**
 * atspi_get_desktop:
 * @i: a #gint indicating which of the accessible desktops is to be returned.
 *
 * Gets the virtual desktop indicated by index @i.
 * NOTE: currently multiple virtual desktops are not implemented;
 * as a consequence, any @i value different from 0 will not return a
 * virtual desktop - instead it will return NULL.
 *
 * Returns: (transfer full): a pointer to the @i-th virtual desktop's
 * #AtspiAccessible representation.
 **/
AtspiAccessible *
atspi_get_desktop (gint i)
{
  if (i != 0)
    return NULL;
  return _atspi_ref_accessible (atspi_bus_registry, atspi_path_root);
}

/**
 * atspi_get_desktop_list:
 *
 * Gets the list of virtual desktops.  On return, @list will point
 *     to a newly-created, NULL terminated array of virtual desktop
 *     pointers.
 *     It is the responsibility of the caller to free this array when
 *     it is no longer needed.
 * NOTE: currently multiple virtual desktops are not implemented;
 * this implementation always returns a #Garray with a single
 * #AtspiAccessible desktop.
 *
 * Returns: (element-type AtspiAccessible*) (transfer full): a #GArray of
 * desktops.
 **/
GArray *
atspi_get_desktop_list ()
{
  GArray *array = g_array_new (TRUE, TRUE, sizeof (AtspiAccessible *));
  AtspiAccessible *desktop;

  desktop = _atspi_ref_accessible (atspi_bus_registry, atspi_path_root);
  if (array)
    g_array_index (array, AtspiAccessible *, 0) = desktop;
  return array;
}

static gboolean
notify_keystroke_listener (DeviceListenerEntry *e)
{
  gchar *path = _atspi_device_listener_get_path (e->listener);
  dbus_uint32_t d_modmask = e->modmask;
  dbus_uint32_t d_event_types = e->event_types;
  AtspiEventListenerMode listener_mode;
  gboolean retval = FALSE;
  DBusError d_error;

  listener_mode.synchronous =
      (dbus_bool_t) ((e->sync_type & ATSPI_KEYLISTENER_SYNCHRONOUS) != 0);
  listener_mode.preemptive =
      (dbus_bool_t) ((e->sync_type & ATSPI_KEYLISTENER_CANCONSUME) != 0);
  listener_mode.global =
      (dbus_bool_t) ((e->sync_type & ATSPI_KEYLISTENER_ALL_WINDOWS) != 0);

  dbus_error_init (&d_error);
  dbind_method_call_reentrant (_atspi_bus (), atspi_bus_registry,
                               atspi_path_dec, atspi_interface_dec,
                               "RegisterKeystrokeListener", &d_error,
                               "oa(iisi)uu(bbb)=>b", path, e->key_set,
                               d_modmask, d_event_types, &listener_mode,
                               &retval);
  if (dbus_error_is_set (&d_error))
    {
      g_warning ("RegisterKeystrokeListener failed: %s", d_error.message);
      dbus_error_free (&d_error);
    }

  g_free (path);

  return retval;
}

static void
device_listener_entry_free (DeviceListenerEntry *e)
{
  g_array_free (e->key_set, TRUE);
  g_free (e);
}

static void
unregister_listener (gpointer data, GObject *object)
{
  GList *l;
  AtspiDeviceListener *listener = ATSPI_DEVICE_LISTENER (object);

  for (l = device_listeners; l;)
    {
      DeviceListenerEntry *e = l->data;
      if (e->listener == listener)
        {
          GList *next = l->next;
          device_listener_entry_free (e);
          device_listeners = g_list_delete_link (device_listeners, l);
          l = next;
        }
      else
        l = l->next;
    }
}

/**
 * atspi_register_keystroke_listener:
 * @listener:  a pointer to the #AtspiDeviceListener for which
 *             keystroke events are requested.
 * @key_set: (element-type AtspiKeyDefinition) (allow-none): a pointer to the
 *        #AtspiKeyDefinition array indicating which keystroke events are
 *        requested, or NULL
 *        to indicate that all keycodes and keyvals for the specified
 *        modifier set are to be included.
 * @modmask:   an #AtspiKeyMaskType mask indicating which
 *             key event modifiers must be set in combination with @keys,
 *             events will only be reported for key events for which all
 *             modifiers in @modmask are set.  If you wish to listen for
 *             events with multiple modifier combinations, you must call
 *             #atspi_register_keystroke_listener once for each
 *             combination.
 * @event_types: an #AtspiKeyMaskType mask indicating which
 *             types of key events are requested (%ATSPI_KEY_PRESSED etc.).
 * @sync_type: an #AtspiKeyListenerSyncType parameter indicating
 *             the behavior of the notification/listener transaction.
 * @error: (allow-none): a pointer to a %NULL #GError pointer, or %NULL
 *
 * Registers a listener for keystroke events, either pre-emptively for
 *             all windows (%ATSPI_KEYLISTENER_ALL_WINDOWS),
 *             non-preemptively (%ATSPI_KEYLISTENER_NOSYNC), or
 *             pre-emptively at the toolkit level (%ATSPI_KEYLISTENER_CANCONSUME).
 *             If ALL_WINDOWS or CANCONSUME are used, the event is consumed
 *             upon receipt if one of @listener's callbacks returns %TRUE
 *             (other sync_type values may be available in the future).
 *
 * Returns: %TRUE if successful, otherwise %FALSE.
 **/
gboolean
atspi_register_keystroke_listener (AtspiDeviceListener *listener,
                                   GArray *key_set,
                                   AtspiKeyMaskType modmask,
                                   AtspiKeyEventMask event_types,
                                   AtspiKeyListenerSyncType sync_type,
                                   GError **error)
{
  DeviceListenerEntry *e;

  g_return_val_if_fail (listener != NULL, FALSE);

  e = g_new0 (DeviceListenerEntry, 1);
  e->listener = listener;
  e->modmask = modmask;
  e->event_types = event_types;
  e->sync_type = sync_type;
  if (key_set)
    {
      gint i;
      e->key_set = g_array_sized_new (FALSE, TRUE, sizeof (AtspiKeyDefinition),
                                      key_set->len);
      e->key_set->len = key_set->len;
      for (i = 0; i < key_set->len; i++)
        {
          AtspiKeyDefinition *kd = ((AtspiKeyDefinition *) key_set->data) + i;
          AtspiKeyDefinition *d_kd = ((AtspiKeyDefinition *) e->key_set->data) + i;
          d_kd->keycode = kd->keycode;
          d_kd->keysym = kd->keysym;
          if (kd->keystring)
            {
              d_kd->keystring = kd->keystring;
            }
          else
            {
              d_kd->keystring = "";
            }
        }
    }
  else
    {
      e->key_set = g_array_sized_new (FALSE, TRUE, sizeof (AtspiKeyDefinition), 0);
    }

  g_object_weak_ref (G_OBJECT (listener), unregister_listener, NULL);
  device_listeners = g_list_prepend (device_listeners, e);
  return notify_keystroke_listener (e);
}

/**
 * atspi_deregister_keystroke_listener:
 * @listener: a pointer to the #AtspiDeviceListener for which
 *            keystroke events are requested.
 * @key_set: (element-type AtspiKeyDefinition) (allow-none): a pointer to the
 *        #AtspiKeyDefinition array indicating which keystroke events are
 *        requested, or %NULL
 *        to indicate that all keycodes and keyvals for the specified
 *        modifier set are to be included.
 * @modmask:  the key modifier mask for which this listener is to be
 *            'deregistered' (of type #AtspiKeyMaskType).
 * @event_types: an #AtspiKeyMaskType mask indicating which
 *             types of key events were requested (%ATSPI_KEY_PRESSED, etc.).
 * @error: (allow-none): a pointer to a %NULL #GError pointer, or %NULL
 *
 * Removes a keystroke event listener from the registry's listener queue,
 *            ceasing notification of events with modifiers matching @modmask.
 *
 * Returns: %TRUE if successful, otherwise %FALSE.
 **/
gboolean
atspi_deregister_keystroke_listener (AtspiDeviceListener *listener,
                                     GArray *key_set,
                                     AtspiKeyMaskType modmask,
                                     AtspiKeyEventMask event_types,
                                     GError **error)
{
  GArray *d_key_set;
  gchar *path;
  gint i;
  dbus_uint32_t d_modmask = modmask;
  dbus_uint32_t d_event_types = event_types;
  DBusError d_error;
  GList *l;

  if (!listener)
    {
      return FALSE;
    }

  dbus_error_init (&d_error);

  path = _atspi_device_listener_get_path (listener);

  /* copy the keyval filter values from the C api into the DBind KeySet */
  if (key_set)
    {
      d_key_set = g_array_sized_new (FALSE, TRUE, sizeof (AtspiKeyDefinition), key_set->len);
      d_key_set->len = key_set->len;
      for (i = 0; i < key_set->len; ++i)
        {
          AtspiKeyDefinition *kd = ((AtspiKeyDefinition *) key_set->data) + i;
          AtspiKeyDefinition *d_kd = ((AtspiKeyDefinition *) d_key_set->data) + i;
          d_kd->keycode = kd->keycode;
          d_kd->keysym = kd->keysym;
          if (kd->keystring)
            {
              d_kd->keystring = kd->keystring;
            }
          else
            {
              d_kd->keystring = "";
            }
        }
    }
  else
    {
      d_key_set = g_array_sized_new (FALSE, TRUE, sizeof (AtspiKeyDefinition), 0);
    }

  dbind_method_call_reentrant (_atspi_bus (), atspi_bus_registry,
                               atspi_path_dec, atspi_interface_dec,
                               "DeregisterKeystrokeListener", &d_error,
                               "oa(iisi)uu", path, d_key_set, d_modmask,
                               d_event_types);
  if (dbus_error_is_set (&d_error))
    {
      g_warning ("DeregisterKeystrokeListener failed: %s", d_error.message);
      dbus_error_free (&d_error);
    }

  unregister_listener (listener, NULL);
  for (l = device_listeners; l;)
    {
      /* TODO: This code is all wrong / doesn't match what is in
       *       deviceeventcontroller.c. It would be nice to deprecate these methods
       *       in favor of methods that return an ID for the registration that can
       *       be passed to a deregister function, for instance. */
      DeviceListenerEntry *e = l->data;
      if (e->modmask == modmask && e->event_types == event_types)
        {
          GList *next = l->next;
          device_listener_entry_free (e);
          device_listeners = g_list_delete_link (device_listeners, l);
          l = next;
        }
      else
        l = l->next;
    }
  g_array_free (d_key_set, TRUE);
  g_free (path);
  return TRUE;
}

/**
 * atspi_register_device_event_listener:
 *
 * This function does nothing and should not be called.
 *
 * Returns: Always returns %FALSE.
 **/
gboolean
atspi_register_device_event_listener (AtspiDeviceListener *listener,
                                      AtspiDeviceEventMask event_types,
                                      void *filter,
                                      GError **error)
{
  /* See https://gitlab.gnome.org/GNOME/at-spi2-core/-/issues/94 for why this code is removed */
  return FALSE;
}

/**
 * atspi_deregister_device_event_listener:
 * @listener: a pointer to the #AtspiDeviceListener for which
 *            device events are requested.
 * @filter: (allow-none): Unused parameter.
 * @error: (allow-none): a pointer to a %NULL #GError pointer, or %NULL
 *
 * Removes a device event listener from the registry's listener queue,
 *            ceasing notification of events of the specified type.
 *
 * Returns: %TRUE if successful, otherwise %FALSE.
 **/
gboolean
atspi_deregister_device_event_listener (AtspiDeviceListener *listener,
                                        void *filter,
                                        GError **error)
{
  /* See https://gitlab.gnome.org/GNOME/at-spi2-core/-/issues/94 for why this code is removed */
  return FALSE;
}

static gboolean
using_mutter ()
{
  return (g_getenv ("WAYLAND_DISPLAY") != NULL);
}

/**
 * atspi_generate_keyboard_event:
 * @keyval: a #gint indicating the keycode or keysym or modifier mask of the
 *           key event being synthesized.
 * @keystring: (allow-none): an (optional) UTF-8 string which, if
 *           @synth_type is %ATSPI_KEY_STRING, indicates a 'composed'
 *           keyboard input string being synthesized; this type of
 *           keyboard event synthesis does not emulate hardware
 *           keypresses but injects the string as though a composing
 *           input method (such as XIM) were used.
 * @synth_type: an #AtspiKeySynthType flag indicating whether @keyval
 *           is to be interpreted as a keysym rather than a keycode
 *           (%ATSPI_KEY_SYM) or a string (%ATSPI_KEY_STRING) or a modifier
 *           mask (%ATSPI_KEY_LOCKMODIFIERS and %ATSPI_KEY_UNLOCKMODIFIERS), or
 *           whether to synthesize %ATSPI_KEY_PRESS,
 *           %ATSPI_KEY_RELEASE, or both (%ATSPI_KEY_PRESSRELEASE).
 * @error: (allow-none): a pointer to a %NULL #GError pointer, or %NULL
 *
 * Synthesizes a keyboard event (as if a hardware keyboard event occurred in the
 * current UI context).
 *
 * Returns: %TRUE if successful, otherwise %FALSE.
 **/
gboolean
atspi_generate_keyboard_event (glong keyval,
                               const gchar *keystring,
                               AtspiKeySynthType synth_type,
                               GError **error)
{
  dbus_uint32_t d_synth_type = synth_type;
  dbus_int32_t d_keyval = keyval;
  DBusError d_error;

  if (using_mutter ())
    {
      if (_atspi_mutter_generate_keyboard_event (keyval, keystring, synth_type, error))
        return TRUE;
    }

  dbus_error_init (&d_error);
  if (!keystring)
    keystring = "";
  dbind_method_call_reentrant (_atspi_bus (), atspi_bus_registry, atspi_path_dec, atspi_interface_dec, "GenerateKeyboardEvent", &d_error, "isu", d_keyval, keystring, d_synth_type);
  if (dbus_error_is_set (&d_error))
    {
      g_warning ("GenerateKeyboardEvent failed: %s", d_error.message);
      dbus_error_free (&d_error);
    }

  return TRUE;
}

/**
 * atspi_generate_mouse_event:
 * @x: a #glong indicating the screen x coordinate of the mouse event.
 * @y: a #glong indicating the screen y coordinate of the mouse event.
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
 * Returns: %TRUE if successful, otherwise %FALSE.
 **/
gboolean
atspi_generate_mouse_event (glong x, glong y, const gchar *name, GError **error)
{
  dbus_int32_t d_x = x, d_y = y;
  DBusError d_error;

  g_return_val_if_fail (name != NULL, FALSE);

  if (using_mutter ())
    {
      if (_atspi_mutter_generate_mouse_event (x, y, name, error))
        return TRUE;
    }

  dbus_error_init (&d_error);
  dbind_method_call_reentrant (_atspi_bus (), atspi_bus_registry,
                               atspi_path_dec, atspi_interface_dec,
                               "GenerateMouseEvent", &d_error, "iis",
                               d_x, d_y, name);
  if (dbus_error_is_set (&d_error))
    {
      g_warning ("GenerateMouseEvent failed: %s", d_error.message);
      dbus_error_free (&d_error);
    }

  return TRUE;
}

/**
 * atspi_set_reference_window:
 * @accessible: the #AtspiAccessible corresponding to the window to select.
 *              should be a top-level window with a role of
 *              ATSPI_ROLE_APPLICATION.
 *
 * Sets the reference window that will be used when atspi_generate_mouse_event
 * is called. Coordinates will be assumed to be relative to this window. This
 * is needed because, due to Wayland's security model, it is not currently
 * possible to retrieve global coordinates.
 * If NULL is passed, then AT-SPI will use the window that has focus at the
 * time that atspi_generate_mouse_event is called.
 */
void
atspi_set_reference_window (AtspiAccessible *accessible)
{
  if (using_mutter ())
    _atspi_mutter_set_reference_window (accessible);
}

AtspiKeyDefinition *
atspi_key_definition_copy (AtspiKeyDefinition *src)
{
  AtspiKeyDefinition *dst;

  dst = g_new0 (AtspiKeyDefinition, 1);
  dst->keycode = src->keycode;
  dst->keysym = src->keysym;
  if (src->keystring)
    dst->keystring = g_strdup (src->keystring);
  dst->modifiers = src->modifiers;
  return dst;
}

void
atspi_key_definition_free (AtspiKeyDefinition *kd)
{
  if (kd->keystring)
    g_free (kd->keystring);
  g_free (kd);
}

void
_atspi_reregister_device_listeners ()
{
  GList *l;
  DeviceListenerEntry *e;

  for (l = device_listeners; l; l = l->next)
    {
      e = l->data;
      notify_keystroke_listener (e);
    }
}
G_DEFINE_BOXED_TYPE (AtspiKeyDefinition, atspi_key_definition, atspi_key_definition_copy, atspi_key_definition_free)
