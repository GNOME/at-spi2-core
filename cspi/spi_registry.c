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

#include <cspi/spi-private.h>

/*
 *
 * Global functions serviced by the registry
 *
 */

/* static stuff used only by registry C bindings */
static GList *key_listeners = NULL;
static Display *display = NULL;

/**
 * registerGlobalEventListener:
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
 *            object:property-change:accessible-state
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
 * NOTE: this string may be UTF-8, but should not contain byte value 56
 *            (ascii ':'), except as a delimiter, since non-UTF-8 string
 *            delimiting functions are used internally.
 *            In general, listening to
 *            toolkit-specific events is not recommended.
 *
 * Add an in-process callback function to an existing AccessibleEventListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
registerGlobalEventListener (AccessibleEventListener *listener,
                             char *eventType)
{
  boolean retval;

  Accessibility_Registry_registerGlobalEventListener (
                         spi_registry (),
                         (Accessibility_EventListener)
                            bonobo_object_corba_objref (bonobo_object (listener)),
                         eventType,
                         spi_ev ());

  retval = !spi_exception ();
 
  return retval;
}

/**
 * deregisterGlobalEventListenerAll:
 * @listener: the #AccessibleEventListener to be registered against
 *            an event type.
 *
 * deregisters an AccessibleEventListener from the registry, for all
 *            event types it may be listening to.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
deregisterGlobalEventListenerAll (AccessibleEventListener *listener)
{
  Accessibility_Registry_deregisterGlobalEventListenerAll (
                         spi_registry (),
                         (Accessibility_EventListener)
                            CORBA_Object_duplicate (
				    bonobo_object_corba_objref (
					    bonobo_object (listener)), spi_ev ()),
                         spi_ev ());

  return !spi_exception ();
}
/**
 * deregisterGlobalEventListener:
 * @listener: the #AccessibleEventListener registered against an event type.
 * @eventType: a string specifying the event type for which this
 *             listener is to be deregistered.
 *
 * deregisters an AccessibleEventListener from the registry, for a specific
 *             event type.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
deregisterGlobalEventListener (AccessibleEventListener *listener,
			       char *eventType)
{
  Accessibility_Registry_deregisterGlobalEventListener (
	  spi_registry (),
	  (Accessibility_EventListener)
	  CORBA_Object_duplicate (
		  bonobo_object_corba_objref (bonobo_object (listener)), spi_ev ()),
	  (CORBA_char *) eventType,
	  spi_ev ());

  return !spi_exception ();
}

/**
 * getDesktopCount:
 *
 * Get the number of virtual desktops.
 * NOTE: currently multiple virtual desktops are not implemented, this
 *       function always returns '1'.
 *
 * Returns: an integer indicating the number of active virtual desktops.
 *
 **/
int
getDesktopCount ()
{
  return Accessibility_Registry_getDesktopCount (
	  spi_registry (), spi_ev ());
}

/**
 * getDesktop:
 * @i: an integer indicating which of the accessible desktops is to be returned.
 *
 * Get the virtual desktop indicated by index @i.
 * NOTE: currently multiple virtual desktops are not implemented, this
 *       function always returns '1'.
 *
 * Returns: a pointer to the 'i-th' virtual desktop's #Accessible representation.
 *
 **/
Accessible*
getDesktop (int i)
{
  return spi_object_add (Accessibility_Registry_getDesktop (spi_registry (), (CORBA_short) i, spi_ev ()));
}

/**
 * getDesktopList:
 * @list: a pointer to an array of #Accessible objects.
 *
 * Get the list of virtual desktops.  On return, @list will point
 *     to a newly-created array of virtual desktop pointers.
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
getDesktopList (Accessible **list)
{
  *list = NULL;
  return 0;
}

static gboolean
key_event_source_func (void *p)
{
  GList *listeners = (GList *)p;
  XEvent *x_event = g_new0 (XEvent, 1);
  while (XPending (display))
    {
      XNextEvent (display, x_event);
      while (listeners)
        {
        /* if the listener mask matches, notify it*/
          if (1)
  	    {
	      ;	  
	    }
	}
    }
  return TRUE;
}

/**
 * registerAccessibleKeystrokeListener:
 * @listener:  a pointer to the #AccessibleKeystrokeListener for which
 *             keystroke events are requested.
 * @keys:      a pointer to the #AccessibleKeySet indicating which
 *             keystroke events are requested, or #SPI_KEYSET_ALL_KEYS.
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
 *             all windows (SPI_KEYLISTENER_ALL_WINDOWS), or
 *             non-preemptively (SPI_KEYLISTENER_NOSYNC).
 *             ( Other sync_type values may be available in the future.)
 **/
