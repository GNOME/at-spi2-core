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

#include <stdio.h>
#include <stdlib.h>
#include <libbonobo.h>
#include <bonobo-activation/bonobo-activation.h>
#include <atk/atkobject.h>
#include <libspi/Accessibility.h>
#include <libspi/accessible.h>
#include <libspi/application.h>

#define APP_STATIC_BUFF_SZ 30

int
main(int argc, char **argv)
{
        CORBA_Environment ev;
        CORBA_Object oclient;
        AtkObject *atko;
        char *obj_id;
        char sbuf[APP_STATIC_BUFF_SZ];

        Accessibility_Registry registry;
        Accessibility_Event e;
        SpiAccessible *accessible;
        SpiApplication *app;

        CORBA_exception_init(&ev);

        if (!bonobo_init (&argc, argv))
          {
            g_error ("Could not initialize Bonobo");
          }

        /* Create the accesssible application server object */
        g_snprintf(sbuf, APP_STATIC_BUFF_SZ, "application-%s", argv[0]);

        atko = g_object_new (atk_object_get_type(), NULL);
        atk_object_set_name (atko, sbuf);
        atk_object_set_description( atko, "test application for accessibility SPI");
        app = spi_application_new(atko);

        /* Create the SpiAccessible 'source' for the event */
        accessible = spi_accessible_new (atko);
        fprintf(stderr, "accessible created.\n");

        e.source = bonobo_object_corba_objref ( bonobo_object (accessible));
        e.type = CORBA_string_dup ("focus:");

        obj_id = "OAFIID:Accessibility_Registry:1.0";

        oclient = bonobo_activation_activate_from_id (obj_id, 0, NULL, &ev);
        if (ev._major != CORBA_NO_EXCEPTION) {
                fprintf(stderr,
                ("Accessibility app error: exception during registry activation from id: %s\n"),
                        CORBA_exception_id(&ev));
                CORBA_exception_free(&ev);
                exit(-1);
        }

        if (CORBA_Object_is_nil (oclient, &ev))
          {
            g_error ("Could not locate registry");
          }

        registry = (Accessibility_Registry) oclient;

        Accessibility_Registry_registerApplication (registry,
                                                    bonobo_object_corba_objref (bonobo_object (app)),
                                                    &ev);
        fprintf(stderr, "registerApplication has been called.\n");

        Accessibility_Registry_notifyEvent (registry, &e, &ev);
        fprintf (stderr, "notify event has been called.\n");

        bonobo_main (); /* needed when app becomes a server ? */
        exit(0);
}
