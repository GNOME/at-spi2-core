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
 * accessibleeventlistener.c: bonobo implementation of Listener.idl,
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
#define PARENT_TYPE LISTENER_TYPE

/*
 * A pointer to our parent object class
 */
static ListenerClass *accessible_event_listener_parent_class;

/*
 * Implemented GObject::finalize
 */
static void
accessible_event_listener_object_finalize (GObject *object)
{
#ifdef SPI_DEBUG
        fprintf(stderr, "listener_object_finalize called\n");
#endif
        ((GObjectClass *) accessible_event_listener_parent_class)->finalize (object);
}

/*
 * CORBA Accessibility::Listener::notifyEvent method implementation
 */

static void
impl_accessible_event_notify_event (PortableServer_Servant     servant,
                                    const Accessibility_Event *e,
                                    CORBA_Environment         *ev)
{

  int n;
  int len;
  VoidEventListenerCB cb;
  AccessibleEventListener *listener = ACCESSIBLE_EVENT_LISTENER (
                                       bonobo_object_from_servant (servant));
  len = g_list_length (listener->callbacks);

  for (n=0; n<len; ++n)
    {
      cb =  (VoidEventListenerCB) g_list_nth_data (listener->callbacks, n);
      (*cb) (NULL);
    }
  bonobo_object_release_unref (e->target, ev);
}

static void
accessible_event_listener_class_init (AccessibleEventListenerClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        ListenerClass * listener_class = (ListenerClass *) klass;
        POA_Accessibility_EventListener__epv *epv = &listener_class->epv;
        accessible_event_listener_parent_class = g_type_class_ref (LISTENER_TYPE);

        object_class->finalize = accessible_event_listener_object_finalize;

        epv->notifyEvent = impl_accessible_event_notify_event;
}

static void
accessible_event_listener_init (Listener *listener)
{
}

GType
accessible_event_listener_get_type (void)
{
        static GType type = 0;

        if (!type) {
                static const GTypeInfo tinfo = {
                        sizeof (AccessibleEventListenerClass),
                        (GBaseInitFunc) NULL,
                        (GBaseFinalizeFunc) NULL,
                        (GClassInitFunc) accessible_event_listener_class_init,
                        (GClassFinalizeFunc) NULL,
                        NULL, /* class data */
                        sizeof (Listener),
                        0, /* n preallocs */
                        (GInstanceInitFunc) accessible_event_listener_init,
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
                        G_STRUCT_OFFSET (ListenerClass, epv),
                        &tinfo,
                        "AccessibleEventListener");
        }

        return type;
}

AccessibleEventListener *
accessible_event_listener_new ()
{
    AccessibleEventListener *retval =
               LISTENER (g_object_new (accessible_event_listener_get_type (), NULL));
    return retval;
}

void   accessible_event_listener_add_callback (AccessibleEventListener *listener,
                                               VoidEventListenerCB callback)
{
  listener->callbacks = g_list_append (listener->callbacks, listener);
}

void   accessible_event_listener_remove_callback (AccessibleEventListener *listener,
                                                  VoidEventListenerCB callback)
{
  listener->callbacks = g_list_remove (listener->callbacks, listener);
}
