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

/*
 * listener.c: test for accessibility implementation
 *
 */

#ifdef SPI_DEBUG
#include <stdio.h>
#endif

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <config.h>
#include <gdk/gdkx.h> /* TODO: hide dependency (wrap in single porting file) */
#include <gdk/gdkwindow.h>
#include <bonobo/Bonobo.h>
#include <libspi/Accessibility.h>

/*
 * This pulls the definition for the BonoboObject (GType)
 */
#include "deviceeventcontroller.h"

/*
 * Our parent Gtk object type
 */
#define PARENT_TYPE BONOBO_OBJECT_TYPE

/*
 * A pointer to our parent object class
 */
static GObjectClass *spi_device_event_controller_parent_class;

static gboolean kbd_registered = FALSE;

static Display *display;

static Window root_window;

typedef enum {
  DEVICE_TYPE_KBD,
  DEVICE_TYPE_MOUSE,
  DEVICE_TYPE_LAST_DEFINED
} DeviceTypeCategory;

static gboolean _controller_register_with_devices (SpiDeviceEventController *controller);
static gboolean _controller_grab_keyboard (SpiDeviceEventController *controller);

static void _controller_register_device_listener (SpiDeviceEventController *controller,
						  const CORBA_Object l,
						  const Accessibility_ControllerEventMask *mask,
						  DeviceTypeCategory type,
						  CORBA_Environment *ev);

/*
 * Private methods
 */

static gint
_compare_corba_objects (gconstpointer p1, gconstpointer p2)
{
  CORBA_Environment ev;
  gint retval;
  retval = !CORBA_Object_is_equivalent ((CORBA_Object) p1, (CORBA_Object) p2, &ev);

#ifdef SPI_DEBUG
  fprintf (stderr, "comparing %p to %p; result %d\n",
	   p1, p2,
	   retval);
#endif
  return retval;  
}

static gint
_eventmask_compare_value (gconstpointer p1, gconstpointer p2)
{
    long d;
    if (!p1 || !p2)
	return (gint) (p1?1:(p2?-1:0));
    else
	d = ((long)((Accessibility_ControllerEventMask*)p2)->value) -
		((long)((Accessibility_ControllerEventMask*)p1)->value);
    return (gint) d;
}

static void
_controller_register_device_listener (SpiDeviceEventController *controller,
				      const CORBA_Object l,
				      const Accessibility_ControllerEventMask *mask,
				      DeviceTypeCategory type,
				      CORBA_Environment *ev)
{
  Accessibility_ControllerEventMask *mask_ptr = NULL;
  
  switch (type) {
  case DEVICE_TYPE_KBD:
      controller->key_listeners = g_list_append (controller->key_listeners,
						 CORBA_Object_duplicate (l, ev));
      
      mask_ptr = (Accessibility_ControllerEventMask *)
	      g_list_find_custom (controller->keymask_list, (gpointer) mask,
				  _eventmask_compare_value);
      if (mask_ptr)
	      ++(mask_ptr->refcount);
      else
      {
	      if (mask->refcount != (CORBA_unsigned_short) 1)
		      fprintf (stderr, "mask initial refcount is not 1!\n");
	      if (mask->value > (CORBA_unsigned_long) 2048)
		      fprintf (stderr, "mask value looks invalid (%lu)\n",
			       (unsigned long) mask->value);
	      else
		      fprintf (stderr, "appending mask with val=%lu\n",
			       (unsigned long) mask->value);
	      mask_ptr = Accessibility_ControllerEventMask__alloc();
	      mask_ptr->value = mask->value;
	      mask_ptr->refcount = (CORBA_unsigned_short) 1;
	      controller->keymask_list = g_list_append (controller->keymask_list,
							(gpointer) mask_ptr);
      }
      break;
  case DEVICE_TYPE_MOUSE:
/*    controller->mouse_listeners = g_list_append (controller->mouse_listeners,
                                                   CORBA_Object_duplicate (l, ev));*/

/* possibly this interface should NOT be used for mouse events ? */
      break;
  }
}

