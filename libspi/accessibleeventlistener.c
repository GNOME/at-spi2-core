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
 * accessibleeventlistener.c: bonobo implementation of SpiListener.idl,
 *   with added ability to attach/remove in-process callbacks.
 *
 */

#ifdef SPI_DEBUG
#include <stdio.h>
#endif

#include <config.h>
#include <bonobo/Bonobo.h>
#include <libspi/Accessibility.h>

/*
 * This pulls the definition for the BonoboObject (GType)
 */
#include "accessibleeventlistener.h"

/*
 * Our parent Gtk object type
 */
#define PARENT_TYPE SPI_LISTENER_TYPE

/*
 * A pointer to our parent object class
 */
static SpiListenerClass *spi_accessible_event_spi_listener_parent_class;

/*
 * Implemented GObject::finalize
 */
static void
spi_accessible_event_spi_listener_object_finalize (GObject *object)
{
#ifdef SPI_DEBUG
        fprintf(stderr, "spi_listener_object_finalize called\n");
#endif
        ((GObjectClass *) spi_accessible_event_spi_listener_parent_class)->finalize (object);
}

/*
 * CORBA Accessibility::SpiListener::notifyEvent method implementation
 */

static void
impl_accessible_event_notify_event (PortableServer_Servant     servant,
                                    const Accessibility_Event *e,
                                    CORBA_Environment         *ev)
{
  int n;
  int len;
  VoidEventListenerCB cb;
  SpiAccessibleEventListener *listener = SPI_ACCESSIBLE_EVENT_SPI_LISTENER (
                                       bonobo_object_from_servant (servant));
  len = g_list_length (listener->callbacks);

  for (n=0; n<len; ++n)
    {
      cb =  (VoidEventListenerCB) g_list_nth_data (listener->callbacks, n);
      if (cb)
        {
          (*cb) (e);
        }
    }
  /* Accessibility_SpiAccessible_unref (e->source, ev); */
}

static void
spi_accessible_event_spi_listener_class_init (SpiAccessibleEventListenerClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        SpiListenerClass * spi_listener_class = (SpiListenerClass *) klass;
        POA_Accessibility_EventListener__epv *epv = &spi_listener_class->epv;
        spi_accessible_event_spi_listener_parent_class = g_type_class_ref (SPI_LISTENER_TYPE);

        object_class->finalize = spi_accessible_event_spi_listener_object_finalize;

        epv->notifyEvent = impl_accessible_event_notify_event;
}

static void
spi_accessible_event_spi_listener_init (SpiAccessibleEventListener *listener)
{
        listener->callbacks = NULL;
}

GType
spi_accessible_event_spi_listener_get_type (void)
{
        static GType type = 0;

        if (!type) {
                static const GTypeInfo tinfo = {
                        sizeof (SpiAccessibleEventListenerClass),
                        (GBaseInitFunc) NULL,
                        (GBaseFinalizeFunc) NULL,
                        (GClassInitFunc) spi_accessible_event_spi_listener_class_init,
                        (GClassFinalizeFunc) NULL,
                        NULL, /* class data */
                        sizeof (SpiListener),
                        0, /* n preallocs */
                        (GInstanceInitFunc) spi_accessible_event_spi_listener_init,
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
                        POA_Accessibility_EventListener__init,
                        NULL,
                        G_STRUCT_OFFSET (SpiListenerClass, epv),
                        &tinfo,
                        "SpiAccessibleEventListener");
        }

        return type;
}

SpiAccessibleEventListener *
spi_accessible_event_spi_listener_new ()
{
    SpiAccessibleEventListener *retval =
    SPI_ACCESSIBLE_EVENT_SPI_LISTENER (g_object_new (spi_accessible_event_spi_listener_get_type (), NULL));
    return retval;
}

void   spi_accessible_event_spi_listener_add_callback (SpiAccessibleEventListener *listener,
                                               VoidEventListenerCB callback)
{
  listener->callbacks = g_list_append (listener->callbacks, callback);
}

void   spi_accessible_event_spi_listener_remove_callback (SpiAccessibleEventListener *listener,
                                                  VoidEventListenerCB callback)
{
  listener->callbacks = g_list_remove (listener->callbacks, callback);
}
