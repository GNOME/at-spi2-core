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

/* deviceeventcontroller.c: implement the DeviceEventController interface */

#include <config.h>

#undef  SPI_DEBUG
#undef  SPI_KEYEVENT_DEBUG

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <bonobo/bonobo-exception.h>

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#define XK_MISCELLANY
#include <X11/keysymdef.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h> /* TODO: hide dependency (wrap in single porting file) */
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkwindow.h>

#include "../libspi/spi-private.h"
#include "deviceeventcontroller.h"

/* Our parent Gtk object type */
#define PARENT_TYPE BONOBO_TYPE_OBJECT

/* A pointer to our parent object class */
static GObjectClass *spi_device_event_controller_parent_class;
static int spi_error_code = 0;
static GdkPoint *last_mouse_pos = NULL; 
static unsigned int mouse_button_state = 0;
static unsigned int mouse_button_mask =
  Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask;

int (*x_default_error_handler) (Display *display, XErrorEvent *error_event);

typedef enum {
  SPI_DEVICE_TYPE_KBD,
  SPI_DEVICE_TYPE_MOUSE,
  SPI_DEVICE_TYPE_LAST_DEFINED
} SpiDeviceTypeCategory;

typedef struct {
  guint                             ref_count : 30;
  guint                             pending_add : 1;
  guint                             pending_remove : 1;

  Accessibility_ControllerEventMask mod_mask;
  CORBA_unsigned_long               key_val;  /* KeyCode */
} DEControllerGrabMask;

typedef struct {
  CORBA_Object          object;
  SpiDeviceTypeCategory type;
} DEControllerListener;

typedef struct {
  DEControllerListener listener;

  Accessibility_KeySet             *keys;
  Accessibility_ControllerEventMask mask;
  Accessibility_KeyEventTypeSeq    *typeseq;
  Accessibility_EventListenerMode  *mode;	
} DEControllerKeyListener;

static void     spi_controller_register_with_devices          (SpiDEController           *controller);
static gboolean spi_controller_update_key_grabs               (SpiDEController           *controller,
							       Accessibility_DeviceEvent *recv);
static gboolean spi_controller_register_device_listener       (SpiDEController           *controller,
							       DEControllerListener      *l,
							       CORBA_Environment         *ev);
static void     spi_device_event_controller_forward_key_event (SpiDEController           *controller,
							       const XEvent              *event);
static void     spi_deregister_controller_key_listener (SpiDEController         *controller,
							DEControllerKeyListener *key_listener,
							CORBA_Environment       *ev);

static gboolean spi_clear_error_state (void);
static gboolean spi_dec_poll_mouse_moved (gpointer data);
static gboolean spi_dec_poll_mouse_moving (gpointer data);
static gboolean spi_dec_poll_mouse_idle (gpointer data);

#define spi_get_display() GDK_DISPLAY()

/* Private methods */

static KeyCode
keycode_for_keysym (long keysym)
{
  return XKeysymToKeycode (spi_get_display (), (KeySym) keysym);
}

static DEControllerGrabMask *
spi_grab_mask_clone (DEControllerGrabMask *grab_mask)
{
  DEControllerGrabMask *clone = g_new (DEControllerGrabMask, 1);

  memcpy (clone, grab_mask, sizeof (DEControllerGrabMask));

  clone->ref_count = 1;
  clone->pending_add = TRUE;
  clone->pending_remove = FALSE;

  return clone;
}

static void
spi_grab_mask_free (DEControllerGrabMask *grab_mask)
{
  g_free (grab_mask);
}

static gint
spi_grab_mask_compare_values (gconstpointer p1, gconstpointer p2)
{
  DEControllerGrabMask *l1 = (DEControllerGrabMask *) p1;
  DEControllerGrabMask *l2 = (DEControllerGrabMask *) p2;

  if (p1 == p2)
    {
      return 0;
    }
  else
    { 
      return ((l1->mod_mask != l2->mod_mask) || (l1->key_val != l2->key_val));
    }
}

