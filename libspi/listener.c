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

#include <config.h>
#include <bonobo/Bonobo.h>
#include <libspi/Accessibility.h>

/*
 * This pulls the definition for the BonoboObject (GType)
 */
#include "listener.h"

/*
 * Our parent Gtk object type
 */
#define PARENT_TYPE BONOBO_OBJECT_TYPE

/*
 * A pointer to our parent object class
 */
static GObjectClass *spi_listener_parent_class;

/*
 * Implemented GObject::finalize
 */
static void
spi_listener_object_finalize (GObject *object)
{
/*        SpiListener *listener = SPI_LISTENER (object); */

#ifdef SPI_DEBUG
        fprintf(stderr, "spi_listener_object_finalize called\n");
#endif
        spi_listener_parent_class->finalize (object);
}

/*
 * CORBA Accessibility::SpiListener::notifyEvent method implementation
 */

static void
impl_notify_event (PortableServer_Servant     servant,
                   const Accessibility_Event *e,
                   CORBA_Environment         *ev)
{
#ifdef SPI_DEBUG
  fprintf (stderr, "notify %s...\n", e->type);
  fprintf (stderr, "source name: '%s'\n",
           Accessibility_SpiAccessible__get_name(e->source, ev));
  if (ev->_major != CORBA_NO_EXCEPTION) {
    fprintf(stderr,
            ("Accessibility app error: exception during event notification: %s\n"),
            CORBA_exception_id(ev));
    exit(-1);
  }
  /*
  fprintf (stderr, "source is component ? : %s\n",
           Accessibility_SpiAccessible_queryInterface (e->source,
                                                    "IDL:Accessibility/SpiComponent:1.0",
                                                    ev)
           ? "yes" : "no");
  */
#endif
  Accessibility_SpiAccessible_unref (e->source, ev);
}

static void
spi_listener_class_init (SpiListenerClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        POA_Accessibility_EventListener__epv *epv = &klass->epv;
        spi_listener_parent_class = g_type_class_ref (BONOBO_OBJECT_TYPE);

        object_class->finalize = spi_listener_object_finalize;

        epv->notifyEvent = impl_notify_event;
}

static void
spi_listener_init (SpiListener *listener)
{
}

GType
spi_listener_get_type (void)
{
        static GType type = 0;

        if (!type) {
                static const GTypeInfo tinfo = {
                        sizeof (SpiListenerClass),
                        (GBaseInitFunc) NULL,
                        (GBaseFinalizeFunc) NULL,
                        (GClassInitFunc) spi_listener_class_init,
                        (GClassFinalizeFunc) NULL,
                        NULL, /* class data */
                        sizeof (SpiListener),
                        0, /* n preallocs */
                        (GInstanceInitFunc) spi_listener_init,
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
                        "SpiListener");
        }

        return type;
}

SpiListener *
spi_listener_new (void)
{
    SpiListener *retval =
               SPI_LISTENER (g_object_new (spi_listener_get_type (), NULL));
    return retval;
}
