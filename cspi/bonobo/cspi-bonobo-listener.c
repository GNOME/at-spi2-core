/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2002 Ximian Inc.
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

#include <libbonobo.h>
#include "../spi-private.h"
#include "cspi-bonobo-listener.h"

typedef struct
{
  union
    {
      AccessibleEventListenerCB     event;
      AccessibleKeystrokeListenerCB key_event;
      gpointer                      method;
    } cb;
  gpointer user_data;
} EventHandler;

GObjectClass *event_parent_class;
GObjectClass *keystroke_parent_class;

/*
 * Misc. helpers.
 */

static EventHandler *
cspi_event_handler_new (gpointer method, gpointer user_data)
{
  EventHandler *eh = g_new0 (EventHandler, 1);

  eh->cb.method = method;
  eh->user_data = user_data;

  return eh;
}

static void
cspi_event_handler_free (EventHandler *handler)
{
  g_free (handler);
}

static GList *
cspi_event_list_remove_by_cb (GList *list, gpointer callback)
{
  GList *l, *next;
	
  for (l = list; l; l = next)
    {
      EventHandler *eh = l->data;
      next = l->next;
      
      list = g_list_delete_link (list, l);
      
      cspi_event_handler_free (eh);
    }

  return list;
}

/*
 * Standard event dispatcher
 */

BONOBO_CLASS_BOILERPLATE (CSpiEventListener, cspi_event_listener,
			  GObject, spi_event_listener_get_type ())

static void
cspi_event (SpiEventListener    *listener,
	    Accessibility_Event *event)
{
  GList *l;
  CSpiEventListener *clistener = (CSpiEventListener *) listener;
  AccessibleEvent    aevent;
  Accessible        *source = cspi_object_borrow (event->source);
  
  aevent.type    = event->type;
  aevent.source  = source;
  aevent.detail1 = event->detail1;
  aevent.detail2 = event->detail2;

  /* FIXME: re-enterancy hazard on this list */
  for (l = clistener->callbacks; l; l = l->next)
    {
      EventHandler *eh = l->data;

      eh->cb.event (&aevent, eh->user_data);
    }

  cspi_object_return (source);
}

static void
cspi_event_listener_instance_init (CSpiEventListener *listener)
{
}

static void
cspi_event_listener_finalize (GObject *object)
{
  CSpiEventListener *listener = (CSpiEventListener *) object;
  GList *l;
  
  for (l = listener->callbacks; l; l = l->next)
    {
      cspi_event_handler_free (l->data);
    }
  
  g_list_free (listener->callbacks);

  event_parent_class->finalize (object);
}

static void
cspi_event_listener_class_init (CSpiEventListenerClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;

  event_parent_class = g_type_class_peek_parent (klass);
  object_class->finalize = cspi_event_listener_finalize;

  klass->event = cspi_event;
}

gpointer
cspi_event_listener_new (void)
{
  CSpiEventListener *listener;

  listener = g_object_new (cspi_event_listener_get_type (), NULL);

  return listener;
}

void
cspi_event_listener_add_cb (AccessibleEventListener  *al,
			    AccessibleEventListenerCB callback,
			    void                     *user_data)
{
  CSpiEventListener *listener = al;

  g_return_if_fail (CSPI_IS_EVENT_LISTENER (listener));

  listener->callbacks = g_list_prepend (listener->callbacks,
					cspi_event_handler_new (callback, user_data));
}

void
cspi_event_listener_remove_cb (AccessibleEventListener  *al,
			       AccessibleEventListenerCB callback)
{
  CSpiEventListener *listener = al;

  g_return_if_fail (CSPI_IS_EVENT_LISTENER (listener));

  listener->callbacks = cspi_event_list_remove_by_cb (listener->callbacks, callback);
}

/*
 * Key event dispatcher
 */