static gboolean
spi_dec_poll_mouse_moved (gpointer data)
{
  SpiRegistry *registry = SPI_REGISTRY (data);
  CORBA_Environment ev;
  Accessibility_Event e;
  Window root_return, child_return;
  int win_x_return,win_y_return;
  int x, y;
  unsigned int mask_return;
  gchar event_name[24];
  Display *display = spi_get_display ();

  if (display != NULL)
	  XQueryPointer(display, DefaultRootWindow (display),
		&root_return, &child_return,
		&x, &y,
		&win_x_return, &win_y_return, &mask_return);

  if ((mask_return & mouse_button_mask) != mouse_button_state) {
	  int button_number = 0;
	  if (!(mask_return & Button1Mask) &&
	      (mouse_button_state & Button1Mask)) {
		  button_number = 1;
	  } else if (!(mask_return & Button2Mask) &&
		     (mouse_button_state & Button2Mask)) {
		  button_number = 2;
	  } else if (!(mask_return & Button3Mask) &&
		     (mouse_button_state & Button3Mask)) {
		  button_number = 3;
	  } else if (!(mask_return & Button4Mask) &&
		     (mouse_button_state & Button1Mask)) {
		  button_number = 4;
	  } else if (!(mask_return & Button5Mask) &&
		     (mouse_button_state & Button5Mask)) {
		  button_number = 5;
	  }
	  if (button_number) {
#ifdef SPI_DEBUG		  
		  fprintf (stderr, "Button %d Released\n",
			   button_number);
#endif
		  snprintf (event_name, 22, "mouse:button:%dr", button_number);
		  e.type = event_name;
		  e.source = BONOBO_OBJREF (registry->desktop);
		  e.detail1 = last_mouse_pos->x;
		  e.detail2 = last_mouse_pos->y;
		  CORBA_exception_init (&ev);
		  Accessibility_Registry_notifyEvent (BONOBO_OBJREF (registry),
						      &e,
						      &ev);
	  }
	  mouse_button_state = mask_return & mouse_button_mask;
  }
  if (last_mouse_pos == NULL) {
	  last_mouse_pos = g_new0 (GdkPoint, 1);
	  last_mouse_pos->x = 0;
	  last_mouse_pos->y = 0;
	  e.type = "mouse:abs";
  } else {
	  e.type = "mouse:rel";  
  }
  if (x != last_mouse_pos->x || y != last_mouse_pos->y) {
	  e.source = BONOBO_OBJREF (registry->desktop);
	  e.detail1 = x - last_mouse_pos->x;
	  e.detail2 = y - last_mouse_pos->y;
	  CORBA_exception_init (&ev);
	  last_mouse_pos->x = x;
	  last_mouse_pos->y = y;
	  Accessibility_Registry_notifyEvent (BONOBO_OBJREF (registry),
					      &e,
					      &ev);
	  return TRUE;
  }
  return FALSE;
}

static gboolean
spi_dec_poll_mouse_idle (gpointer data)
{
  if (! spi_dec_poll_mouse_moved (data)) 
    return TRUE;
  else
    {
      g_timeout_add (20, spi_dec_poll_mouse_moving, data);	    
      return FALSE;	    
    }
}

static gboolean
spi_dec_poll_mouse_moving (gpointer data)
{
  if (spi_dec_poll_mouse_moved (data))
    return TRUE;
  else
    {
      g_timeout_add (100, spi_dec_poll_mouse_idle, data);	    
      return FALSE;
    }
}

static int
spi_dec_ungrab_mouse (gpointer data)
{
	Display *display = spi_get_display ();
	fprintf (stderr, "mouse ungrab : display = %p\n", display);
	if (display)
	  {
	    XUngrabButton (spi_get_display (), AnyButton, AnyModifier,
			   XDefaultRootWindow (spi_get_display ()));
	    fprintf (stderr, "mouse grab released\n");
	  }
	return FALSE;
}

static void
spi_dec_init_mouse_listener (SpiRegistry *registry)
{
  Display *display = spi_get_display ();
  g_timeout_add (100, spi_dec_poll_mouse_idle, registry);

  if (display)
    {
      XGrabButton (display, AnyButton, 0,
		   gdk_x11_get_default_root_xwindow (),
		   True, ButtonPressMask | ButtonReleaseMask,
		   GrabModeSync, GrabModeAsync, None, None);
      XSync (display, False);
      fprintf (stderr, "mouse buttons grabbed\n");
    }
}

static DEControllerKeyListener *
spi_dec_key_listener_new (CORBA_Object                            l,
			  const Accessibility_KeySet             *keys,
			  const Accessibility_ControllerEventMask mask,
			  const Accessibility_KeyEventTypeSeq    *typeseq,
			  const Accessibility_EventListenerMode  *mode,
			  CORBA_Environment                      *ev)
{
  DEControllerKeyListener *key_listener = g_new0 (DEControllerKeyListener, 1);
  key_listener->listener.object = bonobo_object_dup_ref (l, ev);
  key_listener->listener.type = SPI_DEVICE_TYPE_KBD;
  key_listener->keys = ORBit_copy_value (keys, TC_Accessibility_KeySet);
  key_listener->mask = mask;
  key_listener->typeseq = ORBit_copy_value (typeseq, TC_Accessibility_KeyEventTypeSeq);
  if (mode)
    key_listener->mode = ORBit_copy_value (mode, TC_Accessibility_EventListenerMode);
  else
    key_listener->mode = NULL;

#ifdef SPI_DEBUG
  g_print ("new listener, with mask %x, is_global %d, keys %p (%d)\n",
	   (unsigned int) key_listener->mask,
           (int) (mode ? mode->global : 0),
	   (void *) key_listener->keys,
	   (int) (key_listener->keys ? key_listener->keys->_length : 0));
#endif

  return key_listener;	
}

static DEControllerKeyListener *
spi_key_listener_clone (DEControllerKeyListener *key_listener, CORBA_Environment *ev)
{
  DEControllerKeyListener *clone = g_new0 (DEControllerKeyListener, 1);
  clone->listener.object =
	  CORBA_Object_duplicate (key_listener->listener.object, ev);
  clone->listener.type = SPI_DEVICE_TYPE_KBD;
  clone->keys = ORBit_copy_value (key_listener->keys, TC_Accessibility_KeySet);
  clone->mask = key_listener->mask;
  clone->typeseq = ORBit_copy_value (key_listener->typeseq, TC_Accessibility_KeyEventTypeSeq);
  if (key_listener->mode)
    clone->mode = ORBit_copy_value (key_listener->mode, TC_Accessibility_EventListenerMode);
  else
    clone->mode = NULL;
  return clone;
}

