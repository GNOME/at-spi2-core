
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
 * @listener: the #AccessibleEventListener to be registered against an event type.
 * @callback: a character string indicating the type of events for which
 *            notification is requested.  Format is
 *            EventClass:major_type:minor_type:detail
 *            where all subfields other than EventClass are optional.
 *            EventClasses include "Focus", "Window", "Mouse",
 *            and toolkit events (e.g. "Gtk", "AWT").
 *            Examples: "focus:", "Gtk:GtkWidget:button_press_event".
 *
 * NOTE: this string may be UTF-8, but should not contain byte value 56 (ascii ':'),
 *            except as a delimiter, since non-UTF-8 string delimiting
 *            functions are used internally.  In general, listening to
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
  Accessibility_Registry_registerGlobalEventListener (
                         registry,
                         (Accessibility_EventListener)
                            bonobo_object_corba_objref (bonobo_object (listener)),
                         eventType,
                         &ev);

  if (ev._major != CORBA_NO_EXCEPTION)
    {
    return FALSE;
    }
  else
    {
      return TRUE;
    }
}

/**
 * deregisterGlobalEventListener:
 * @listener: the #AccessibleEventListener to be registered against an event type.
 *
 * deregisters an AccessibleEventListener from the registry, for all event types it may be listening to.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
deregisterGlobalEventListenerAll (AccessibleEventListener *listener)
{
  Accessibility_Registry_deregisterGlobalEventListenerAll (
                         registry,
                         (Accessibility_EventListener)
                            CORBA_Object_duplicate (bonobo_object_corba_objref (bonobo_object (listener)), &ev),
                         &ev);

  if (ev._major != CORBA_NO_EXCEPTION)
    {
    return FALSE;
    }
  else
    {
      return TRUE;
    }
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
  return Accessibility_Registry_getDesktopCount (registry, &ev);
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
getDesktop (int n)
{
  return Obj_Add (Accessibility_Registry_getDesktop (registry, (CORBA_short) n, &ev));
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
 * Not Yet Implemented.
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
 * @listener: a pointer to the #AccessibleKeystrokeListener for which
 *            keystroke events are requested.
 *
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
	  Accessibility_Registry_getDeviceEventController (registry, &ev);
  Accessibility_KeySet *all_keys = Accessibility_KeySet__alloc();
  Accessibility_KeyEventTypeSeq *key_events = Accessibility_KeyEventTypeSeq__alloc();
  Accessibility_KeystrokeListener spi_listener_corba_ref;
  Accessibility_DeviceEventController_ref (device_event_controller, &ev);
  controller_event_mask->value = (CORBA_unsigned_long) modmask;
  controller_event_mask->refcount = (CORBA_unsigned_short) 1;

  spi_listener_corba_ref = (Accessibility_KeystrokeListener)
	  CORBA_Object_duplicate (bonobo_object_corba_objref (bonobo_object (listener)), &ev);
  
	  Accessibility_DeviceEventController_registerKeystrokeListener (
	  device_event_controller,
	  spi_listener_corba_ref,
	  all_keys,
	  controller_event_mask,
	  key_events,
	  (CORBA_boolean) ((sync_type | SPI_KEYLISTENER_ALL_WINDOWS)!=0),
	  &ev);
}

/**
 * deregisterAccessibleKeystrokeListener:
 * @listener: a pointer to the #AccessibleKeystrokeListener for which
 *            keystroke events are requested.
 *
 **/
void
deregisterAccessibleKeystrokeListener (AccessibleKeystrokeListener *listener, AccessibleKeyMaskType keymask)
{
  Accessibility_ControllerEventMask *controller_event_mask =
	  Accessibility_ControllerEventMask__alloc();
  Accessibility_DeviceEventController device_event_controller = 
	  Accessibility_Registry_getDeviceEventController (registry, &ev);
  Accessibility_KeySet *all_keys = Accessibility_KeySet__alloc();
  Accessibility_KeyEventTypeSeq *key_events = Accessibility_KeyEventTypeSeq__alloc();
  Accessibility_KeystrokeListener spi_listener_corba_ref;
  Accessibility_DeviceEventController_unref (device_event_controller, &ev);
  controller_event_mask->value = (CORBA_unsigned_long) keymask;
  controller_event_mask->refcount = (CORBA_unsigned_short) 1;

  spi_listener_corba_ref = (Accessibility_KeystrokeListener)
	  CORBA_Object_duplicate (bonobo_object_corba_objref (bonobo_object (listener)), &ev);
  
  Accessibility_DeviceEventController_deregisterKeystrokeListener (
	  device_event_controller,
	  spi_listener_corba_ref,
	  all_keys,
	  controller_event_mask,
	  key_events,
	  (CORBA_boolean) TRUE,
	  &ev);
}

/**
 * generateKeyEvent:
 * @keycode: a #long indicating the keycode of the key event
 *           being synthesized.
 * @meta: a #long indicating the key modifiers to be sent
 *        with the event, if any.
 *
 * Synthesize a keyboard event (as if a hardware keyboard event occurred in the
 * current UI context).
 * Not Yet Implemented.
 *
 **/
void
generateKeyEvent (long keyval, AccessibleKeySynthType type)
{
/* TODO: check current modifier status and
 *  send keycode to alter, if necessary
 */
  Accessibility_DeviceEventController device_event_controller = 
	  Accessibility_Registry_getDeviceEventController (registry, &ev);
  Accessibility_DeviceEventController_generateKeyEvent (device_event_controller,
							keyval,
							(unsigned long) type,
							&ev);
}

/**
 * generateMouseEvent:
 * @x: a #long indicating the screen x coordinate of the mouse event.
 * @y: a #long indicating the screen y coordinate of the mouse event.
 * @name: a string indicating which mouse event to be synthesized
 *        (e.g. "button1", "button2", "mousemove").
 *
 * Synthesize a mouse event at a specific screen coordinate.
 * Not Yet Implemented.
 *
 **/
void
generateMouseEvent (long x, long y, char *name)
{
  ;
}

