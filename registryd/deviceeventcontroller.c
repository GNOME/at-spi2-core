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

/* deviceeventcontroler.c: implement the DeviceEventController interface */

#include <config.h>

#undef SPI_DEBUG

#ifdef SPI_DEBUG
#  include <stdio.h>
#endif

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#define XK_MISCELLANY
#include <X11/keysymdef.h>
#include <gdk/gdkx.h> /* TODO: hide dependency (wrap in single porting file) */
#include <gdk/gdkwindow.h>

#include "deviceeventcontroller.h"

/* Our parent Gtk object type */
#define PARENT_TYPE BONOBO_TYPE_OBJECT

/* A pointer to our parent object class */
static GObjectClass *spi_device_event_controller_parent_class;

static gboolean kbd_registered = FALSE;

static Display *display;

static Window root_window;

typedef enum {
  SPI_DEVICE_TYPE_KBD,
  SPI_DEVICE_TYPE_MOUSE,
  SPI_DEVICE_TYPE_LAST_DEFINED
} SpiDeviceTypeCategory;

struct _DEControllerGrabMask {
  Accessibility_ControllerEventMask modmask;
  CORBA_unsigned_long               keyval;
  unsigned int                      refcount;
};

typedef struct _DEControllerGrabMask DEControllerGrabMask;

struct _DEControllerListener {
  CORBA_Object          object;
  SpiDeviceTypeCategory type;
};

typedef struct _DEControllerListener DEControllerListener;

struct _DEControllerKeyListener {
  DEControllerListener listener;
  Accessibility_KeySet *keys;
  Accessibility_ControllerEventMask mask;
  Accessibility_KeyEventTypeSeq *typeseq;
  gboolean is_system_global;	
};

typedef struct _DEControllerKeyListener DEControllerKeyListener;

static gboolean spi_controller_register_with_devices (SpiDeviceEventController *controller);
static gboolean spi_controller_grab_keyboard (SpiDeviceEventController *controller);

static void spi_controller_register_device_listener (SpiDeviceEventController *controller,
						     DEControllerListener *l,
						     CORBA_Environment *ev);

/*
 * Private methods
 */

static DEControllerGrabMask * 
spi_grabmask_clone (DEControllerGrabMask *grabmask)
{
  DEControllerGrabMask *clone = g_new0 (DEControllerGrabMask, 1);
  memcpy (clone, grabmask, sizeof (DEControllerGrabMask));
  return clone;
}

static gint
spi_compare_corba_objects (gconstpointer p1, gconstpointer p2)
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
spi_compare_listeners (gconstpointer p1, gconstpointer p2)
{
  DEControllerListener *l1 = (DEControllerListener *) p1;	
  DEControllerListener *l2 = (DEControllerListener *) p2;	
  return spi_compare_corba_objects (l1->object, l2->object);
}

static gint
spi_grabmask_compare_values (gconstpointer p1, gconstpointer p2)
{
  DEControllerGrabMask *l1;
  DEControllerGrabMask *l2;
  if (p1 == p2)
    {
      return 0;
    }
  else
    { 
      l1 = (DEControllerGrabMask *) p1;	
      l2 = (DEControllerGrabMask *) p2;
      return ((l1->modmask != l2->modmask) || (l1->keyval != l2->keyval));
    }
}

static DEControllerKeyListener *
spi_dec_key_listener_new (CORBA_Object l,
			  const Accessibility_KeySet *keys,
			  const Accessibility_ControllerEventMask mask,
			  const Accessibility_KeyEventTypeSeq *typeseq,
			  const CORBA_boolean is_system_global,
			  CORBA_Environment *ev)
{
  DEControllerKeyListener *key_listener = g_new0 (DEControllerKeyListener, 1);
  key_listener->listener.object = bonobo_object_dup_ref (l, ev);
  key_listener->listener.type = SPI_DEVICE_TYPE_KBD;
  key_listener->keys = ORBit_copy_value (keys, TC_Accessibility_KeySet);
  key_listener->mask = mask;
  key_listener->typeseq = ORBit_copy_value (typeseq, TC_Accessibility_KeyEventTypeSeq);
  key_listener->is_system_global = is_system_global;

#ifdef SPI_DEBUG
  g_print ("new listener, with mask %x, is_global %d, keys %p\n",
	   (unsigned int) key_listener->mask,
           (int) key_listener->is_system_global,
	   (void *) key_listener->keys);
#endif
  return key_listener;	
}