static void
spi_key_listener_data_free (DEControllerKeyListener *key_listener, CORBA_Environment *ev)
{
  CORBA_free (key_listener->typeseq);
  CORBA_free (key_listener->keys);
  g_free (key_listener);
}

static void
spi_key_listener_clone_free (DEControllerKeyListener *clone, CORBA_Environment *ev)
{
  CORBA_Object_release (clone->listener.object, ev);
  spi_key_listener_data_free (clone, ev);
}

static void
spi_dec_key_listener_free (DEControllerKeyListener *key_listener,
			   CORBA_Environment       *ev)
{
  bonobo_object_release_unref (key_listener->listener.object, ev);
  spi_key_listener_data_free (key_listener, ev);
}

static void
_register_keygrab (SpiDEController      *controller,
		   DEControllerGrabMask *grab_mask)
{
  GList *l;

  l = g_list_find_custom (controller->keygrabs_list, grab_mask,
			  spi_grab_mask_compare_values);
  if (l)
    {
      DEControllerGrabMask *cur_mask = l->data;

      cur_mask->ref_count++;
      if (cur_mask->pending_remove)
        {
          cur_mask->pending_remove = FALSE;
	}
    }
  else
    {
      controller->keygrabs_list =
        g_list_prepend (controller->keygrabs_list,
			spi_grab_mask_clone (grab_mask));
    }
}

static void
_deregister_keygrab (SpiDEController      *controller,
		     DEControllerGrabMask *grab_mask)
{
  GList *l;

  l = g_list_find_custom (controller->keygrabs_list, grab_mask,
			  spi_grab_mask_compare_values);

  if (l)
    {
      DEControllerGrabMask *cur_mask = l->data;

      cur_mask->ref_count--;
      if (cur_mask->ref_count <= 0)
        {
          cur_mask->pending_remove = TRUE;
	}
    }
  else
    {
      g_warning ("De-registering non-existant grab");
    }
}

static void
handle_keygrab (SpiDEController         *controller,
		DEControllerKeyListener *key_listener,
		void                   (*process_cb) (SpiDEController *controller,
						      DEControllerGrabMask *grab_mask))
{
  DEControllerGrabMask grab_mask = { 0 };

  grab_mask.mod_mask = key_listener->mask;
  if (key_listener->keys->_length == 0) /* special case means AnyKey/AllKeys */
    {
      grab_mask.key_val = AnyKey;
#ifdef SPI_DEBUG
      fprintf (stderr, "AnyKey grab!");
#endif
      process_cb (controller, &grab_mask);
    }
  else
    {
      int i;

      for (i = 0; i < key_listener->keys->_length; ++i)
        {
	  Accessibility_KeyDefinition keydef = key_listener->keys->_buffer[i];	
	  long int key_val = keydef.keysym;
	  /* X Grabs require keycodes, not keysyms */
	  if (keydef.keystring && keydef.keystring[0])
	    {
	      key_val = XStringToKeysym(keydef.keystring);		    
	    }
	  if (key_val > 0)
	    {
	      key_val = XKeysymToKeycode (spi_get_display (), (KeySym) key_val);
	    }
	  else
	    {
	      key_val = keydef.keycode;
	    }
	  grab_mask.key_val = key_val;
	  process_cb (controller, &grab_mask);
	}
    }
}

static gboolean
spi_controller_register_global_keygrabs (SpiDEController         *controller,
					 DEControllerKeyListener *key_listener)
{
  handle_keygrab (controller, key_listener, _register_keygrab);
  return spi_controller_update_key_grabs (controller, NULL);
}

static void
spi_controller_deregister_global_keygrabs (SpiDEController         *controller,
					   DEControllerKeyListener *key_listener)
{
  handle_keygrab (controller, key_listener, _deregister_keygrab);
  spi_controller_update_key_grabs (controller, NULL);
}

static gboolean
spi_controller_register_device_listener (SpiDEController      *controller,
					 DEControllerListener *listener,
					 CORBA_Environment    *ev)
{
  DEControllerKeyListener *key_listener;
  
  switch (listener->type) {
  case SPI_DEVICE_TYPE_KBD:
      key_listener = (DEControllerKeyListener *) listener;

      controller->key_listeners = g_list_prepend (controller->key_listeners,
						  key_listener);
      if (key_listener->mode->global)
        {
	  return spi_controller_register_global_keygrabs (controller, key_listener);	
	}
      else
	      return TRUE;
      break;
    default:
      break;
  }
  return FALSE; 
}