static void
_controller_deregister_device_listener (SpiDeviceEventController *controller,
					const CORBA_Object l,
					const Accessibility_ControllerEventMask *mask,
					DeviceTypeCategory type,
					CORBA_Environment *ev)
{
  Accessibility_ControllerEventMask *mask_ptr;
  GList *list_ptr;
  switch (type) {
  case DEVICE_TYPE_KBD:
      list_ptr = g_list_find_custom (controller->key_listeners, l, _compare_corba_objects);
      if (list_ptr)
	  controller->key_listeners = g_list_remove (controller->key_listeners, list_ptr);
      list_ptr = (GList *)
	          g_list_find_custom (controller->keymask_list, (gpointer) mask,
				     _eventmask_compare_value);
      if (list_ptr)
        {
	  mask_ptr = (Accessibility_ControllerEventMask *) list_ptr->data;
          if (mask_ptr)
	      --mask_ptr->refcount;
          if (!mask_ptr->refcount)
            {
	      controller->keymask_list =
		      g_list_remove (controller->keymask_list, mask_ptr);
	      ;  /* TODO: release any key grabs that are in place for this key mask */
	    }
	}
      break;
  case DEVICE_TYPE_MOUSE:
/*    controller->mouse_listeners = g_list_append (controller->mouse_listeners,
                                                   CORBA_Object_duplicate (l, ev));*/

/* possibly this interface should NOT be used for mouse events ? */
      break;
  }
}

static gboolean
_controller_register_with_devices (SpiDeviceEventController *controller)
{
  gboolean retval = FALSE;

  /* calls to device-specific implementations and routines go here */
  /* register with: keyboard hardware code handler */
  /* register with: (translated) keystroke handler */
#ifdef SPI_DEBUG
  fprintf (stderr, "About to request events on window %ld of display %x\n",
	   (unsigned long) GDK_ROOT_WINDOW(), GDK_DISPLAY());
#endif
  /* We must open a new connection to the server to avoid clashing with the GDK event loop */
  display = XOpenDisplay (getenv ("DISPLAY"));
  root_window = DefaultRootWindow (display);		
  XSelectInput (display,
		root_window,
		KeyPressMask | KeyReleaseMask);
  /* register with: mouse hardware device handler? */
  /* register with: mouse event handler */
  return retval;
}

static gboolean
_check_key_event (SpiDeviceEventController *controller)
{
	static gboolean initialized = FALSE;
	static gboolean is_active = FALSE;
	XEvent *x_event = g_new0 (XEvent, 1);
	XKeyEvent *x_key_event;
	KeySym keysym;
	gboolean is_consumed = FALSE;
	char key_name[16];
	int i;
	int n_listeners = g_list_length (controller->key_listeners);
	Accessibility_KeyStroke key_event;
	static CORBA_Environment ev;

	if (!initialized)
	{
	  initialized = TRUE;
	  CORBA_exception_init (&ev);
	}

	while (XPending(display))
	  {
	    XNextEvent (display, x_event);
	    if (XFilterEvent (x_event, None)) continue;	  
	    if (x_event->type == KeyPress)
	      {
	        x_key_event = (XKeyEvent *)x_event;
		keysym = XLookupKeysym (x_key_event, 0);
		key_event.keyID = (CORBA_long)(keysym);
		key_event.keycode = (CORBA_short) x_key_event->keycode;
		key_event.type = Accessibility_KEY_PRESSED;
		key_event.modifiers = (CORBA_unsigned_short)(x_key_event->state);
#ifdef SPI_KEYEVENT_DEBUG
	    fprintf (stderr,
		     "Key %lu pressed (%c), modifiers %d\n",
		     (unsigned long) keysym,
		     keysym ? (int) keysym : '*',
		     (int) x_key_event->state);
#endif
#ifdef SPI_DEBUG
	    fprintf(stderr, "%s%c",
		    (x_key_event->state & Mod1Mask)?"Alt-":"",
		    ((x_key_event->state & ShiftMask)^(x_key_event->state & LockMask))?
		    (char) toupper((int) keysym) : (char) tolower((int)keysym));
#endif /* SPI_DEBUG */
	      }
	    else
	    {
#ifdef SPI_KEYEVENT_DEBUG
		    fprintf (stderr, "other event, type %d\n", (int) x_event->type);
#endif
	    }
	    /* relay to listeners, and decide whether to consume it or not */
	    for (i=0; i<n_listeners && !is_consumed; ++i)
	    {
		    Accessibility_KeystrokeListener ls;
		    ls = (Accessibility_KeystrokeListener)
			    g_list_nth_data (controller->key_listeners, i);
		    if (!CORBA_Object_is_nil(ls, &ev))
		    {
			    is_consumed = Accessibility_KeystrokeListener_keyEvent (ls, &key_event, &ev);
		    }		
	    }
	    if (is_consumed)
	    {
	      XAllowEvents (display, SyncKeyboard, CurrentTime);
	    }
	    else
	    {
	      XAllowEvents (display, ReplayKeyboard, CurrentTime);
	    }
	  }
	XUngrabKey (display, AnyKey, AnyModifier, root_window);
	return _controller_grab_keyboard (controller);
}