void
registerAccessibleKeystrokeListener (AccessibleKeystrokeListener *listener,
				     AccessibleKeySet *keys,
				     AccessibleKeyMaskType modmask,
				     AccessibleKeyEventMask eventmask,
				     AccessibleKeyListenerSyncType sync_type)
{
  Accessibility_ControllerEventMask *controller_event_mask =
	  Accessibility_ControllerEventMask__alloc();
  Accessibility_DeviceEventController device_event_controller = 
	  Accessibility_Registry_getDeviceEventController (spi_registry (), spi_ev ());
  Accessibility_KeySet *key_set = Accessibility_KeySet__alloc();
  Accessibility_KeyEventTypeSeq *key_events = Accessibility_KeyEventTypeSeq__alloc();
  Accessibility_KeystrokeListener spi_listener_corba_ref;
  gint i, mask;
  Accessibility_DeviceEventController_ref (device_event_controller, spi_ev ());

  /* copy the keyval filter values from the C api into the CORBA KeySet */
  if (keys)
    {
      key_set->_buffer = Accessibility_KeySet_allocbuf (
 	                                            (unsigned long) keys->len);
      key_set->_length = (unsigned long) keys->len;
      for (i=0; i < key_set->_length; ++i)
        {
          /* we overload the keyset long w/keycodes, the - bit acts as a flag */
          key_set->_buffer[i] = (keys->keysyms[i]) ? keys->keysyms[i] :
	                                         -keys->keycodes[i];
	  /* g_print ("key-set %d = %d\n", i, (int) key_set->_buffer[i]); */
        }
    }
  /* copy the event filter values from the C api into the CORBA KeyEventTypeSeq */
  mask=1;
  i=0;
  do
    {
      if (mask & eventmask) ++i; 
      mask <<= 1;
    } while (mask & 0xFFFF);
  
  key_events->_buffer = Accessibility_KeyEventTypeSeq_allocbuf (i);
  i=0;
  if (eventmask & SPI_KEY_PRESSED)
    {
      key_events->_buffer[i++] = Accessibility_KEY_PRESSED;
    }
  if (eventmask & SPI_KEY_RELEASED)
    {
      key_events->_buffer[i++] = Accessibility_KEY_RELEASED;
    }
  key_events->_length = i;
  
  controller_event_mask->value = (CORBA_unsigned_long) modmask;
  controller_event_mask->refcount = (CORBA_unsigned_short) 1;

  spi_listener_corba_ref = (Accessibility_KeystrokeListener)
	  CORBA_Object_duplicate (bonobo_object_corba_objref (bonobo_object (listener)), spi_ev ());
  
	  Accessibility_DeviceEventController_registerKeystrokeListener (
	  device_event_controller,
	  spi_listener_corba_ref,
	  key_set,
	  controller_event_mask,
	  key_events,
	  (CORBA_boolean) ((sync_type & SPI_KEYLISTENER_ALL_WINDOWS)!=0),
	  spi_ev ());
}

/**
 * deregisterAccessibleKeystrokeListener:
 * @listener: a pointer to the #AccessibleKeystrokeListener for which
 *            keystroke events are requested.
 * @modmask:  the key modifier mask for which this listener is to be
 *            'deregistered' (of type #AccessibleeyMaskType).
 *
 * Removes a keystroke event listener from the registry's listener queue,
 *            ceasing notification of events with modifiers matching @modmask.
 **/
void
deregisterAccessibleKeystrokeListener (AccessibleKeystrokeListener *listener,
				       AccessibleKeyMaskType modmask)
{
  Accessibility_ControllerEventMask *controller_event_mask =
	  Accessibility_ControllerEventMask__alloc();
  Accessibility_DeviceEventController device_event_controller = 
	  Accessibility_Registry_getDeviceEventController (spi_registry (), spi_ev ());
  Accessibility_KeySet *all_keys = Accessibility_KeySet__alloc();
  Accessibility_KeyEventTypeSeq *key_events = Accessibility_KeyEventTypeSeq__alloc();
  Accessibility_KeystrokeListener spi_listener_corba_ref;
  Accessibility_DeviceEventController_unref (device_event_controller, spi_ev ());
  controller_event_mask->value = (CORBA_unsigned_long) modmask;
  controller_event_mask->refcount = (CORBA_unsigned_short) 1;

  spi_listener_corba_ref = (Accessibility_KeystrokeListener)
	  CORBA_Object_duplicate (bonobo_object_corba_objref (bonobo_object (listener)), spi_ev ());
  
  Accessibility_DeviceEventController_deregisterKeystrokeListener (
	  device_event_controller,
	  spi_listener_corba_ref,
	  all_keys,
	  controller_event_mask,
	  key_events,
	  (CORBA_boolean) TRUE,
	  spi_ev ());
}

/**
 * generateKeyEvent:
 * @keyval: a long integer indicating the keycode or keysym of the key event
 *           being synthesized.
 * @synth_type: a #AccessibleKeySynthType flag indicating whether @keyval
 *           is to be interpreted as a keysym rather than a keycode
 *           (SPI_KEYSYM), or whether to synthesize
 *           SPI_KEY_PRESS, SPI_KEY_RELEASE, or both (SPI_KEY_PRESSRELEASE).
 *
 * Synthesize a keyboard event (as if a hardware keyboard event occurred in the
 * current UI context).
 *
 **/
void
generateKeyEvent (long int keyval, AccessibleKeySynthType synth_type)
{
/* TODO: check current modifier status and
 *  send keycode to alter, if necessary
 */
  Accessibility_DeviceEventController device_event_controller = 
	  Accessibility_Registry_getDeviceEventController (spi_registry (), spi_ev ());
  Accessibility_DeviceEventController_generateKeyEvent (device_event_controller,
							keyval,
							(unsigned long) synth_type,
							spi_ev ());
}

/**
 * generateMouseEvent:
 * @x: a #long indicating the screen x coordinate of the mouse event.
 * @y: a #long indicating the screen y coordinate of the mouse event.
 * @name: a string indicating which mouse event to be synthesized
 *        (e.g. "button1", "button2", "mousemove").
 *
 * Synthesize a mouse event at a specific screen coordinate.
 * Most AT clients should use the #AccessibleAction interface when
 * tempted to generate mouse events, rather than this method.
 * Not Yet Implemented.
 *
 **/
void
generateMouseEvent (long x, long y, char *name)
{
  ;
}