static void
spi_dec_key_listener_free (DEControllerKeyListener *key_listener, CORBA_Environment *ev)
{
  bonobo_object_release_unref (key_listener->listener.object, ev);
  CORBA_free (key_listener->typeseq);
  CORBA_free (key_listener->keys);
  g_free (key_listener);
}

static void
spi_controller_deregister_global_keygrabs (SpiDeviceEventController *controller,
					   DEControllerKeyListener *key_listener)
{
  GList *list_ptr;
  DEControllerGrabMask *mask_ptr;
  /* TODO: implement this! Also remember to release any keygrabs still held */
  ;
}

static void
spi_controller_register_global_keygrabs (SpiDeviceEventController *controller,
					 DEControllerKeyListener *key_listener)
{
  DEControllerGrabMask grabmask, *grabmask_ptr;
  GList *list_ptr;
  gint i;
  /* TODO: deregistration version of this function */
  
  grabmask.modmask = key_listener->mask;
  if (key_listener->keys->_length == 0) /* special case means AnyKey/AllKeys */
    {
      grabmask.keyval = AnyKey;
      list_ptr = g_list_find_custom (controller->keygrabs_list, &grabmask,
				     spi_grabmask_compare_values);
      if (list_ptr)
        {
          grabmask_ptr = (DEControllerGrabMask *) list_ptr->data;
	  grabmask_ptr->refcount++;
        }
      else
        {
	  controller->keygrabs_list =
		  g_list_prepend (controller->keygrabs_list,
				  spi_grabmask_clone (&grabmask));
        }
    }
  else
    {
      for (i = 0; i < key_listener->keys->_length; ++i)
        {
	  long int keyval = key_listener->keys->_buffer[i];
	  /* X Grabs require keycodes, not keysyms */
	  if (keyval >= 0)
	    {
	      keyval = XKeysymToKeycode(display, (KeySym) keyval);		    
	    }
	  grabmask.keyval = keyval;
          list_ptr = g_list_find_custom (controller->keygrabs_list, &grabmask,
					     spi_grabmask_compare_values);
          if (list_ptr)
            {
	      grabmask_ptr = (DEControllerGrabMask *) list_ptr->data;
              grabmask_ptr->refcount++;
            }
          else
            {
	      controller->keygrabs_list =
		  g_list_prepend (controller->keygrabs_list,
				  spi_grabmask_clone (&grabmask));
	      fprintf (stderr, "appending mask with val=%lu\n",
		       (unsigned long) grabmask.modmask);
            }
        }
    }
}

static void
spi_controller_register_device_listener (SpiDeviceEventController *controller,
					 DEControllerListener *listener,
					 CORBA_Environment *ev)
{
  DEControllerKeyListener *key_listener;
  
  switch (listener->type) {
  case SPI_DEVICE_TYPE_KBD:
      key_listener = (DEControllerKeyListener *) listener;  	  
      controller->key_listeners = g_list_prepend (controller->key_listeners, key_listener);
      if (key_listener->is_system_global)
        {
	  spi_controller_register_global_keygrabs (controller, key_listener);	
	}
      break;
  case SPI_DEVICE_TYPE_MOUSE:
/*    controller->mouse_listeners = g_list_append (controller->mouse_listeners,
                                                   CORBA_Object_duplicate (l, ev));*/

/* this interface should only be used for mouse motion events, not mouse clicks events */
      break;
  }
}

