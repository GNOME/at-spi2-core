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
static void     spi_controller_register_device_listener       (SpiDEController           *controller,
							       DEControllerListener      *l,
							       CORBA_Environment         *ev);
static void     spi_device_event_controller_forward_key_event (SpiDEController           *controller,
							       const XEvent              *event);

/* Private methods */

static KeyCode
keycode_for_keysym (long keysym)
{
  return XKeysymToKeycode (GDK_DISPLAY (), (KeySym) keysym);
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

static void
spi_dec_key_listener_free (DEControllerKeyListener *key_listener,
			   CORBA_Environment       *ev)
{
  bonobo_object_release_unref (key_listener->listener.object, ev);
  CORBA_free (key_listener->typeseq);
  CORBA_free (key_listener->keys);
  g_free (key_listener);
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
      cur_mask->pending_remove = TRUE;
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
      process_cb (controller, &grab_mask);
    }
  else
    {
      int i;

      for (i = 0; i < key_listener->keys->_length; ++i)
        {
	  long int key_val = key_listener->keys->_buffer[i];
	  /* X Grabs require keycodes, not keysyms */
	  if (key_val >= 0)
	    {
	      key_val = XKeysymToKeycode (GDK_DISPLAY (), (KeySym) key_val);
	    }
	  grab_mask.key_val = key_val;

	  process_cb (controller, &grab_mask);
	}
    }
}

static void
spi_controller_register_global_keygrabs (SpiDEController         *controller,
					 DEControllerKeyListener *key_listener)
{
  handle_keygrab (controller, key_listener, _register_keygrab);
  spi_controller_update_key_grabs (controller, NULL);
}

static void
spi_controller_deregister_global_keygrabs (SpiDEController         *controller,
					   DEControllerKeyListener *key_listener)
{
  handle_keygrab (controller, key_listener, _deregister_keygrab);
  spi_controller_update_key_grabs (controller, NULL);
}

static void
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
	  spi_controller_register_global_keygrabs (controller, key_listener);	
	}
      break;
    default:
      break;
  }
}

static GdkFilterReturn
global_filter_fn (GdkXEvent *gdk_xevent, GdkEvent *event, gpointer data)
{
  XEvent *xevent = gdk_xevent;
  SpiDEController *controller;

  if (xevent->type != KeyPress && xevent->type != KeyRelease)
    {
      return GDK_FILTER_CONTINUE;
    }

  controller = SPI_DEVICE_EVENT_CONTROLLER (data);

  spi_device_event_controller_forward_key_event (controller, xevent);

  /* FIXME: is this right ? */
  return GDK_FILTER_CONTINUE;
}

int
_spi_controller_device_error_handler (Display *display, XErrorEvent *error)
{
  if (error->error_code == BadAccess) 
    {  
      g_message ("Could not complete key grab: grab already in use.\n");
    }
  else 
    {
      (*x_default_error_handler) (display, error);
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

  XSelectInput (GDK_DISPLAY (),
		DefaultRootWindow (GDK_DISPLAY ()),
		KeyPressMask | KeyReleaseMask);
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
      g_print ("key_set[%d] = %d; key_event %d, code %d\n",
	        i, (int) key_set->_buffer[i],
	       (int) key_event->id, (int) key_event->hw_code); 
#endif
      if (key_set->_buffer[i] == (CORBA_long) key_event->id)
        {
          return TRUE;
	}
      if (key_set->_buffer[i] == (CORBA_long) -key_event->hw_code)
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
spi_notify_keylisteners (GList                          **key_listeners,
			 const Accessibility_DeviceEvent *key_event,
			 CORBA_boolean                    is_system_global,
			 CORBA_Environment               *ev)
{
  GList   *l;
  GSList  *notify = NULL, *l2;
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
               notify = g_slist_prepend (notify, CORBA_Object_duplicate (ls, ev));
	     }
         }
    }

#ifdef SPI_KEYEVENT_DEBUG
  if (!notify)
    {
      g_print ("no match for listener %d\n", i);
    }
#endif

  is_consumed = FALSE;
  for (l2 = notify; l2 && !is_consumed; l2 = l2->next)
    {
      Accessibility_DeviceEventListener ls = l2->data;

      is_consumed = Accessibility_DeviceEventListener_notifyEvent (ls, key_event, ev);

      if (BONOBO_EX (ev))
        {
          is_consumed = FALSE;
	  CORBA_exception_free (ev);
        }

      CORBA_Object_release (ls, ev);
    }

  for (; l2; l2 = l2->next)
    {
      CORBA_Object_release (l2->data, ev);
    }

  g_slist_free (notify);
  
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
spi_controller_update_key_grabs (SpiDEController           *controller,
				 Accessibility_DeviceEvent *recv)
{
  GList *l, *next;

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
	  XUngrabKey (GDK_DISPLAY (),
		      grab_mask->key_val,
		      grab_mask->mod_mask,
		      gdk_x11_get_default_root_xwindow ());

          do_remove = TRUE;
	}
      else if (grab_mask->pending_add || re_issue_grab)
        {

#ifdef SPI_DEBUG
	  fprintf (stderr, "grab with mask %x\n", grab_mask->mod_mask);
#endif
          XGrabKey (GDK_DISPLAY (),
		    grab_mask->key_val,
		    grab_mask->mod_mask,
		    gdk_x11_get_default_root_xwindow (),
		    True,
		    GrabModeAsync,
		    GrabModeAsync);
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

      /* TODO: check calls for errors and return FALSE if error occurs */
    } 

  return TRUE;
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
  XUngrabKey (GDK_DISPLAY (), AnyKey, AnyModifier, DefaultRootWindow (GDK_DISPLAY ()));
	
  spi_device_event_controller_parent_class->finalize (object);
}