static void
spi_device_event_controller_forward_mouse_event (SpiDEController *controller,
						 XEvent *xevent)
{
  Accessibility_Event e;
  CORBA_Environment ev;
  gchar event_name[24];
  int button = ((XButtonEvent *) xevent)->button;
  
  mouse_button_state = ((XButtonEvent *) xevent)->state;

  switch (button)
    {
    case 1:
	    mouse_button_state |= Button1Mask;
	    break;
    case 2:
	    mouse_button_state |= Button2Mask;
	    break;
    case 3:
	    mouse_button_state |= Button3Mask;
	    break;
    case 4:
	    mouse_button_state |= Button4Mask;
	    break;
    case 5:
	    mouse_button_state |= Button5Mask;
	    break;
    }
  last_mouse_pos->x = ((XButtonEvent *) xevent)->x_root;
  last_mouse_pos->y = ((XButtonEvent *) xevent)->y_root;

#ifdef SPI_DEBUG  
  fprintf (stderr, "mouse button %d %s (%x)\n",
	   ((XButtonEvent *) xevent)->button, 
	   (xevent->type == ButtonPress) ? "Press" : "Release",
	   mouse_button_state);
#endif
  snprintf (event_name, 22, "mouse:button:%d%c", button,
	    (xevent->type == ButtonPress) ? 'p' : 'r');

  e.type = CORBA_string_dup (event_name);
  e.source = BONOBO_OBJREF (controller->registry->desktop);
  e.detail1 = last_mouse_pos->x;
  e.detail2 = last_mouse_pos->y;
  CORBA_exception_init (&ev);
  Accessibility_Registry_notifyEvent (BONOBO_OBJREF (controller->registry),
				      &e,
				      &ev);
  
  XAllowEvents (spi_get_display (), ReplayPointer, CurrentTime);
}

static GdkFilterReturn
global_filter_fn (GdkXEvent *gdk_xevent, GdkEvent *event, gpointer data)
{
  XEvent *xevent = gdk_xevent;
  SpiDEController *controller;

  if (xevent->type == KeyPress || xevent->type == KeyRelease)
    {
      controller = SPI_DEVICE_EVENT_CONTROLLER (data);
      spi_device_event_controller_forward_key_event (controller, xevent);
      /* FIXME: is this right ? */
      return GDK_FILTER_CONTINUE;
    }
  if (xevent->type == ButtonPress || xevent->type == ButtonRelease)
    {
      controller = SPI_DEVICE_EVENT_CONTROLLER (data);
      spi_device_event_controller_forward_mouse_event (controller, xevent);
    }
  
  return GDK_FILTER_CONTINUE;
}

int
_spi_controller_device_error_handler (Display *display, XErrorEvent *error)
{
  if (error->error_code == BadAccess) 
    {  
      g_message ("Could not complete key grab: grab already in use.\n");
      spi_error_code = BadAccess;
      return 0;
    }
  else 
    {
      return (*x_default_error_handler) (display, error);
    }
}

static void
spi_controller_register_with_devices (SpiDEController *controller)
{
  /* calls to device-specific implementations and routines go here */
  /* register with: keyboard hardware code handler */
  /* register with: (translated) keystroke handler */

  gdk_window_add_filter (NULL, global_filter_fn, controller);

  gdk_window_set_events (gdk_get_default_root_window (),
			 GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);

  x_default_error_handler = XSetErrorHandler (_spi_controller_device_error_handler);
}

static gboolean
spi_key_set_contains_key (Accessibility_KeySet            *key_set,
			  const Accessibility_DeviceEvent *key_event)
{
  gint i;
  gint len;

  if (!key_set)
    {
      g_print ("null key set!");
      return TRUE;
    }

  len = key_set->_length;
  
  if (len == 0) /* special case, means "all keys/any key" */
    {
      g_print ("anykey\n");	    
      return TRUE;
    }

  for (i = 0; i < len; ++i)
    {
#ifdef SPI_KEYEVENT_DEBUG	    
      g_print ("key_set[%d] = %d; key_event %d, code %d, string %s\n",
	        i, (int) key_set->_buffer[i].keycode,
	       (int) key_event->id, (int) key_event->hw_code,
	       key_event->event_string); 
#endif
      if (key_set->_buffer[i].keysym == (CORBA_long) key_event->id)
        {
          return TRUE;
	}
      if (key_set->_buffer[i].keycode == (CORBA_long) key_event->hw_code)
        {
          return TRUE;
	}
      if (key_event->event_string && key_event->event_string[0] &&
	  !strcmp (key_set->_buffer[i].keystring, key_event->event_string))
        {
          return TRUE;
	}
    }
  
  return FALSE;
}

static gboolean
spi_key_eventtype_seq_contains_event (Accessibility_KeyEventTypeSeq   *type_seq,
				      const Accessibility_DeviceEvent *key_event)
{
  gint i;
  gint len;


  if (!type_seq)
    {
      g_print ("null type seq!");
      return TRUE;
    }

  len = type_seq->_length;
  
  if (len == 0) /* special case, means "all events/any event" */
    {
      return TRUE;
    }

  for (i = 0; i < len; ++i)
    {
#ifdef SPI_DEBUG	    
      g_print ("type_seq[%d] = %d; key event type = %d\n", i,
	       (int) type_seq->_buffer[i], (int) key_event->type);
#endif      
      if (type_seq->_buffer[i] == (CORBA_long) key_event->type)
        {
          return TRUE;
	}
    }
  
  return FALSE;
}

static gboolean
spi_key_event_matches_listener (const Accessibility_DeviceEvent *key_event,
				DEControllerKeyListener         *listener,
				CORBA_boolean                    is_system_global)
{
  if ((key_event->modifiers == (CORBA_unsigned_short) (listener->mask & 0xFFFF)) &&
       spi_key_set_contains_key (listener->keys, key_event) &&
       spi_key_eventtype_seq_contains_event (listener->typeseq, key_event) && 
      (is_system_global == listener->mode->global))
    {
      return TRUE;
    }
  else
    {
      return FALSE;
    }
}

