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

/* accessibleeventlistener.c: implementation of SpiListener.idl */

#include <config.h>
#ifdef SPI_DEBUG
#include <stdio.h>
#endif
#include <libspi/eventlistener.h>

/* Our parent Gtk object type */
#define PARENT_TYPE SPI_LISTENER_TYPE

enum {
	EVENT,
	LAST_SIGNAL
};
static guint signals [LAST_SIGNAL];

/*
 * CORBA Accessibility::Listener::notifyEvent method implementation
 */
static void
impl_accessible_event_notify_event (PortableServer_Servant     servant,
                                    const Accessibility_Event *e,
                                    CORBA_Environment         *ev)
{
  SpiEventListener *listener = SPI_EVENT_LISTENER (
	  bonobo_object_from_servant (servant));

  g_signal_emit (G_OBJECT (listener), signals [EVENT], 0, e);

}

static void
spi_event_listener_class_init (SpiEventListenerClass *klass)
{
        SpiListenerClass *spi_listener_class = (SpiListenerClass *) klass;
        POA_Accessibility_EventListener__epv *epv = &spi_listener_class->epv;

	signals [EVENT] = g_signal_new (
		"event",
		G_TYPE_FROM_CLASS (klass),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (SpiEventListenerClass, event),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER,
		G_TYPE_NONE, 1, G_TYPE_POINTER);

        epv->notifyEvent = impl_accessible_event_notify_event;
}

static void
spi_event_listener_init (SpiEventListener *listener)
{
}

BONOBO_TYPE_FUNC (SpiEventListener,
		  PARENT_TYPE,
		  spi_event_listener)

SpiEventListener *
spi_event_listener_new (void)
{
    SpiEventListener *retval = g_object_new (
	    SPI_EVENT_LISTENER_TYPE, NULL);
    return retval;
}
