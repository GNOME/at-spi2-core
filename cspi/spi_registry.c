/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001 Sun Microsystems Inc.
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

/**
 * SPI_registerGlobalEventListener:
 * @listener: the #AccessibleEventListener to be registered against an
 *            event type.
 * @eventType: a character string indicating the type of events for which
 *            notification is requested.  Format is
 *            EventClass:major_type:minor_type:detail
 *            where all subfields other than EventClass are optional.
 *            EventClasses include "object", "window", "mouse",
 *            and toolkit events (e.g. "Gtk", "AWT").
 *            Examples: "focus:", "Gtk:GtkWidget:button_press_event".
 *
 * Legal object event types:
 *
 *    (property change events)
 *
 *            object:property-change
 *            object:property-change:accessible-name
 *            object:property-change:accessible-description
 *            object:property-change:accessible-parent
 *            object:property-change:accessible-value
 *            object:property-change:accessible-role
 *            object:property-change:accessible-table-caption
 *            object:property-change:accessible-table-column-description
 *            object:property-change:accessible-table-column-header
 *            object:property-change:accessible-table-row-description
 *            object:property-change:accessible-table-row-header
 *            object:property-change:accessible-table-summary
 *
 *    (other object events)
 *
 *            object:state-changed 
 *            object:children-changed
 *            object:visible-data-changed
 *            object:selection-changed
 *            object:text-selection-changed
 *            object:text-changed
 *            object:text-caret-moved
 *            object:row-inserted
 *            object:row-reordered
 *            object:row-deleted
 *            object:column-inserted
 *            object:column-reordered
 *            object:column-deleted
 *            object:model-changed
 *
 *  (window events)
 *
 *            window:minimize
 *            window:maximize
 *            window:restore
 *            window:close
 *            window:create
 *            window:reparent
 *            window:desktop-create
 *            window:desktop-destroy
 *            window:activate
 *            window:deactivate
 *            window:raise
 *            window:lower
 *            window:move
 *            window:resize
 *            window:shade
 *            window:unshade
 *            window:restyle
 *
 * NOTE: this string may be UTF-8, but should not contain byte value 56
 *            (ascii ':'), except as a delimiter, since non-UTF-8 string
 *            delimiting functions are used internally.
 *            In general, listening to
 *            toolkit-specific events is not recommended.
 *
 * Add an in-process callback function to an existing AccessibleEventListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
SPIBoolean
SPI_registerGlobalEventListener (AccessibleEventListener *listener,
				 const char              *eventType)
{
  if (!listener)
    {
      return FALSE;
    }

  Accessibility_Registry_registerGlobalEventListener (
    cspi_registry (),
    cspi_event_listener_get_corba (listener),
    eventType, cspi_ev ());

  return  !cspi_exception ();
}

/**
 * SPI_deregisterGlobalEventListenerAll:
 * @listener: the #AccessibleEventListener to be registered against
 *            an event type.
 *
 * deregisters an AccessibleEventListener from the registry, for all
 *            event types it may be listening to. Use
 *            AccessibleEventListener_unref to release the
 *            listener reference.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
SPIBoolean
SPI_deregisterGlobalEventListenerAll (AccessibleEventListener *listener)
{
  if (!listener)
    {
      return FALSE;
    }

  Accessibility_Registry_deregisterGlobalEventListenerAll (
    cspi_registry (),
    cspi_event_listener_get_corba (listener),
    cspi_ev ());

  return !cspi_exception ();
}

/**
 * SPI_deregisterGlobalEventListener:
 * @listener: the #AccessibleEventListener registered against an event type.
 * @eventType: a string specifying the event type for which this
 *             listener is to be deregistered.
 *
 * deregisters an AccessibleEventListener from the registry, for a specific
 *             event type.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
SPIBoolean
SPI_deregisterGlobalEventListener (AccessibleEventListener *listener,
				   const char              *eventType)
{
  if (!listener)
    {
      return FALSE;
    }

  Accessibility_Registry_deregisterGlobalEventListener (
    cspi_registry (), 
    cspi_event_listener_get_corba (listener),
    eventType, cspi_ev ());

  return !cspi_exception ();
}

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
  int retval;

  retval = Accessibility_Registry_getDesktopCount (
    cspi_registry (), cspi_ev ());

  cspi_return_val_if_ev ("getDesktopCount", -1);

  return retval;
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
  return cspi_object_add (
    Accessibility_Registry_getDesktop (
      cspi_registry (), i, cspi_ev ()));
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
  int i;
  Accessible **list;
  Accessibility_DesktopSeq *desktops;

  if (!desktop_list)
	  return 0;

  *desktop_list = NULL;

  desktops = Accessibility_Registry_getDesktopList (cspi_registry (),
						    cspi_ev ());

  cspi_return_val_if_ev ("getDesktopList", 0);

  list = g_new0 (Accessible *, desktops->_length + 1);

  for (i = 0; i < desktops->_length; i++)
    {
      list [i] = cspi_object_add (
	      CORBA_Object_duplicate (desktops->_buffer [i], cspi_ev ()));
    }
  list [i] = NULL;

  CORBA_free (desktops);

  *desktop_list = list;

  return i;
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
 *             keystroke events are requested, or #CSPI_KEYSET_ALL_KEYS.
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
 *             all windows (CSPI_KEYLISTENER_ALL_WINDOWS), or
 *             non-preemptively (CSPI_KEYLISTENER_NOSYNC).
 *             ( Other sync_type values may be available in the future.)
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
  gint                                i, mask;
  Accessibility_KeySet                key_set;
  Accessibility_KeyEventTypeSeq       key_events;
  Accessibility_ControllerEventMask   controller_event_mask;
  Accessibility_DeviceEventController device_event_controller;
  Accessibility_EventListenerMode     listener_mode;

  if (!listener)
    {
      return FALSE;
    }

  device_event_controller = 
    Accessibility_Registry_getDeviceEventController (cspi_registry (), cspi_ev ());

  cspi_return_val_if_ev ("getting event controller", FALSE);

  /* copy the keyval filter values from the C api into the CORBA KeySet */
  if (keys)
    {
      key_set._length = keys->len;
      key_set._buffer = Accessibility_KeySet_allocbuf (keys->len);
      for (i = 0; i < key_set._length; ++i)
        {
          key_set._buffer[i].keycode = keys->keycodes[i];
	  key_set._buffer[i].keysym = keys->keysyms[i];
	  if (keys->keystrings && keys->keystrings[i]) 
	    {
	      key_set._buffer[i].keystring = keys->keystrings[i];
	    } 
          else 
            {
	      key_set._buffer[i].keystring = CORBA_string_dup("");
	    }
        }
    }
  else
    {
      key_set._length = 0;
      key_set._buffer = NULL;
    }
	
  /* copy the event filter values from the C api into the CORBA KeyEventTypeSeq */
  mask = 1;
  i = 0;
  do
    {
      if (mask & eventmask)
        {
          ++i; 
	}
      mask <<= 1;
    }
  while (mask & 0xFFFF);
  
  key_events._buffer = Accessibility_KeyEventTypeSeq_allocbuf (i);
  i = 0;
  if (eventmask & SPI_KEY_PRESSED)
    {
      key_events._buffer[i++] = Accessibility_KEY_PRESSED;
    }
  if (eventmask & SPI_KEY_RELEASED)
    {
      key_events._buffer[i++] = Accessibility_KEY_RELEASED;
    }
  key_events._length = i;
  
  controller_event_mask = (CORBA_unsigned_long) modmask;

  listener_mode.synchronous =
	  (CORBA_boolean) ((sync_type & SPI_KEYLISTENER_SYNCHRONOUS)!=0);
  listener_mode.preemptive =
	  (CORBA_boolean) ((sync_type & SPI_KEYLISTENER_CANCONSUME)!=0);
  listener_mode.global =
	  (CORBA_boolean) ((sync_type & SPI_KEYLISTENER_ALL_WINDOWS)!=0);

  Accessibility_DeviceEventController_registerKeystrokeListener (
    device_event_controller,
    cspi_event_listener_get_corba (listener),
    &key_set,
    controller_event_mask,
    &key_events,
    &listener_mode,
    cspi_ev ());

  cspi_return_val_if_ev ("registering keystroke listener", FALSE);

  cspi_release_unref (device_event_controller);

  return TRUE;
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
  Accessibility_ControllerEventMask   controller_event_mask;
  Accessibility_KeySet                key_set;
  Accessibility_KeyEventTypeSeq       key_events;
  Accessibility_DeviceEventController device_event_controller;

  if (!listener)
    {
      return FALSE;
    }

  device_event_controller = 
    Accessibility_Registry_getDeviceEventController (cspi_registry (), cspi_ev ());

  cspi_return_val_if_ev ("getting keystroke listener", FALSE);

  controller_event_mask = (CORBA_unsigned_long) modmask;

  key_events._buffer = NULL;
  key_events._length = 0;

  key_set._buffer = NULL;
  key_set._length = 0;

  Accessibility_DeviceEventController_deregisterKeystrokeListener (
    device_event_controller,
    cspi_event_listener_get_corba (listener),
    &key_set,
    controller_event_mask,
    &key_events,
    cspi_ev ());

  cspi_release_unref (device_event_controller);

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
/* TODO: check current modifier status and
 *  send keycode to alter, if necessary
 */
	
  /* TODO: implement keystring use case */
  Accessibility_KeySynthType keysynth_type;
  Accessibility_DeviceEventController device_event_controller = 
	  Accessibility_Registry_getDeviceEventController (cspi_registry (), cspi_ev ());

  cspi_return_val_if_ev ("getting event controller for key event gen", FALSE);

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

  Accessibility_DeviceEventController_generateKeyboardEvent (device_event_controller,
							     keyval,
							     "",
							     keysynth_type,
							     cspi_ev ());

  cspi_return_val_if_ev ("generating keyboard event", FALSE);

  cspi_release_unref (device_event_controller);

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
  Accessibility_DeviceEventController device_event_controller = 
	  Accessibility_Registry_getDeviceEventController (cspi_registry (), cspi_ev ());

  cspi_return_val_if_ev ("getting event controller for mouse event gen", FALSE);

  Accessibility_DeviceEventController_generateMouseEvent (device_event_controller,
							  x, y, name, cspi_ev ());
  cspi_return_val_if_ev ("generating mouse event", FALSE);

  cspi_release_unref (device_event_controller);

  return TRUE;
}