static void
spi_controller_deregister_device_listener (SpiDeviceEventController *controller,
					   DEControllerListener *listener,
					   CORBA_Environment *ev)
{
  Accessibility_ControllerEventMask *mask_ptr;
  DEControllerListener *dec_listener;
  GList *list_ptr;
  switch (listener->type)
    {
      case SPI_DEVICE_TYPE_KBD:
        spi_controller_deregister_global_keygrabs (controller,
						   (DEControllerKeyListener *) listener);

        /* now, remove this listener from the keylistener list */
        list_ptr = g_list_find_custom (controller->key_listeners, listener, spi_compare_listeners);
        if (list_ptr)
          {
	    dec_listener = (DEControllerListener *) list_ptr->data;
#ifdef SPI_DEBUG	  
	    g_print ("removing keylistener %p\n", dec_listener->object);
#endif
	    controller->key_listeners = g_list_remove_link (controller->key_listeners,
							  list_ptr);
	    spi_dec_key_listener_free ((DEControllerKeyListener *) dec_listener, ev);
	  }
        break;
      case SPI_DEVICE_TYPE_MOUSE: /* TODO: implement */
        break;
    }
}

static gboolean
spi_controller_register_with_devices (SpiDeviceEventController *controller)
{
  gboolean retval = FALSE;

  /* calls to device-specific implementations and routines go here */
  /* register with: keyboard hardware code handler */
  /* register with: (translated) keystroke handler */
#ifdef SPI_DEBUG
  fprintf (stderr, "About to request events on window %ld of display %p\n",
	   (unsigned long) GDK_ROOT_WINDOW(), GDK_DISPLAY());
#endif
  /* We must open a new connection to the server to avoid clashing with the GDK event loop */
  display = XOpenDisplay (g_getenv ("DISPLAY"));
  root_window = DefaultRootWindow (display);		
  XSelectInput (display,
		root_window,
		KeyPressMask | KeyReleaseMask);
  /* register with: mouse hardware device handler? */
  /* register with: mouse event handler */
  return retval;
}

static gboolean
spi_key_set_contains_key (Accessibility_KeySet *key_set, const Accessibility_DeviceEvent *key_event)
{
  gint i;
  gint len;

  /* g_assert (key_set); */
  if (!key_set) { g_print ("null key set!"); return TRUE; }

  len = key_set->_length;
  
  if (len == 0) /* special case, means "all keys/any key" */
    {
      g_print ("anykey\n");	    
      return TRUE;
    }

  for (i=0; i<len; ++i)
    {
#ifdef SPI_KEYEVENT_DEBUG	    
      g_print ("key_set[%d] = %d; key_event %d, code %d\n",
	        i,
	       (int) key_set->_buffer[i],
	       (int) key_event->id,
	       (int) key_event->hw_code); 
#endif
      if (key_set->_buffer[i] == (CORBA_long) key_event->id) return TRUE;
      if (key_set->_buffer[i] == (CORBA_long) -key_event->hw_code) return TRUE;
    }
  
  return FALSE;
}

static gboolean
spi_key_eventtype_seq_contains_event (Accessibility_KeyEventTypeSeq *type_seq,
				  const Accessibility_DeviceEvent *key_event)
{
  gint i;
  gint len;

  /* g_assert (type_seq); */
  if (!type_seq) { g_print ("null type seq!"); return TRUE; }

  len = type_seq->_length;
  
  if (len == 0) /* special case, means "all events/any event" */
    {
      return TRUE;
    }

  for (i=0; i<len; ++i)
    {
#ifdef SPI_DEBUG	    
      g_print ("type_seq[%d] = %d; key event type = %d\n", i, (int) type_seq->_buffer[i],
	       (int) key_event->type);
#endif      
      if (type_seq->_buffer[i] == (CORBA_long) key_event->type) return TRUE;	    
    }
  
  return FALSE;
}

static gboolean
spi_key_event_matches_listener (const Accessibility_DeviceEvent *key_event,
			    DEControllerKeyListener *listener,
			    CORBA_boolean is_system_global)
{
  if ((key_event->modifiers == (CORBA_unsigned_short) (listener->mask & 0xFFFF)) &&
       spi_key_set_contains_key (listener->keys, key_event) &&
       spi_key_eventtype_seq_contains_event (listener->typeseq, key_event) && 
      (is_system_global == listener->is_system_global))
    {
      return TRUE;
    }
  else
    return FALSE;
}

