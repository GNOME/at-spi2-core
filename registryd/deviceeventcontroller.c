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
#include "deviceeventcontroller.h"

/*
 * Our parent Gtk object type
 */
#define PARENT_TYPE BONOBO_OBJECT_TYPE

/*
 * A pointer to our parent object class
 */
static GObjectClass *device_event_controller_parent_class;

/*
 * Implemented GObject::finalize
 */
static void
device_event_controller_object_finalize (GObject *object)
{

#ifdef SPI_DEBUG
        fprintf(stderr, "device_event_controller_object_finalize called\n");
#endif
        device_event_controller_parent_class->finalize (object);
}

/*
 * CORBA Accessibility::DeviceEventController::registerKeystrokeListener
 *     method implementation
 */
static void
impl_register_keystroke_listener (PortableServer_Servant     servant,
				  const Accessibility_KeystrokeListener l,
				  CORBA_Environment         *ev)
{
#ifdef SPI_DEBUG
	fprintf (stderr, "registering keystroke listener %p\n", l);
#endif
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
#ifdef SPI_DEBUG
	fprintf (stderr, "registering mouse listener %p\n", l);
#endif
}
*/

/*
 * CORBA Accessibility::DeviceEventController::registerKeystrokeListener
 *     method implementation
 */
static void
impl_generate_key_event (PortableServer_Servant     servant,
			 const CORBA_long           keyEventID,
			 CORBA_Environment         *ev)
{
#ifdef SPI_DEBUG
	fprintf (stderr, "synthesizing keystroke %ld\n", keyEventID);
#endif
}

/*
 * CORBA Accessibility::DeviceEventController::generateMouseEvent
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
device_event_controller_class_init (DeviceEventControllerClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        POA_Accessibility_DeviceEventController__epv *epv = &klass->epv;
        device_event_controller_parent_class = g_type_class_ref (BONOBO_OBJECT_TYPE);

        object_class->finalize = device_event_controller_object_finalize;

        epv->registerKeystrokeListener = impl_register_keystroke_listener;
/*        epv->registerMouseListener = impl_register_mouse_listener; */
        epv->generateKeyEvent = impl_generate_key_event;
        epv->generateMouseEvent = impl_generate_mouse_event;
}

static void
device_event_controller_init (DeviceEventController *device_event_controller)
{
}

GType
device_event_controller_get_type (void)
{
        static GType type = 0;

        if (!type) {
                static const GTypeInfo tinfo = {
                        sizeof (DeviceEventControllerClass),
                        (GBaseInitFunc) NULL,
                        (GBaseFinalizeFunc) NULL,
                        (GClassInitFunc) device_event_controller_class_init,
                        (GClassFinalizeFunc) NULL,
                        NULL, /* class data */
                        sizeof (DeviceEventController),
                        0, /* n preallocs */
                        (GInstanceInitFunc) device_event_controller_init,
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
                        POA_Accessibility_DeviceEventController__init,
                        NULL,
                        G_STRUCT_OFFSET (DeviceEventControllerClass, epv),
                        &tinfo,
                        "DeviceEventController");
        }

        return type;
}