static gboolean
cspi_key_event (SpiKeystrokeListener            *listener,
		const Accessibility_DeviceEvent *keystroke)
{
  GList *l;
  CSpiKeystrokeListener *clistener = (CSpiKeystrokeListener *) listener;
  AccessibleKeystroke akeystroke;
  gboolean handled = FALSE;

#ifdef SPI_KEYEVENT_DEBUG
  fprintf (stderr, "%s%c",
	   (keystroke->modifiers & SPI_KEYMASK_ALT)?"Alt-":"",
	   ((keystroke->modifiers & SPI_KEYMASK_SHIFT)^(keystroke->modifiers & SPI_KEYMASK_SHIFTLOCK))?
	   (char) toupper((int) keystroke->keyID) : (char) tolower((int) keystroke->keyID));
  
  fprintf (stderr, "Key:\tsym %ld\n\tmods %x\n\tcode %d\n\ttime %ld\n",
	   (long) keystroke->keyID,
	   (unsigned int) keystroke->modifiers,
	   (int) keystroke->keycode,
	   (long int) keystroke->timestamp);
#endif

  switch (keystroke->type)
    {
      case Accessibility_KEY_PRESSED:
	akeystroke.type = SPI_KEY_PRESSED;
	break;
      case Accessibility_KEY_RELEASED:
	akeystroke.type = SPI_KEY_RELEASED;
	break;
      default:
	akeystroke.type = 0;
	break;
    }
  akeystroke.keyID     = keystroke->id;
  akeystroke.keycode   = keystroke->hw_code;
  akeystroke.timestamp = keystroke->timestamp;
  akeystroke.keystring = g_strdup (keystroke->event_string);
  akeystroke.modifiers = keystroke->modifiers;

  /* FIXME: re-enterancy hazard on this list */
  for (l = clistener->callbacks; l; l = l->next)
    {
      EventHandler *eh = l->data;

      if ((handled = eh->cb.key_event (&akeystroke, eh->user_data)))
        {
	  break;
	}
    }
  
  return handled;
}

static void
cspi_keystroke_listener_init (CSpiKeystrokeListener *listener)
{
}


static void
cspi_keystroke_listener_finalize (GObject *object)
{
  CSpiKeystrokeListener *listener = (CSpiKeystrokeListener *) object;
  GList *l;
  
  for (l = listener->callbacks; l; l = l->next)
    {
      cspi_event_handler_free (l->data);
    }
  
  g_list_free (listener->callbacks);

  keystroke_parent_class->finalize (object);
}

static void
cspi_keystroke_listener_class_init (CSpiKeystrokeListenerClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;

  keystroke_parent_class = g_type_class_peek_parent (klass);
  object_class->finalize = cspi_keystroke_listener_finalize;

  klass->key_event = cspi_key_event;
}

BONOBO_TYPE_FUNC (CSpiKeystrokeListener, 
		  spi_keystroke_listener_get_type (),
		  cspi_keystroke_listener);

gpointer
cspi_keystroke_listener_new (void)
{
  CSpiEventListener *listener;

  listener = g_object_new (cspi_keystroke_listener_get_type (), NULL);

  return listener;
}

void
cspi_keystroke_listener_add_cb (AccessibleKeystrokeListener  *al,
				AccessibleKeystrokeListenerCB callback,
				void                         *user_data)
{
  CSpiKeystrokeListener *listener = al;

  g_return_if_fail (CSPI_IS_KEYSTROKE_LISTENER (listener));

  listener->callbacks = g_list_prepend (listener->callbacks,
					cspi_event_handler_new (callback, user_data));
}

void
cspi_keystroke_listener_remove_cb (AccessibleKeystrokeListener  *al,
				   AccessibleKeystrokeListenerCB callback)
{
  CSpiKeystrokeListener *listener = al;

  g_return_if_fail (CSPI_IS_KEYSTROKE_LISTENER (listener));

  listener->callbacks = cspi_event_list_remove_by_cb (listener->callbacks, callback);
}

void
cspi_event_listener_unref (AccessibleEventListener *listener)
{
  bonobo_object_unref (BONOBO_OBJECT (listener));
}

void
cspi_keystroke_listener_unref (AccessibleKeystrokeListener *listener)
{
  bonobo_object_unref (BONOBO_OBJECT (listener));
}


CORBA_Object
cspi_event_listener_get_corba (AccessibleEventListener *listener)
{
  return BONOBO_OBJREF (listener);
}

CORBA_Object
cspi_keystroke_listener_get_corba (AccessibleKeystrokeListener *listener)
{
  return BONOBO_OBJREF (listener);
}