static gboolean
spi_controller_notify_keylisteners (SpiDEController                 *controller,
				    const Accessibility_DeviceEvent *key_event,
				    CORBA_boolean                    is_system_global,
				    CORBA_Environment               *ev)
{
  GList   *l;
  GSList  *notify = NULL, *l2;
  GList  **key_listeners = &controller->key_listeners;
  gboolean is_consumed;

  if (!key_listeners)
    {
      return FALSE;
    }

  for (l = *key_listeners; l; l = l->next)
    {
       DEControllerKeyListener *key_listener = l->data;

       if (spi_key_event_matches_listener (key_event, key_listener, is_system_global))
         {
           Accessibility_DeviceEventListener ls = key_listener->listener.object;

	   if (ls != CORBA_OBJECT_NIL)
	     {
	       /* we clone (don't dup) the listener, to avoid refcount inc. */
	       notify = g_slist_prepend (notify,
					 spi_key_listener_clone (key_listener, ev));
	     }
         }
    }

#ifdef SPI_KEYEVENT_DEBUG
  if (!notify)
    {
      g_print ("no match for event\n");
    }
#endif

  is_consumed = FALSE;
  for (l2 = notify; l2 && !is_consumed; l2 = l2->next)
    {
      DEControllerKeyListener *key_listener = l2->data;	    
      Accessibility_DeviceEventListener ls = key_listener->listener.object;

      is_consumed = Accessibility_DeviceEventListener_notifyEvent (ls, key_event, ev);

      if (BONOBO_EX (ev))
        {
          is_consumed = FALSE;
	  spi_deregister_controller_key_listener (controller, key_listener,
						  ev);
	  CORBA_exception_free (ev);
        }

      CORBA_Object_release (ls, ev);
    }

  for (; l2; l2 = l2->next)
    {
      DEControllerKeyListener *key_listener = l2->data;	    
      spi_key_listener_clone_free (key_listener, ev);
      /* clone doesn't have its own ref, so don't use spi_key_listener_free */
    }

  g_slist_free (notify);

#ifdef SPI_DEBUG
  if (is_consumed) g_message ("consumed\n");
#endif
  return is_consumed;
}

