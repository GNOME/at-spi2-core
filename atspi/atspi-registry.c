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

/* atspi_registry.c: Global functions wrapping the registry */

#include "atspi-private.h"

static GArray *desktops;

/**
 * atspi_get_desktop_count:
 *
 * Get the number of virtual desktops.
 * NOTE: currently multiple virtual desktops are not implemented, this
 *       function always returns '1'.
 *
 * Returns: an integer indicating the number of active virtual desktops.
 **/
gint
atspi_get_desktop_count ()
{
  return 1;
}

/**
 * atspi_get_desktop:
 * @i: an integer indicating which of the accessible desktops is to be returned.
 *
 * Get the virtual desktop indicated by index @i.
 * NOTE: currently multiple virtual desktops are not implemented.
 *
 * Returns: (transfer full): a pointer to the 'i-th' virtual desktop's
 * #AtspiAccessible representation.
 **/
AtspiAccessible*
atspi_get_desktop (gint i)
{
  if (i != 0) return NULL;
  return _atspi_ref_accessible (atspi_bus_registry, atspi_path_root);
}

/**
 * atspi_get_desktop_list:
 *
 * Get the list of virtual desktops.  On return, @list will point
 *     to a newly-created, NULL terminated array of virtual desktop
 *     pointers.
 *     It is the responsibility of the caller to free this array when
 *     it is no longer needed.
 *
 * Not Yet Implemented : this implementation always returns a single
 * #Accessible desktop.
 *
 * Returns: (transfer full): a #GArray of desktops.
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

/**
 * ATSPI_KEYSET_ALL_KEYS:
 * @ATSPI_KEYSET_ALL_KEYS: A special value for an AtspiKeySet type, which tacitly
 *                       includes all keycodes and keyvals for the specified modifier set.
 **/

