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

/* spi_registry.c: Global functions wrapping the registry */

#include <cspi/spi-private.h>

static GArray *desktops;

/**
 * SPI_getDesktopCount:
 *
 * Get the number of virtual desktops.
 * NOTE: currently multiple virtual desktops are not implemented, this
 *       function always returns '1'.
 *
 * Returns: an integer indicating the number of active virtual desktops.
 **/
int
SPI_getDesktopCount ()
{
  return 1;
}

/**
 * SPI_getDesktop:
 * @i: an integer indicating which of the accessible desktops is to be returned.
 *
 * Get the virtual desktop indicated by index @i.
 * NOTE: currently multiple virtual desktops are not implemented, this
 *       function always returns '1'.
 *
 * Returns: a pointer to the 'i-th' virtual desktop's #Accessible representation.
 **/
Accessible*
SPI_getDesktop (int i)
{
  if (i != 0) return NULL;
  return cspi_ref_accessible (spi_bus_registry, NULL);
}

/**
 * SPI_getDesktopList:
 * @desktop_list: a pointer to an array of #Accessible references.
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
 * Returns: an integer indicating how many virtual desktops have been
 *          placed in the list pointed to by parameter @list.
 **/
int
SPI_getDesktopList (Accessible ***desktop_list)
{
  Accessible **list;

  list = g_new0 (Accessible *, 2);

  if (!desktop_list) return 1;
  list [0] = cspi_ref_accessible (spi_bus_registry, NULL);

  *desktop_list = list;

  return 1;
}

/**
 * SPI_freeDesktopList:
 * @desktop_list: a pointer to an array of #Accessible objects
 * as returned from @SPI_getDesktopList
 * 
 * This routine frees the memory associated with the list.
 **/
void
SPI_freeDesktopList (Accessible **desktop_list)
{
  Accessible **p;
  
  for (p = desktop_list; p && *p; p++)
    {
      cspi_object_unref (*p);
    }
  g_free (desktop_list);
}

/**
 * SPI_KEYSET_ALL_KEYS:
 * @SPI_KEYSET_ALL_KEYS: A special value for an AccessibleKeySet type, which tacitly
 *                       includes all keycodes and keyvals for the specified modifier set.
 **/

/**
 * SPI_registerAccessibleKeystrokeListener:
 * @listener:  a pointer to the #AccessibleKeystrokeListener for which
 *             keystroke events are requested.
 * @keys:      a pointer to the #AccessibleKeySet indicating which
 *             keystroke events are requested, or #SPI_KEYSET_ALL_KEYS
 *             to indicate that all keycodes and keyvals for the specified
 *             modifier set are to be included.
 * @modmask:   an #AccessibleKeyMaskType mask indicating which
 *             key event modifiers must be set in combination with @keys,
 *             events will only be reported for key events for which all
 *             modifiers in @modmask are set.  If you wish to listen for
 *             events with multiple modifier combinations you must call
 *             registerAccessibleKeystrokeListener() once for each combination.
 * @eventmask: an #AccessibleKeyMaskType mask indicating which
 *             types of key events are requested (#SPI_KEY_PRESSED, etc.).
 * @sync_type: a #AccessibleKeyListenerSyncType parameter indicating
 *             the behavior of the notification/listener transaction.
 *             
 * Register a listener for keystroke events, either pre-emptively for
 *             all windows (SPI_KEYLISTENER_ALL_WINDOWS),
 *             non-preemptively (SPI_KEYLISTENER_NOSYNC), or
 *             pre-emptively at the toolkit level (SPI_KEYLISTENER_CANCONSUME).
 *             If ALL_WINDOWS or CANCONSUME are used, the event is consumed
 *             upon receipt if one of @listener's callbacks returns #TRUE.
 *             ( Other sync_type values may be available in the future )
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
SPIBoolean
SPI_registerAccessibleKeystrokeListener (AccessibleKeystrokeListener  *listener,
					 AccessibleKeySet             *keys,
					 AccessibleKeyMaskType         modmask,
					 AccessibleKeyEventMask        eventmask,
					 AccessibleKeyListenerSyncType sync_type)
{
  gchar *path = cspi_device_listener_get_path (listener);
  gint                                i;
  GArray *key_set;
  dbus_uint32_t key_events = 0;
  Accessibility_ControllerEventMask   controller_event_mask;
  Accessibility_EventListenerMode     listener_mode;
  DBusError error;
  SPIBoolean                          retval = FALSE;

  if (!listener)
    {
      return retval;
    }

  /* copy the keyval filter values from the C api into the DBind KeySet */
  if (keys)
    {
      key_set = g_array_sized_new (FALSE, TRUE, sizeof (Accessibility_KeyDefinition), keys->len);
      key_set->len = keys->len;
      for (i = 0; i < keys->len; ++i)
        {
	  Accessibility_KeyDefinition *kd =  ((Accessibility_KeyDefinition *) key_set->data) + i;
          kd->keycode = keys->keycodes[i];
	  kd->keysym = keys->keysyms[i];
	  if (keys->keystrings && keys->keystrings[i])
	    {
	      kd->keystring = keys->keystrings[i];
	    } 
          else 
            {
	      kd->keystring = "";
	    }
        }
    }
  else
    {
      key_set = g_array_sized_new (FALSE, TRUE, sizeof (Accessibility_KeyDefinition), 0);
    }
	
  /* copy the event filter values from the C api into the DBus key_events */
  if (eventmask & SPI_KEY_PRESSED)
    {
      key_events |= (1 << Accessibility_KEY_PRESSED_EVENT);
    }
  if (eventmask & SPI_KEY_RELEASED)
    {
      key_events |= (1 << Accessibility_KEY_RELEASED_EVENT);
    }
  
  controller_event_mask = (dbus_uint32_t) modmask;

  listener_mode.synchronous =
	  (dbus_bool_t) ((sync_type & SPI_KEYLISTENER_SYNCHRONOUS)!=0);
  listener_mode.preemptive =
	  (dbus_bool_t) ((sync_type & SPI_KEYLISTENER_CANCONSUME)!=0);
  listener_mode.global =
	  (dbus_bool_t) ((sync_type & SPI_KEYLISTENER_ALL_WINDOWS)!=0);

    dbus_error_init (&error);
    dbind_method_call_reentrant (SPI_bus(), spi_bus_registry, spi_path_dec, spi_interface_dec, "registerKeystrokeListener", &error, "oa(iisi)uu(bbb)=>b", path, key_set, controller_event_mask, key_events, &listener_mode, &retval);

  g_array_free (key_set, TRUE);
  g_free (path);

  return retval;
}