static gboolean
spi_clear_error_state ()
{
	gboolean retval = spi_error_code != 0;
	spi_error_code = 0;
	return retval;
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
#ifdef SPI_KEYEVENT_DEBUG
	fprintf(stderr, "Tab\n");
#endif
        key_event.event_string = CORBA_string_dup ("Tab");
	break;
      case XK_BackSpace:
        key_event.event_string = CORBA_string_dup ("Backspace");
	break;
      case XK_Return:
        key_event.event_string = CORBA_string_dup ("Return");
	break;
      case XK_Home:
        key_event.event_string = CORBA_string_dup ("Home");
	break;
      case XK_Page_Down:
        key_event.event_string = CORBA_string_dup ("Page_Down");
	break;
      case XK_Page_Up:
        key_event.event_string = CORBA_string_dup ("Page_Up");
	break;
      case XK_F1:
        key_event.event_string = CORBA_string_dup ("F1");
	break;
      case XK_F2:
        key_event.event_string = CORBA_string_dup ("F2");
	break;
      case XK_F3:
        key_event.event_string = CORBA_string_dup ("F3");
	break;
      case XK_F4:
        key_event.event_string = CORBA_string_dup ("F4");
	break;
      case XK_F5:
        key_event.event_string = CORBA_string_dup ("F5");
	break;
      case XK_F6:
        key_event.event_string = CORBA_string_dup ("F6");
	break;
      case XK_F7:
        key_event.event_string = CORBA_string_dup ("F7");
	break;
      case XK_F8:
        key_event.event_string = CORBA_string_dup ("F8");
	break;
      case XK_F9:
        key_event.event_string = CORBA_string_dup ("F9");
	break;
      case XK_F10:
        key_event.event_string = CORBA_string_dup ("F10");
	break;
      case XK_F11:
        key_event.event_string = CORBA_string_dup ("F11");
	break;
      case XK_F12:
        key_event.event_string = CORBA_string_dup ("F12");
	break;
      case XK_End:
        key_event.event_string = CORBA_string_dup ("End");
	break;
      case XK_Escape:
        key_event.event_string = CORBA_string_dup ("Escape");
	break;
      case XK_Up:
        key_event.event_string = CORBA_string_dup ("Up");
	break;
      case XK_Down:
        key_event.event_string = CORBA_string_dup ("Down");
	break;
      case XK_Left:
        key_event.event_string = CORBA_string_dup ("Left");
	break;
      case XK_Right:
        key_event.event_string = CORBA_string_dup ("Right");
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
spi_controller_update_key_grabs (SpiDEController           *controller,
				 Accessibility_DeviceEvent *recv)
{
  GList *l, *next;
  gboolean   update_failed = FALSE;
  
  g_return_val_if_fail (controller != NULL, FALSE);

  /*
   * masks known to work with default RH 7.1+:
   * 0 (no mods), LockMask, Mod1Mask, Mod2Mask, ShiftMask,
   * ShiftMask|LockMask, Mod1Mask|LockMask, Mod2Mask|LockMask,
   * ShiftMask|Mod1Mask, ShiftMask|Mod2Mask, Mod1Mask|Mod2Mask,
   * ShiftMask|LockMask|Mod1Mask, ShiftMask|LockMask|Mod2Mask,
   *
   * ControlMask grabs are broken, must be in use already
   */
  for (l = controller->keygrabs_list; l; l = next)
    {
      gboolean do_remove;
      gboolean re_issue_grab;
      DEControllerGrabMask *grab_mask = l->data;

      next = l->next;

      re_issue_grab = recv &&
/*	      (recv->type == Accessibility_KEY_RELEASED) && - (?) */
	      (recv->modifiers & grab_mask->mod_mask) &&
	      (grab_mask->key_val == keycode_for_keysym (recv->id));

#ifdef SPI_DEBUG
      fprintf (stderr, "mask=%lx %lx (%c%c) %s\n",
	       (long int) grab_mask->key_val,
	       (long int) grab_mask->mod_mask,
	       grab_mask->pending_add ? '+' : '.',
	       grab_mask->pending_remove ? '-' : '.',
	       re_issue_grab ? "re-issue": "");
#endif

      do_remove = FALSE;

      if (grab_mask->pending_add && grab_mask->pending_remove)
        {
          do_remove = TRUE;
	}
      else if (grab_mask->pending_remove)
        {
#ifdef SPI_DEBUG
      fprintf (stderr, "ungrabbing, mask=%x\n", grab_mask->mod_mask);
#endif
	  XUngrabKey (spi_get_display (),
		      grab_mask->key_val,
		      grab_mask->mod_mask,
		      gdk_x11_get_default_root_xwindow ());

          do_remove = TRUE;
	}
      else if (grab_mask->pending_add || re_issue_grab)
        {

#ifdef SPI_DEBUG
	  fprintf (stderr, "grab %d with mask %x\n", grab_mask->key_val, grab_mask->mod_mask);
#endif
          XGrabKey (spi_get_display (),
		    grab_mask->key_val,
		    grab_mask->mod_mask,
		    gdk_x11_get_default_root_xwindow (),
		    True,
		    GrabModeSync,
		    GrabModeSync);
	  XSync (spi_get_display (), False);
	  update_failed = spi_clear_error_state ();
	  if (update_failed) {
		  while (grab_mask->ref_count > 0) --grab_mask->ref_count;
		  do_remove = TRUE;
	  }
	}

      grab_mask->pending_add = FALSE;
      grab_mask->pending_remove = FALSE;

      if (do_remove)
        {
          g_assert (grab_mask->ref_count <= 0);

	  controller->keygrabs_list = g_list_delete_link (
	    controller->keygrabs_list, l);

	  spi_grab_mask_free (grab_mask);
	}

    } 

  return ! update_failed;
}

/*
 * Implemented GObject::finalize
 */
static void
spi_device_event_controller_object_finalize (GObject *object)
{
  SpiDEController *controller;

  controller = SPI_DEVICE_EVENT_CONTROLLER (object);

#ifdef SPI_DEBUG
  fprintf(stderr, "spi_device_event_controller_object_finalize called\n");
#endif
  /* disconnect any special listeners, get rid of outstanding keygrabs */
  XUngrabKey (spi_get_display (), AnyKey, AnyModifier, DefaultRootWindow (spi_get_display ()));
	
  spi_device_event_controller_parent_class->finalize (object);
}

/*
 * CORBA Accessibility::DEController::registerKeystrokeListener
 *     method implementation
 */
static CORBA_boolean
impl_register_keystroke_listener (PortableServer_Servant                  servant,
				  const Accessibility_DeviceEventListener l,
				  const Accessibility_KeySet             *keys,
				  const Accessibility_ControllerEventMask mask,
				  const Accessibility_KeyEventTypeSeq    *type,
				  const Accessibility_EventListenerMode  *mode,
				  CORBA_Environment                      *ev)
{
  SpiDEController *controller = SPI_DEVICE_EVENT_CONTROLLER (
	  bonobo_object_from_servant (servant));
  DEControllerKeyListener *dec_listener;
#ifdef SPI_DEBUG
  fprintf (stderr, "registering keystroke listener %p with maskVal %lu\n",
	   (void *) l, (unsigned long) mask);
#endif
  dec_listener = spi_dec_key_listener_new (l, keys, mask, type, mode, ev);
  return spi_controller_register_device_listener (
	  controller, (DEControllerListener *) dec_listener, ev);
}


typedef struct {
	CORBA_Environment       *ev;
	DEControllerKeyListener *key_listener;
} RemoveKeyListenerClosure;

static SpiReEntrantContinue
remove_key_listener_cb (GList * const *list,
			gpointer       user_data)
{
  DEControllerKeyListener  *key_listener = (*list)->data;
  RemoveKeyListenerClosure *ctx = user_data;

  if (CORBA_Object_is_equivalent (ctx->key_listener->listener.object,
				  key_listener->listener.object, ctx->ev))
    {
      spi_re_entrant_list_delete_link (list);
      spi_dec_key_listener_free (key_listener, ctx->ev);
    }

  return SPI_RE_ENTRANT_CONTINUE;
}

static SpiReEntrantContinue
copy_key_listener_cb (GList * const *list,
		      gpointer       user_data)
{
  DEControllerKeyListener  *key_listener = (*list)->data;
  RemoveKeyListenerClosure *ctx = user_data;

  if (CORBA_Object_is_equivalent (ctx->key_listener->listener.object,
				  key_listener->listener.object, ctx->ev))
    {
      /* TODO: FIXME aggregate keys in case the listener is registered twice */
      CORBA_free (ctx->key_listener->keys);	    
      ctx->key_listener->keys = ORBit_copy_value (key_listener->keys, TC_Accessibility_KeySet);
    }

  return SPI_RE_ENTRANT_CONTINUE;
}


static void
spi_deregister_controller_key_listener (SpiDEController            *controller,
					DEControllerKeyListener    *key_listener,
					CORBA_Environment          *ev)
{
  RemoveKeyListenerClosure  ctx;

  ctx.ev = ev;
  ctx.key_listener = key_listener;

  /* special case, copy keyset from existing controller list entry */
  if (key_listener->keys->_length == 0) 
    {
      spi_re_entrant_list_foreach (&controller->key_listeners,
				  copy_key_listener_cb, &ctx);
    }
  
  spi_controller_deregister_global_keygrabs (controller, key_listener);

  spi_re_entrant_list_foreach (&controller->key_listeners,
				remove_key_listener_cb, &ctx);

}

/*
 * CORBA Accessibility::DEController::deregisterKeystrokeListener
 *     method implementation
 */
static void
impl_deregister_keystroke_listener (PortableServer_Servant                  servant,
				    const Accessibility_DeviceEventListener l,
				    const Accessibility_KeySet             *keys,
				    const Accessibility_ControllerEventMask mask,
				    const Accessibility_KeyEventTypeSeq    *type,
				    CORBA_Environment                      *ev)
{
  DEControllerKeyListener  *key_listener;
  SpiDEController *controller;

  controller = SPI_DEVICE_EVENT_CONTROLLER (bonobo_object (servant));

  key_listener = spi_dec_key_listener_new (l, keys, mask, type, NULL, ev);

#ifdef SPI_DEREGISTER_DEBUG
  fprintf (stderr, "deregistering keystroke listener %p with maskVal %lu\n",
	   (void *) l, (unsigned long) mask->value);
#endif

  spi_deregister_controller_key_listener (controller, key_listener, ev);

  spi_dec_key_listener_free (key_listener, ev);
}

/*
 * CORBA Accessibility::DEController::registerKeystrokeListener
 *     method implementation
 */
static void
impl_generate_keyboard_event (PortableServer_Servant           servant,
			      const CORBA_long                 keycode,
			      const CORBA_char                *keystring,
			      const Accessibility_KeySynthType synth_type,
			      CORBA_Environment               *ev)
{
  long key_synth_code;

#ifdef SPI_DEBUG
  fprintf (stderr, "synthesizing keystroke %ld, type %d\n",
	   (long) keycode, (int) synth_type);
#endif
  /* TODO: hide/wrap/remove X dependency */

  /* TODO: be accessX-savvy so that keyrelease occurs after sufficient timeout */
	
  /*
   * TODO: when initializing, query for XTest extension before using,
   * and fall back to XSendEvent() if XTest is not available.
   */
  
  /* TODO: implement keystring mode also */
  gdk_error_trap_push ();

  switch (synth_type)
    {
      case Accessibility_KEY_PRESS:
        XTestFakeKeyEvent (spi_get_display (), (unsigned int) keycode, True, CurrentTime);
	break;
      case Accessibility_KEY_PRESSRELEASE:
	XTestFakeKeyEvent (spi_get_display (), (unsigned int) keycode, True, CurrentTime);
      case Accessibility_KEY_RELEASE:
	XTestFakeKeyEvent (spi_get_display (), (unsigned int) keycode, False, CurrentTime);
	break;
      case Accessibility_KEY_SYM:
	key_synth_code = keycode_for_keysym (keycode);
	XTestFakeKeyEvent (spi_get_display (), (unsigned int) key_synth_code, True, CurrentTime);
	XTestFakeKeyEvent (spi_get_display (), (unsigned int) key_synth_code, False, CurrentTime);
	break;
      case Accessibility_KEY_STRING:
	fprintf (stderr, "Not yet implemented\n");
	break;
    }
  if (gdk_error_trap_pop ())
    {
      g_warning ("Error emitting keystroke");
    }
}

/* Accessibility::DEController::generateMouseEvent */
static void
impl_generate_mouse_event (PortableServer_Servant servant,
			   const CORBA_long       x,
			   const CORBA_long       y,
			   const CORBA_char      *eventName,
			   CORBA_Environment     *ev)
{
  int button;
  gboolean error = FALSE;
  Display *display = spi_get_display ();
#ifdef SPI_DEBUG
  fprintf (stderr, "generating mouse %s event at %ld, %ld\n",
	   eventName, (long int) x, (long int) y);
#endif
  g_message ("mouse event synthesis\n");
  switch (eventName[0])
    {
      case 'b':
        switch (eventName[1])
	  {
	  /* TODO: check number of buttons before parsing */
	  case '1':
		    button = 1;
		    break;
	  case '2':
		  button = 2;
		  break;
	  case '3':
	          button = 3;
	          break;
	  default:
		  error = TRUE;
	  }
	if (!error)
	  {
	    if (x != -1 && y != -1)
	      {
	        XTestFakeMotionEvent (display, DefaultScreen (display),
				      x, y, 0);
	      }
	    XTestFakeButtonEvent (display, button, !(eventName[2] == 'r'), 0);
	    if (eventName[2] == 'c')
	      XTestFakeButtonEvent (display, button, FALSE, 1);
	    else if (eventName[2] == 'd')
	      {
	      XTestFakeButtonEvent (display, button, FALSE, 1);
	      XTestFakeButtonEvent (display, button, TRUE, 2);
	      XTestFakeButtonEvent (display, button, FALSE, 3);
	      }
	  }
	break;
      case 'r': /* relative motion */ 
	XTestFakeRelativeMotionEvent (display, x, y, 0);
        break;
      case 'a': /* absolute motion */
	XTestFakeMotionEvent (display, DefaultScreen (display),
			      x, y, 0);
        break;
    }
}

/* Accessibility::DEController::notifyListenersSync */
static CORBA_boolean
impl_notify_listeners_sync (PortableServer_Servant           servant,
			    const Accessibility_DeviceEvent *event,
			    CORBA_Environment               *ev)
{
  SpiDEController *controller = SPI_DEVICE_EVENT_CONTROLLER (
    bonobo_object_from_servant (servant));
#ifdef SPI_DEBUG
  g_print ("notifylistening listeners synchronously: controller %p, event id %d\n",
	   controller, (int) event->id);
#endif
  return spi_controller_notify_keylisteners (controller, event, CORBA_FALSE, ev) ?
	  CORBA_TRUE : CORBA_FALSE; 
}

/* Accessibility::DEController::notifyListenersAsync */
static void
impl_notify_listeners_async (PortableServer_Servant           servant,
			     const Accessibility_DeviceEvent *event,
			     CORBA_Environment               *ev)
{
  SpiDEController *controller = SPI_DEVICE_EVENT_CONTROLLER (
    bonobo_object_from_servant (servant));
#ifdef SPI_DEBUG
  fprintf (stderr, "notifying listeners asynchronously\n");
#endif
  spi_controller_notify_keylisteners (controller, event, CORBA_FALSE, ev); 
}

static void
spi_device_event_controller_class_init (SpiDEControllerClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_DeviceEventController__epv *epv = &klass->epv;

  spi_device_event_controller_parent_class = g_type_class_peek_parent (klass);
  
  object_class->finalize = spi_device_event_controller_object_finalize;
	
  epv->registerKeystrokeListener   = impl_register_keystroke_listener;
  epv->deregisterKeystrokeListener = impl_deregister_keystroke_listener;
  epv->generateKeyboardEvent       = impl_generate_keyboard_event;
  epv->generateMouseEvent          = impl_generate_mouse_event;
  epv->notifyListenersSync         = impl_notify_listeners_sync;
  epv->notifyListenersAsync        = impl_notify_listeners_async;
}

static void
spi_device_event_controller_init (SpiDEController *device_event_controller)
{
  device_event_controller->key_listeners   = NULL;
  device_event_controller->mouse_listeners = NULL;
  device_event_controller->keygrabs_list   = NULL;

  /*
   * TODO: fixme, this module makes the foolish assumptions that
   * registryd uses the same display as the apps, and that the
   * DISPLAY environment variable is set.
   */
  gdk_init (NULL, NULL);
  
  spi_controller_register_with_devices (device_event_controller);
}

static void
spi_device_event_controller_forward_key_event (SpiDEController *controller,
					       const XEvent    *event)
{
  gboolean is_consumed = FALSE;
  CORBA_Environment ev;
  Accessibility_DeviceEvent key_event;

  g_assert (event->type == KeyPress || event->type == KeyRelease);

  CORBA_exception_init (&ev);

  key_event = spi_keystroke_from_x_key_event ((XKeyEvent *) event);

  spi_controller_update_key_grabs (controller, &key_event);

  /* relay to listeners, and decide whether to consume it or not */
  is_consumed = spi_controller_notify_keylisteners (
	  controller, &key_event, CORBA_TRUE, &ev);

  CORBA_exception_free (&ev);

  if (is_consumed)
    {
      XAllowEvents (spi_get_display (), AsyncKeyboard, CurrentTime);
    }
  else
    {
      XAllowEvents (spi_get_display (), ReplayKeyboard, CurrentTime);
    }
}

SpiDEController *
spi_device_event_controller_new (SpiRegistry *registry)
{
  SpiDEController *retval = g_object_new (
    SPI_DEVICE_EVENT_CONTROLLER_TYPE, NULL);

  retval->registry = SPI_REGISTRY (bonobo_object_ref (
	  BONOBO_OBJECT (registry)));

  spi_dec_init_mouse_listener (registry);
  /* TODO: kill mouse listener on finalize */
  
  return retval;
}

BONOBO_TYPE_FUNC_FULL (SpiDEController,
		       Accessibility_DeviceEventController,
		       PARENT_TYPE,
		       spi_device_event_controller);