static gboolean
_controller_grab_keyboard (SpiDeviceEventController *controller)
{
	GList *maskList = controller->keymask_list;
	int i;
	int last_mask;
	last_mask = g_list_length (maskList);

/*
 * masks known to work with default RH 7.1: 
 * 0 (no mods), LockMask, Mod1Mask, Mod2Mask, ShiftMask,
 * ShiftMask|LockMask, Mod1Mask|LockMask, Mod2Mask|LockMask,
 * ShiftMask|Mod1Mask, ShiftMask|Mod2Mask, Mod1Mask|Mod2Mask,
 * ShiftMask|LockMask|Mod1Mask, ShiftMask|LockMask|Mod2Mask,
 *
 * ControlMask grabs are broken, must be in use already
 */
	
	for (i=0; i < last_mask; ++i)
	{
		Accessibility_ControllerEventMask *mask
			= (Accessibility_ControllerEventMask *)g_list_nth_data (maskList, i);
		unsigned long maskVal = 0xFFFFFFFF;
		if (mask) maskVal = (unsigned long) mask->value;
#ifdef SPI_KEYEVENT_DEBUG
		fprintf (stderr, "mask=%lx\n", maskVal);
#endif
		if (!(maskVal & ControlMask))
		{
			XGrabKey (display,
				  AnyKey,
				  maskVal,
				  root_window,
				  True,
				  GrabModeAsync,
				  GrabModeSync);
			/* TODO: check call for errors and return FALSE if error occurs */
		} else {
			return FALSE; /* can't do control key yet */
		}
	}
	return TRUE;
}

/*
 * Implemented GObject::finalize
 */
static void
spi_device_event_controller_object_finalize (GObject *object)
{

#ifdef SPI_DEBUG
        fprintf(stderr, "spi_device_event_controller_object_finalize called\n");
#endif
        spi_device_event_controller_parent_class->finalize (object);
}

/*
 * CORBA Accessibility::SpiDeviceEventController::registerKeystrokeListener
 *     method implementation
 */
static void
impl_register_keystroke_listener (PortableServer_Servant     servant,
				  const Accessibility_KeystrokeListener l,
				  const Accessibility_KeySet *keys,
				  const Accessibility_ControllerEventMask *mask,
				  const Accessibility_KeyEventTypeSeq *type,
				  const CORBA_boolean is_synchronous,
				  CORBA_Environment         *ev)
{
	SpiDeviceEventController *controller = SPI_DEVICE_EVENT_CONTROLLER (
		bonobo_object_from_servant (servant));
#ifdef SPI_DEBUG
	fprintf (stderr, "registering keystroke listener %p with maskVal %lu\n",
		 (void *) l, (unsigned long) mask->value);
#endif
        /* TODO: change this to an enum, indicating if event can be consumed */
	if (is_synchronous)
	_controller_register_device_listener(controller, l, mask, DEVICE_TYPE_KBD, ev);
	else
	; /* register with toolkit instead */	
}
/*
 * CORBA Accessibility::SpiDeviceEventController::deregisterKeystrokeListener
 *     method implementation
 */
static void
impl_deregister_keystroke_listener (PortableServer_Servant     servant,
				    const Accessibility_KeystrokeListener l,
				    const Accessibility_KeySet *keys,
				    const Accessibility_ControllerEventMask *mask,
				    const Accessibility_KeyEventTypeSeq *type,
				    const CORBA_boolean is_synchronous,
				    CORBA_Environment         *ev)
{
	SpiDeviceEventController *controller = SPI_DEVICE_EVENT_CONTROLLER (
		bonobo_object_from_servant (servant));
#ifdef SPI_DEBUG
	fprintf (stderr, "deregistering keystroke listener %p with maskVal %lu\n",
		 (void *) l, (unsigned long) mask->value);
#endif
	_controller_deregister_device_listener(controller, l, mask, DEVICE_TYPE_KBD, ev);
}

/*
 * CORBA Accessibility::SpiDeviceEventController::registerMouseListener
 *     method implementation
 */
/*
static void
impl_register_mouse_listener (PortableServer_Servant     servant,
			      const Accessibility_MouseListener *l,
			      CORBA_Environment         *ev)
{
	SpiDeviceEventController *controller = SPI_DEVICE_EVENT_CONTROLLER (
		bonobo_object_from_servant (servant));
#ifdef SPI_DEBUG
	fprintf (stderr, "registering mouse listener %p\n", l);
#endif
	_controller_register_device_listener(controller, l, mask, DEVICE_TYPE_MOUSE, ev);
}
*/

static KeyCode
keycode_for_keysym (long keysym)
{
  return XKeysymToKeycode (display, (KeySym) keysym);
}