/**
 * SPI_deregisterAccessibleKeystrokeListener:
 * @listener: a pointer to the #AccessibleKeystrokeListener for which
 *            keystroke events are requested.
 * @modmask:  the key modifier mask for which this listener is to be
 *            'deregistered' (of type #AccessibleeyMaskType).
 *
 * Removes a keystroke event listener from the registry's listener queue,
 *            ceasing notification of events with modifiers matching @modmask.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
SPIBoolean
SPI_deregisterAccessibleKeystrokeListener (AccessibleKeystrokeListener *listener,
					   AccessibleKeyMaskType        modmask)
{
  gchar *path = cspi_device_listener_get_path (listener);
  Accessibility_ControllerEventMask   controller_event_mask;
  GArray *key_set;
  dbus_uint32_t key_events = 0;
  DBusError error;

  if (!listener)
    {
      return FALSE;
    }


  controller_event_mask = (dbus_uint32_t) modmask;

      key_set = g_array_sized_new (FALSE, TRUE, sizeof (Accessibility_KeyDefinition), 0);
    dbind_method_call_reentrant (SPI_bus(), spi_bus_registry, spi_path_dec, spi_interface_dec, "deregisterKeystrokeListener", &error, "oa(iisi)uu", path, &key_set, key_events, controller_event_mask);
  g_free (path);
  return TRUE;
}

/**
 * SPI_registerDeviceEventListener:
 * @listener:  a pointer to the #AccessibleDeviceListener which requests
 *             the events.
 * @eventmask: an #AccessibleDeviceEventMask mask indicating which
 *             types of key events are requested (#SPI_KEY_PRESSED, etc.).
 * @filter: Unused parameter.
 *             
 * Register a listener for device events, for instance button events.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
SPIBoolean
SPI_registerDeviceEventListener (AccessibleDeviceListener  *listener,
				 AccessibleDeviceEventMask  eventmask,
				 void                      *filter)
{
  SPIBoolean                          retval = FALSE;
  dbus_uint32_t event_types = 0;
  gint                                i;
  gchar *path = cspi_device_listener_get_path (listener);
  DBusError error;

  if (!listener)
    {
      return retval;
    }

  /* copy the event filter values from the C api into the CORBA KeyEventTypeSeq */
  
  if (eventmask & SPI_BUTTON_PRESSED)
    {
      event_types |= (1 << Accessibility_BUTTON_PRESSED_EVENT);
    }
  if (eventmask & SPI_BUTTON_RELEASED)
    {
      event_types |= (1 << Accessibility_BUTTON_RELEASED_EVENT);
    }

  dbus_error_init (&error);
    dbind_method_call_reentrant (SPI_bus(), spi_bus_registry, spi_path_dec, spi_interface_dec, "registerDeviceEventListener", &error, "ou=>b", path, event_types, &retval);
  g_free (path);
  return retval;
}