static gboolean
spi_notify_keylisteners (GList *key_listeners,
			 const Accessibility_DeviceEvent *key_event,
			 CORBA_boolean is_system_global,
			 CORBA_Environment *ev)
{
  int i, n_listeners = g_list_length (key_listeners);
  gboolean is_consumed = FALSE;

  for (i=0; i<n_listeners && !is_consumed; ++i)
    {
      Accessibility_DeviceEventListener ls;
      DEControllerKeyListener *key_listener = (DEControllerKeyListener *)
	    g_list_nth_data (key_listeners, i);
      ls = (Accessibility_DeviceEventListener) key_listener->listener.object;
      if (spi_key_event_matches_listener (key_event, key_listener, is_system_global))
        {
          if (!CORBA_Object_is_nil(ls, ev))
            {
	      is_consumed = Accessibility_DeviceEventListener_notifyEvent (ls, key_event, ev);
            }		
        }
      else
        {
#ifdef SPI_KEYEVENT_DEBUG
	      g_print ("no match for listener %d\n", i);
#endif
	      ;
	}
    }
  return is_consumed;
}

static Accessibility_DeviceEvent
spi_keystroke_from_x_key_event (XKeyEvent *x_key_event)
{
  Accessibility_DeviceEvent key_event;
  KeySym keysym;
  const int cbuf_bytes = 20;
  char cbuf [cbuf_bytes];
  
  keysym = XLookupKeysym (x_key_event, 0);
  key_event.id = (CORBA_long)(keysym);
  key_event.hw_code = (CORBA_short) x_key_event->keycode;
  if (((XEvent *) x_key_event)->type == KeyPress)
    {
      key_event.type = Accessibility_KEY_PRESSED;
    }
  else
    {
      key_event.type = Accessibility_KEY_RELEASED;
    } 
  key_event.modifiers = (CORBA_unsigned_short)(x_key_event->state);
  key_event.is_text = CORBA_FALSE;
  switch (keysym)
    {
      case ' ':
        key_event.event_string = CORBA_string_dup ("space");
        break;
      case XK_Tab:
        key_event.event_string = CORBA_string_dup ("Tab");
	break;
      case XK_BackSpace:
        key_event.event_string = CORBA_string_dup ("Backspace");
	break;
      case XK_Return:
        key_event.event_string = CORBA_string_dup ("Return");
	break;
      default:
        if (XLookupString (x_key_event, cbuf, cbuf_bytes, &keysym, NULL) > 0)
          {
            key_event.event_string = CORBA_string_dup (cbuf);
	    if (isgraph (keysym))
	      {
	        key_event.is_text = CORBA_TRUE; /* FIXME: incorrect for some composed chars? */
	      }
          }
        else
          {
            key_event.event_string = CORBA_string_dup ("");
          }
    }

  key_event.timestamp = (CORBA_unsigned_long) x_key_event->time;
#ifdef SPI_KEYEVENT_DEBUG
  fprintf (stderr,
     "Key %lu pressed (%c), modifiers %d\n",
     (unsigned long) keysym,
     keysym ? (int) keysym : '*',
     (int) x_key_event->state);
#endif
#ifdef SPI_DEBUG
  fprintf (stderr, "%s%c",
     (x_key_event->state & Mod1Mask)?"Alt-":"",
     ((x_key_event->state & ShiftMask)^(x_key_event->state & LockMask))?
     g_ascii_toupper (keysym) : g_ascii_tolower (keysym));
#endif /* SPI_DEBUG */
  return key_event;	
}


static gboolean
spi_check_key_event (SpiDeviceEventController *controller)
{
	static gboolean initialized = FALSE;
	XEvent *x_event = g_new0 (XEvent, 1);
	XKeyEvent *x_key_event;
	gboolean is_consumed = FALSE;
	Accessibility_DeviceEvent key_event;
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
	    if (x_event->type == KeyPress || x_event->type == KeyRelease)
	      {
	        key_event = spi_keystroke_from_x_key_event ((XKeyEvent *) x_event);
	        /* relay to listeners, and decide whether to consume it or not */
	        is_consumed = spi_notify_keylisteners (controller->key_listeners, &key_event, CORBA_TRUE, &ev);
	      }
	    else
	      {
#ifdef SPI_KEYEVENT_DEBUG
	        fprintf (stderr, "other event, type %d\n", (int) x_event->type);
#endif
	      }

	    if (is_consumed)
	      {
	        XAllowEvents (display, AsyncKeyboard, CurrentTime);
	      }
	    else
	      {
	        XAllowEvents (display, ReplayKeyboard, CurrentTime);
	      }
	  }
	XUngrabKey (display, AnyKey, AnyModifier, root_window);

	return spi_controller_grab_keyboard (controller);
}

