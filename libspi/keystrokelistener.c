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
#include "keystrokelistener.h"

/*
 * Our parent Gtk object type
 */
#define PARENT_TYPE BONOBO_OBJECT_TYPE

/*
 * A pointer to our parent object class
 */
static GObjectClass *keystroke_listener_parent_class;

/*
 * Implemented GObject::finalize
 */
static void
keystroke_listener_object_finalize (GObject *object)
{
/*        KeystrokeListener *keystroke_listener = KEYSTROKE_LISTENER (object); */

#ifdef SPI_DEBUG
        fprintf(stderr, "keystroke_listener_object_finalize called\n");
#endif
        keystroke_listener_parent_class->finalize (object);
}

/*
 * CORBA Accessibility::KeystrokeListener::keyEvent method implementation
 */

static void
impl_key_event (PortableServer_Servant     servant,
		const Accessibility_KeyStroke *key,
		CORBA_Environment         *ev)
{
#ifdef SPI_DEBUG
  if (ev->_major != CORBA_NO_EXCEPTION) {
    fprintf(stderr,
            ("Accessibility app error: exception during keystroke notification: %s\n"),
            CORBA_exception_id(ev));
    exit(-1);
  }
#endif
}

static void
keystroke_listener_class_init (KeystrokeListenerClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        POA_Accessibility_KeystrokeListener__epv *epv = &klass->epv;
        keystroke_listener_parent_class = g_type_class_ref (BONOBO_OBJECT_TYPE);

        object_class->finalize = keystroke_listener_object_finalize;

        epv->keyEvent = impl_key_event;
}

static void
keystroke_listener_init (KeystrokeListener *keystroke_listener)
{
}

GType
keystroke_listener_get_type (void)
{
        static GType type = 0;

        if (!type) {
                static const GTypeInfo tinfo = {
                        sizeof (KeystrokeListenerClass),
                        (GBaseInitFunc) NULL,
                        (GBaseFinalizeFunc) NULL,
                        (GClassInitFunc) keystroke_listener_class_init,
                        (GClassFinalizeFunc) NULL,
                        NULL, /* class data */
                        sizeof (KeystrokeListener),
                        0, /* n preallocs */
                        (GInstanceInitFunc) keystroke_listener_init,
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
                        POA_Accessibility_KeystrokeListener__init,
                        NULL,
                        G_STRUCT_OFFSET (KeystrokeListenerClass, epv),
                        &tinfo,
                        "KeystrokeListener");
        }

        return type;
}

KeystrokeListener *
keystroke_listener_new (void)
{
    KeystrokeListener *retval =
               KEYSTROKE_LISTENER (g_object_new (keystroke_listener_get_type (), NULL));
    return retval;
}