/**
 * atspi_register_keystroke_listener:
 * @listener:  a pointer to the #AtspiDeviceListener for which
 *             keystroke events are requested.
 * @key_set: (element-type AtspiKeyDefinition): a pointer to the
 *        #AtspiKeyDefinition array indicating which keystroke events are requested, or #ATSPI_KEYSET_ALL_KEYS
 *             to indicate that all keycodes and keyvals for the specified
 *             modifier set are to be included.
 * @modmask:   an #AtspiKeyMaskType mask indicating which
 *             key event modifiers must be set in combination with @keys,
 *             events will only be reported for key events for which all
 *             modifiers in @modmask are set.  If you wish to listen for
 *             events with multiple modifier combinations you must call
 *             register_keystroke_listener() once for each
 *             combination.
 * @eventmask: an #AtspiKeyMaskType mask indicating which
 *             types of key events are requested (#ATSPI_KEY_PRESSED, etc.).
 * @sync_type: a #AtspiKeyListenerSyncType parameter indicating
 *             the behavior of the notification/listener transaction.
 *             
 * Register a listener for keystroke events, either pre-emptively for
 *             all windows (ATSPI_KEYLISTENER_ALL_WINDOWS),
 *             non-preemptively (ATSPI_KEYLISTENER_NOSYNC), or
 *             pre-emptively at the toolkit level (ATSPI_KEYLISTENER_CANCONSUME).
 *             If ALL_WINDOWS or CANCONSUME are used, the event is consumed
 *             upon receipt if one of @listener's callbacks returns #TRUE.
 *             ( Other sync_type values may be available in the future )
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
gboolean
atspi_register_keystroke_listener (AtspiDeviceListener  *listener,
					 GArray             *key_set,
					 AtspiKeyMaskType         modmask,
					 AtspiKeyEventMask        event_types,
					 AtspiKeyListenerSyncType sync_type, GError **error)
{
  GArray *d_key_set;
  gchar *path = _atspi_device_listener_get_path (listener);
  gint                                i;
  dbus_uint32_t d_modmask = modmask;
  dbus_uint32_t d_event_types = event_types;
  AtspiEventListenerMode     listener_mode;
  gboolean                          retval = FALSE;
  DBusError d_error;

  if (!listener)
    {
      return retval;
    }

  /* copy the keyval filter values from the C api into the DBind KeySet */
  if (key_set)
    {
      d_key_set = g_array_sized_new (FALSE, TRUE, sizeof (AtspiKeyDefinition), key_set->len);
      d_key_set->len = key_set->len;
      for (i = 0; i < key_set->len; ++i)
        {
	  AtspiKeyDefinition *kd =  ((AtspiKeyDefinition *) key_set->data) + i;
	  AtspiKeyDefinition *d_kd =  ((AtspiKeyDefinition *) d_key_set->data) + i;
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
	
  listener_mode.synchronous =
	  (dbus_bool_t) ((sync_type & ATSPI_KEYLISTENER_SYNCHRONOUS)!=0);
  listener_mode.preemptive =
	  (dbus_bool_t) ((sync_type & ATSPI_KEYLISTENER_CANCONSUME)!=0);
  listener_mode.global =
	  (dbus_bool_t) ((sync_type & ATSPI_KEYLISTENER_ALL_WINDOWS)!=0);

    dbus_error_init (&d_error);
    dbind_method_call_reentrant (_atspi_bus(), atspi_bus_registry, atspi_path_dec, atspi_interface_dec, "RegisterKeystrokeListener", &d_error, "oa(iisi)uu(bbb)=>b", path, d_key_set, d_modmask, d_event_types, &listener_mode, &retval);

  g_array_free (d_key_set, TRUE);
  g_free (path);

  return retval;
}

/**
 * atspi_deregister_keystroke_listener:
 * @listener: a pointer to the #AtspiDeviceListener for which
 *            keystroke events are requested.
 * @modmask:  the key modifier mask for which this listener is to be
 *            'deregistered' (of type #AtspiKeyMaskType).
 *
 * Removes a keystroke event listener from the registry's listener queue,
 *            ceasing notification of events with modifiers matching @modmask.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
gboolean
atspi_deregister_keystroke_listener (AtspiDeviceListener *listener,
					   AtspiKeyMaskType        modmask, AtspiKeyEventMask event_types, GError **error)
{
  gchar *path = _atspi_device_listener_get_path (listener);
  GArray *key_set;
  dbus_uint32_t d_modmask = modmask;
  dbus_uint32_t d_event_types = event_types;
  DBusError d_error;

  dbus_error_init (&d_error);
  if (!listener)
    {
      return FALSE;
    }

      key_set = g_array_sized_new (FALSE, TRUE, sizeof (AtspiKeyDefinition), 0);
    dbind_method_call_reentrant (_atspi_bus(), atspi_bus_registry, atspi_path_dec, atspi_interface_dec, "DeregisterKeystrokeListener", &d_error, "oa(iisi)uu", path, &key_set, d_modmask, d_event_types);
  g_free (path);
  return TRUE;
}

/**
 * atspi_register_device_event_listener:
 * @listener:  a pointer to the #AtspiDeviceListener which requests
 *             the events.
 * @event_types: an #AtspiDeviceEventMask mask indicating which
 *             types of key events are requested (#ATSPI_KEY_PRESSED, etc.).
 * @filter: Unused parameter.
 *             
 * Register a listener for device events, for instance button events.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
gboolean
atspi_register_device_event_listener (AtspiDeviceListener  *listener,
				 AtspiDeviceEventMask  event_types,
				 void                      *filter, GError **error)
{
  gboolean                          retval = FALSE;
  dbus_uint32_t d_event_types = event_types;
  gint                                i;
  gchar *path = _atspi_device_listener_get_path (listener);
  DBusError d_error;

  dbus_error_init (&d_error);
  if (!listener)
    {
      return retval;
    }

    dbind_method_call_reentrant (_atspi_bus(), atspi_bus_registry, atspi_path_dec, atspi_interface_dec, "RegisterDeviceEventListener", &d_error, "ou=>b", path, d_event_types, &retval);
  g_free (path);
  return retval;
}

/**
 * atspi_deregister_device_event_listener:
 * @listener: a pointer to the #AtspiDeviceListener for which
 *            device events are requested.
 * @filter: Unused parameter.
 *
 * Removes a device event listener from the registry's listener queue,
 *            ceasing notification of events of the specified type.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
gboolean
atspi_deregister_device_event_listener (AtspiDeviceListener *listener,
				   void                     *filter, GError **error)
{
  dbus_uint32_t event_types = 0;
  gchar *path = _atspi_device_listener_get_path (listener);
  DBusError d_error;

  dbus_error_init (&d_error);

  if (!listener)
    {
      return FALSE;
    }

  event_types |= (1 << ATSPI_BUTTON_PRESSED_EVENT);
  event_types |= (1 << ATSPI_BUTTON_RELEASED_EVENT);

    dbind_method_call_reentrant (_atspi_bus(), atspi_bus_registry, atspi_path_dec, atspi_interface_dec, "DeregisterDeviceEventListener", &d_error, "ou", path, event_types);
  g_free (path);
  return TRUE;
}

/**
 * atspi_generate_keyboard_event:
 * @keyval: a long integer indicating the keycode or keysym of the key event
 *           being synthesized.
 * @keystring: an (optional) UTF-8 string which, if @keyval is NULL,
 *           indicates a 'composed' keyboard input string which is 
 *           being synthesized; this type of keyboard event synthesis does
 *           not emulate hardware keypresses but injects the string 
 *           as though a composing input method (such as XIM) were used.
 * @synth_type: a #AtspiKeySynthType flag indicating whether @keyval
 *           is to be interpreted as a keysym rather than a keycode
 *           (ATSPI_KEYSYM), or whether to synthesize
 *           ATSPI_KEY_PRESS, ATSPI_KEY_RELEASE, or both (ATSPI_KEY_PRESSRELEASE).
 *
 * Synthesize a keyboard event (as if a hardware keyboard event occurred in the
 * current UI context).
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
gboolean
atspi_generate_keyboard_event (glong keyval,
			   const gchar *keystring,
			   AtspiKeySynthType synth_type, GError **error)
{
  dbus_uint32_t d_synth_type = synth_type;
  dbus_int32_t d_keyval = keyval;
  DBusError d_error;

  dbus_error_init (&d_error);
  if (!keystring) keystring = "";
    dbind_method_call_reentrant (_atspi_bus(), atspi_bus_registry, atspi_path_dec, atspi_interface_dec, "GenerateKeyboardEvent", &d_error, "isu", d_keyval, keystring, d_synth_type);

  return TRUE;
}

/**
 * atspi_generate_mouse_event:
 * @x: a #long indicating the screen x coordinate of the mouse event.
 * @y: a #long indicating the screen y coordinate of the mouse event.
 * @name: a string indicating which mouse event to be synthesized
 *        (e.g. "b1p", "b1c", "b2r", "rel", "abs").
 *
 * Synthesize a mouse event at a specific screen coordinate.
 * Most AT clients should use the #AccessibleAction interface when
 * tempted to generate mouse events, rather than this method.
 * Event names: b1p = button 1 press; b2r = button 2 release;
 *              b3c = button 3 click; b2d = button 2 double-click;
 *              abs = absolute motion; rel = relative motion.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
gboolean
atspi_generate_mouse_event (glong x, glong y, const gchar *name, GError **error)
{
  dbus_int32_t dbus_x = x, dbus__y = y;
  DBusError d_error;

  dbus_error_init (&d_error);
    dbind_method_call_reentrant (_atspi_bus(), atspi_bus_registry, atspi_path_dec, atspi_interface_dec, "GenerateMouseEvent", &d_error, "iis", x, y, name);
  return TRUE;
}

AtspiKeyDefinition *
atspi_key_definition_copy (AtspiKeyDefinition *src)
{
  AtspiKeyDefinition *dst;

  dst = g_new0 (AtspiKeyDefinition, 1);
  if (!dst)
    return NULL;
  dst->keycode = src->keycode;
  dst->keysym = src->keysym;
  if (src->keystring)
    dst->keystring = g_strdup (src->keystring);
  dst->unused = src->unused;
  return dst;
}

void
atspi_key_definition_free (AtspiKeyDefinition *kd)
{
  if (kd->keystring)
    g_free (kd->keystring);
  g_free (kd);
}

G_DEFINE_BOXED_TYPE (AtspiKeyDefinition, atspi_key_definition, atspi_key_definition_copy, atspi_key_definition_free)