/*
 * CORBA Accessibility::SpiDeviceEventController::registerKeystrokeListener
 *     method implementation
 */
static void
impl_generate_key_event (PortableServer_Servant     servant,
			 const CORBA_long           keycode,
			 const CORBA_long           synth_type,
			 CORBA_Environment          *ev)
{
	long key_synth_code;
#ifdef SPI_DEBUG
	fprintf (stderr, "synthesizing keystroke %ld\n", (long) keycode);
#endif
	/* TODO: hide/wrap/remove X dependency */

	/* TODO: be accessX-savvy so that keyrelease occurs after sufficient timeout */
	
	/*
	 * TODO: when initializing, query for XTest extension before using,
	 * and fall back to XSendEvent() if XTest is not available.
	 */
	
	switch (synth_type)
	{
	case Accessibility_KEY_PRESS:
		XTestFakeKeyEvent (GDK_DISPLAY(), (unsigned int) keycode, True, CurrentTime);
		break;
	case Accessibility_KEY_PRESSRELEASE:
		XTestFakeKeyEvent (GDK_DISPLAY(), (unsigned int) keycode, True, CurrentTime);
	case Accessibility_KEY_RELEASE:
		XTestFakeKeyEvent (GDK_DISPLAY(), (unsigned int) keycode, False, CurrentTime);
		break;
	case Accessibility_KEY_SYM:
		key_synth_code = keycode_for_keysym (keycode);
		XTestFakeKeyEvent (GDK_DISPLAY(), (unsigned int) key_synth_code, True, CurrentTime);
		XTestFakeKeyEvent (GDK_DISPLAY(), (unsigned int) key_synth_code, False, CurrentTime);
		break;
	}
}

/*
 * CORBA Accessibility::SpiDeviceEventController::generateMouseEvent
 *     method implementation
 */
static void
impl_generate_mouse_event (PortableServer_Servant     servant,
			   const CORBA_long x,
			   const CORBA_long y,
			   const CORBA_char * eventName,
			   CORBA_Environment         *ev)
{
#ifdef SPI_DEBUG
	fprintf (stderr, "generating mouse %s event at %ld, %ld\n", eventName, x, y);
#endif
}

static void
spi_device_event_controller_class_init (SpiDeviceEventControllerClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        POA_Accessibility_SpiDeviceEventController__epv *epv = &klass->epv;
        spi_device_event_controller_parent_class = g_type_class_ref (BONOBO_OBJECT_TYPE);

        object_class->finalize = spi_device_event_controller_object_finalize;

        epv->registerKeystrokeListener = impl_register_keystroke_listener;
        epv->deregisterKeystrokeListener = impl_deregister_keystroke_listener;
/*        epv->registerMouseListener = impl_register_mouse_listener; */
        epv->generateKeyEvent = impl_generate_key_event;
        epv->generateMouseEvent = impl_generate_mouse_event;
	klass->check_key_event = _check_key_event;
}

static void
spi_device_event_controller_init (SpiDeviceEventController *device_event_controller)
{
  device_event_controller->key_listeners = NULL;
  device_event_controller->mouse_listeners = NULL;
  device_event_controller->keymask_list = NULL;
  kbd_registered = _controller_register_with_devices (device_event_controller);
}

gboolean spi_device_event_controller_check_key_event (SpiDeviceEventController *controller)
{
	SpiDeviceEventControllerClass *klass = SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS (controller);
	if (klass->check_key_event)
		return (klass->check_key_event) (controller);
}

GType
spi_device_event_controller_get_type (void)
{
        static GType type = 0;

        if (!type) {
                static const GTypeInfo tinfo = {
                        sizeof (SpiDeviceEventControllerClass),
                        (GBaseInitFunc) NULL,
                        (GBaseFinalizeFunc) NULL,
                        (GClassInitFunc) spi_device_event_controller_class_init,
                        (GClassFinalizeFunc) NULL,
                        NULL, /* class data */
                        sizeof (SpiDeviceEventController),
                        0, /* n preallocs */
                        (GInstanceInitFunc) spi_device_event_controller_init,
                        NULL /* value table */
                };
                /*
                 *   Here we use bonobo_type_unique instead of
                 * gtk_type_unique, this auto-generates a load of
                 * CORBA structures for us. All derived types must
                 * use bonobo_type_unique.
                 */
                type = bonobo_type_unique (
                        PARENT_TYPE,
                        POA_Accessibility_SpiDeviceEventController__init,
                        NULL,
                        G_STRUCT_OFFSET (SpiDeviceEventControllerClass, epv),
                        &tinfo,
                        "SpiDeviceEventController");
        }

        return type;
}