/*
 * CORBA Accessibility::DEController::registerKeystrokeListener
 *     method implementation
 */
static void
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
  spi_controller_register_device_listener (
    controller, (DEControllerListener *) dec_listener, ev);
}


typedef struct {
	CORBA_Environment       *ev;
	DEControllerKeyListener *key_listener;
} RemoveKeyListenerClosure;

static SpiReEnterantContinue
remove_key_listener_cb (GList * const *list,
			gpointer       user_data)
{
  DEControllerKeyListener  *key_listener = (*list)->data;
  RemoveKeyListenerClosure *ctx = user_data;

  if (CORBA_Object_is_equivalent (ctx->key_listener->listener.object,
				  key_listener->listener.object, ctx->ev))
    {
      spi_re_enterant_list_delete_link (list);
      spi_dec_key_listener_free (key_listener, ctx->ev);
    }

  return SPI_RE_ENTERANT_CONTINUE;
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
  RemoveKeyListenerClosure  ctx;
  SpiDEController *controller;

  controller = SPI_DEVICE_EVENT_CONTROLLER (bonobo_object (servant));

  key_listener = spi_dec_key_listener_new (l, keys, mask, type, NULL, ev);

#ifdef SPI_DEREGISTER_DEBUG
  fprintf (stderr, "deregistering keystroke listener %p with maskVal %lu\n",
	   (void *) l, (unsigned long) mask->value);
#endif

  spi_controller_deregister_global_keygrabs (controller, key_listener);

  ctx.ev = ev;
  ctx.key_listener = key_listener;

  spi_re_enterant_list_foreach (&controller->key_listeners,
				remove_key_listener_cb, &ctx);

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
        XTestFakeKeyEvent (GDK_DISPLAY (), (unsigned int) keycode, True, CurrentTime);
	break;
      case Accessibility_KEY_PRESSRELEASE:
	XTestFakeKeyEvent (GDK_DISPLAY (), (unsigned int) keycode, True, CurrentTime);
      case Accessibility_KEY_RELEASE:
	XTestFakeKeyEvent (GDK_DISPLAY (), (unsigned int) keycode, False, CurrentTime);
	break;
      case Accessibility_KEY_SYM:
	key_synth_code = keycode_for_keysym (keycode);
	XTestFakeKeyEvent (GDK_DISPLAY (), (unsigned int) key_synth_code, True, CurrentTime);
	XTestFakeKeyEvent (GDK_DISPLAY (), (unsigned int) key_synth_code, False, CurrentTime);
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
#ifdef SPI_DEBUG
  fprintf (stderr, "generating mouse %s event at %ld, %ld\n",
	   eventName, (long int) x, (long int) y);
#endif
  g_warning ("not yet implemented");
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
  return spi_notify_keylisteners (&controller->key_listeners, event, CORBA_FALSE, ev) ?
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
  spi_notify_keylisteners (&controller->key_listeners, event, CORBA_FALSE, ev); 
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
  /* relay to listeners, and decide whether to consume it or not */
  is_consumed = spi_notify_keylisteners (
    &controller->key_listeners, &key_event, CORBA_TRUE, &ev);

  CORBA_exception_free (&ev);

  if (is_consumed)
    {
      XAllowEvents (GDK_DISPLAY (), AsyncKeyboard, CurrentTime);
    }
  else
    {
      XAllowEvents (GDK_DISPLAY (), ReplayKeyboard, CurrentTime);
    }

  spi_controller_update_key_grabs (controller, &key_event);
}

SpiDEController *
spi_device_event_controller_new (SpiRegistry *registry)
{
  SpiDEController *retval = g_object_new (
    SPI_DEVICE_EVENT_CONTROLLER_TYPE, NULL);

  retval->registry = SPI_REGISTRY (bonobo_object_ref (
	  BONOBO_OBJECT (registry)));

  return retval;
}

BONOBO_TYPE_FUNC_FULL (SpiDEController,
		       Accessibility_DeviceEventController,
		       PARENT_TYPE,
		       spi_device_event_controller);