static gboolean
spi_controller_grab_keyboard (SpiDeviceEventController *controller)
{
  GList *maskList = controller->keygrabs_list;
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
      DEControllerGrabMask * grab_mask
		= (DEControllerGrabMask *) g_list_nth_data (maskList, i);
      unsigned long maskVal = 0xFFFFFFFF;
      int           keyVal = AnyKey;
      if (grab_mask)
        {
	  maskVal = (unsigned long) grab_mask->modmask;
	  keyVal =  grab_mask->keyval;
        }
#ifdef SPI_DEBUG
      fprintf (stderr, "mask=%lx\n", maskVal);
#endif
      if (!(maskVal & ControlMask))
  	{
	  XGrabKey (display,
		    keyVal, 
		    maskVal,
		    root_window,
		    True,
		    GrabModeAsync,
		    GrabModeAsync);
	  /* TODO: check call for errors and return FALSE if error occurs */
	}
      else
        {
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
	/* disconnect any special listeners, get rid of outstanding keygrabs */
	
        spi_device_event_controller_parent_class->finalize (object);
}

/*
 * CORBA Accessibility::DeviceEventController::registerKeystrokeListener
 *     method implementation
 */
static void
impl_register_keystroke_listener (PortableServer_Servant     servant,
				  const Accessibility_DeviceEventListener l,
				  const Accessibility_KeySet *keys,
				  const Accessibility_ControllerEventMask mask,
				  const Accessibility_KeyEventTypeSeq *type,
				  const CORBA_boolean is_system_global,
				  CORBA_Environment         *ev)
{
	SpiDeviceEventController *controller = SPI_DEVICE_EVENT_CONTROLLER (
		bonobo_object_from_servant (servant));
	DEControllerKeyListener *dec_listener;
#ifdef SPI_DEBUG
	fprintf (stderr, "registering keystroke listener %p with maskVal %lu\n",
		 (void *) l, (unsigned long) mask);
#endif
	dec_listener = spi_dec_key_listener_new (l, keys, mask, type, is_system_global, ev);
	spi_controller_register_device_listener (controller, (DEControllerListener *) dec_listener, ev);
}

/*
 * CORBA Accessibility::DeviceEventController::deregisterKeystrokeListener
 *     method implementation
 */
static void
impl_deregister_keystroke_listener (PortableServer_Servant     servant,
				    const Accessibility_DeviceEventListener l,
				    const Accessibility_KeySet *keys,
				    const Accessibility_ControllerEventMask mask,
				    const Accessibility_KeyEventTypeSeq *type,
				    const CORBA_boolean is_system_global,
				    CORBA_Environment         *ev)
{
	SpiDeviceEventController *controller = SPI_DEVICE_EVENT_CONTROLLER (
		bonobo_object_from_servant (servant));
	DEControllerKeyListener *key_listener = spi_dec_key_listener_new (l,
									  keys,
									  mask,
									  type,
									  is_system_global,
									  ev);
#ifdef SPI_DEREGISTER_DEBUG
	fprintf (stderr, "deregistering keystroke listener %p with maskVal %lu\n",
		 (void *) l, (unsigned long) mask->value);
#endif
	spi_controller_deregister_device_listener(controller,
					      (DEControllerListener *) key_listener,
					      ev);
	spi_dec_key_listener_free (key_listener, ev);
}

/*
 * CORBA Accessibility::DeviceEventController::registerMouseListener
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
	spi_controller_register_device_listener(controller, DEVICE_TYPE_MOUSE, l, keys, mask, ev);
}
*/

static KeyCode
keycode_for_keysym (long keysym)
{
  return XKeysymToKeycode (display, (KeySym) keysym);
}

/*
 * CORBA Accessibility::DeviceEventController::registerKeystrokeListener
 *     method implementation
 */
static void
impl_generate_key_event (PortableServer_Servant     servant,
			 const CORBA_long           keycode,
			 const Accessibility_KeySynthType synth_type,
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

/* Accessibility::DeviceEventController::generateMouseEvent */
static void
impl_generate_mouse_event (PortableServer_Servant servant,
			   const CORBA_long       x,
			   const CORBA_long       y,
			   const CORBA_char      *eventName,
			   CORBA_Environment     *ev)
{
#ifdef SPI_DEBUG
  fprintf (stderr, "generating mouse %s event at %ld, %ld\n", eventName, x, y);
#endif
}

/* Accessibility::DeviceEventController::notifyListenersSync */
static CORBA_boolean
impl_notify_listeners_sync(PortableServer_Servant     servant,
			   const Accessibility_DeviceEvent *event,
			   CORBA_Environment         *ev)
{
  SpiDeviceEventController *controller = SPI_DEVICE_EVENT_CONTROLLER (
                                         bonobo_object_from_servant (servant));
#ifdef SPI_DEBUG
  g_print ("notifylistening listeners synchronously: controller %x, event id %d\n",
	   (void *) controller, (int) event->id);
#endif
  return (spi_notify_keylisteners (controller->key_listeners, event, CORBA_FALSE, ev) ?
	  CORBA_TRUE : CORBA_FALSE); 
}

/* Accessibility::DeviceEventController::notifyListenersAsync */
static void
impl_notify_listeners_async (PortableServer_Servant     servant,
			     const Accessibility_DeviceEvent *event,
			     CORBA_Environment         *ev)
{
  SpiDeviceEventController *controller = SPI_DEVICE_EVENT_CONTROLLER(
	                                 bonobo_object_from_servant (servant));
#ifdef SPI_DEBUG
  fprintf (stderr, "notifying listeners asynchronously\n");
#endif
  spi_notify_keylisteners (controller->key_listeners, event, CORBA_FALSE, ev); 
}

static void
spi_device_event_controller_class_init (SpiDeviceEventControllerClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        POA_Accessibility_DeviceEventController__epv *epv = &klass->epv;
        spi_device_event_controller_parent_class = g_type_class_peek_parent (klass);

        object_class->finalize = spi_device_event_controller_object_finalize;

        epv->registerKeystrokeListener = impl_register_keystroke_listener;
        epv->deregisterKeystrokeListener = impl_deregister_keystroke_listener;
/*        epv->registerMouseListener = impl_register_mouse_listener; */
        epv->generateKeyEvent = impl_generate_key_event;
        epv->generateMouseEvent = impl_generate_mouse_event;
	epv->notifyListenersSync = impl_notify_listeners_sync;
	epv->notifyListenersAsync = impl_notify_listeners_async;
	klass->check_key_event = spi_check_key_event;
}

static void
spi_device_event_controller_init (SpiDeviceEventController *device_event_controller)
{
  device_event_controller->key_listeners = NULL;
  device_event_controller->mouse_listeners = NULL;
  device_event_controller->keygrabs_list = NULL;
  kbd_registered = spi_controller_register_with_devices (device_event_controller);
}

gboolean
spi_device_event_controller_check_key_event (SpiDeviceEventController *controller)
{
  SpiDeviceEventControllerClass *klass = SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS (controller);
  if (klass->check_key_event)
	return (klass->check_key_event) (controller);
  return FALSE;
}

SpiDeviceEventController *
spi_device_event_controller_new (void *registryp)
{
  BonoboObject *registry = (BonoboObject *) registryp;	
  SpiDeviceEventController *retval = g_object_new (
	  SPI_DEVICE_EVENT_CONTROLLER_TYPE, NULL);
  retval->registry = registry;
  bonobo_object_ref (registry);
  return retval;
}

BONOBO_TYPE_FUNC_FULL (SpiDeviceEventController,
		       Accessibility_DeviceEventController,
		       PARENT_TYPE,
		       spi_device_event_controller);

