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

/* accessibleeventlistener.c: implementation of SpiListener.idl */

#include <config.h>
#ifdef SPI_DEBUG
#include <stdio.h>
#endif
#include <libspi/accessibleeventlistener.h>

/* Our parent Gtk object type */
#define PARENT_TYPE SPI_LISTENER_TYPE

/* A pointer to our parent object class */
static SpiListenerClass *spi_event_listener_parent_class;

enum {
	EVENT,
	LAST_SIGNAL
};
static guint signals [LAST_SIGNAL];

/*
 * Implemented GObject::finalize
 */
static void
spi_event_listener_object_finalize (GObject *object)
{
	SpiEventListener *listener = SPI_ACCESSIBLE_EVENT_SPI_LISTENER (object);
#ifdef SPI_DEBUG
        fprintf(stderr, "spi_listener_object_finalize called\n");
#endif
	g_list_free (listener->callbacks);

        ((GObjectClass *) spi_event_listener_parent_class)->finalize (object);
}

/*
 * CORBA Accessibility::Listener::notifyEvent method implementation
 */

static void
impl_accessible_event_notify_event (PortableServer_Servant     servant,
                                    const Accessibility_Event *e,
                                    CORBA_Environment         *ev)
{
  GList *l;
  VoidSpiEventListenerCB cb;
  SpiEventListener *listener = SPI_ACCESSIBLE_EVENT_SPI_LISTENER (
                                       bonobo_object_from_servant (servant));

  for (l = listener->callbacks; l; l = l->next)
    {
      cb = (VoidSpiEventListenerCB) l->data;
      if (cb)
        {
          (*cb) (e);
        }
    }

  g_signal_emit (G_OBJECT (listener), signals [EVENT], 0, e); 

  if (e->source != CORBA_OBJECT_NIL)
    {
      Accessibility_Accessible_unref (e->source, ev);
    }
}

static void
spi_event_listener_class_init (SpiEventListenerClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        SpiListenerClass * spi_listener_class = (SpiListenerClass *) klass;
        POA_Accessibility_EventListener__epv *epv = &spi_listener_class->epv;
        spi_event_listener_parent_class = g_type_class_ref (SPI_LISTENER_TYPE);

	signals [EVENT] = g_signal_new (
		"event",
		G_TYPE_FROM_CLASS (klass),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (SpiEventListenerClass, event),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER,
		G_TYPE_NONE, 1, G_TYPE_POINTER);

        object_class->finalize = spi_event_listener_object_finalize;

        epv->notifyEvent = impl_accessible_event_notify_event;
}

static void
spi_event_listener_init (SpiEventListener *listener)
{
        listener->callbacks = NULL;
}

BONOBO_TYPE_FUNC (SpiEventListener,
		  PARENT_TYPE,
		  spi_event_listener);

SpiEventListener *
spi_event_listener_new ()
{
    SpiEventListener *retval = g_object_new (
	    SPI_ACCESSIBLE_EVENT_SPI_LISTENER_TYPE, NULL);
    return retval;
}

void
spi_event_listener_add_callback (SpiEventListener *listener,
				 VoidSpiEventListenerCB callback)
{
  listener->callbacks = g_list_prepend (listener->callbacks, callback);
}

void
spi_event_listener_remove_callback (SpiEventListener *listener,
				    VoidSpiEventListenerCB callback)
{
  listener->callbacks = g_list_remove (listener->callbacks, callback);
}