/**
 * SPI_deregisterDeviceEventListener:
 * @listener: a pointer to the #AccessibleDeviceListener for which
 *            device events are requested.
 * @filter: Unused parameter.
 *
 * Removes a device event listener from the registry's listener queue,
 *            ceasing notification of events of the specified type.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
SPIBoolean
SPI_deregisterDeviceEventListener (AccessibleDeviceListener *listener,
				   void                     *filter)
{
  dbus_uint32_t event_types = 0;
  gchar *path = cspi_device_listener_get_path (listener);
  DBusError error;

  if (!listener)
    {
      return FALSE;
    }

  event_types |= (1 << Accessibility_BUTTON_PRESSED_EVENT);
  event_types |= (1 << Accessibility_BUTTON_RELEASED_EVENT);

  dbus_error_init (&error);
    dbind_method_call_reentrant (SPI_bus(), spi_bus_registry, spi_path_dec, spi_interface_dec, "deregisterDeviceEventListener", &error, "ou", path, event_types);
  g_free (path);
  return TRUE;
}

/**
 * SPI_generateKeyboardEvent:
 * @keyval: a long integer indicating the keycode or keysym of the key event
 *           being synthesized.
 * @keystring: an (optional) UTF-8 string which, if @keyval is NULL,
 *           indicates a 'composed' keyboard input string which is 
 *           being synthesized; this type of keyboard event synthesis does
 *           not emulate hardware keypresses but injects the string 
 *           as though a composing input method (such as XIM) were used.
 * @synth_type: a #AccessibleKeySynthType flag indicating whether @keyval
 *           is to be interpreted as a keysym rather than a keycode
 *           (CSPI_KEYSYM), or whether to synthesize
 *           SPI_KEY_PRESS, SPI_KEY_RELEASE, or both (SPI_KEY_PRESSRELEASE).
 *
 * Synthesize a keyboard event (as if a hardware keyboard event occurred in the
 * current UI context).
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
SPIBoolean
SPI_generateKeyboardEvent (long int keyval,
			   char *keystring,
			   AccessibleKeySynthType synth_type)
{
  dbus_uint32_t keysynth_type;
  dbus_int32_t keycode = keyval;
  DBusError error;

  switch (synth_type)
    {
      case SPI_KEY_PRESS:
	  keysynth_type = Accessibility_KEY_PRESS;
	  break;
      case SPI_KEY_RELEASE:
	  keysynth_type = Accessibility_KEY_RELEASE;
	  break;
      case SPI_KEY_PRESSRELEASE:
	  keysynth_type = Accessibility_KEY_PRESSRELEASE;
	  break;
      case SPI_KEY_SYM:
	  keysynth_type = Accessibility_KEY_SYM;
	  break;
      case SPI_KEY_STRING:
	  keysynth_type = Accessibility_KEY_STRING;
	  break;
      default:
          return FALSE;
    }

  if (!keystring) keystring = "";
  dbus_error_init (&error);
    dbind_method_call_reentrant (SPI_bus(), spi_bus_registry, spi_path_dec, spi_interface_dec, "generateKeyboardEvent", &error, "isu", keycode, keystring, keysynth_type);

  return TRUE;
}

/**
 * SPI_generateMouseEvent:
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
SPIBoolean
SPI_generateMouseEvent (long x, long y, char *name)
{
  dbus_int32_t dbus_x = x, dbus__y = y;
  DBusError error;

  dbus_error_init (&error);
    dbind_method_call_reentrant (SPI_bus(), spi_bus_registry, spi_path_dec, spi_interface_dec, "generateMouseEvent", &error, "iis", x, y, name);
  return TRUE;
}

char *
cspi_device_listener_get_path (CSpiDeviceListener *listener)
{
  return g_strdup_printf ("/org/freedesktop/atspi/listeners/%d", listener->id);
}
