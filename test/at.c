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
#include <libspi/Accessibility.h>
#include <libspi/accessible.h>
#include <libspi/listener.h>

void
check_ev (CORBA_Environment *ev, char *desc)
{
        if (ev->_major != CORBA_NO_EXCEPTION) {
                fprintf(stderr,
                ("Accessibility app error: exception during CORBA call %s (%s)\n"),
                        CORBA_exception_id(ev), desc);
                CORBA_exception_free(ev);
                exit(-1);
        }
}

int
main(int argc, char **argv)
{
        CORBA_Environment ev;
        CORBA_Object oclient;
        char *obj_id;
        CORBA_long i;
        CORBA_short n_desktops;
        CORBA_long j;
        CORBA_long n_apps;
	CORBA_string s;
        Accessibility_Desktop desktop;
        Accessibility_Application app;
        Accessibility_Registry registry;
        SpiListener *listener;

        CORBA_exception_init(&ev);

        if (!bonobo_init (&argc, argv))
          {
            g_error ("Could not initialize Bonobo");
          }

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

        bonobo_activate ();

        listener = spi_listener_new ();

        registry = (Accessibility_Registry) oclient;

        Accessibility_Registry_registerGlobalEventListener
                                   (registry,
                                    (Accessibility_EventListener)
                                         bonobo_object_corba_objref (bonobo_object (listener)),
                                    "focus:",
                                    &ev);
        check_ev (&ev, "register:focus");
        Accessibility_Registry_registerGlobalEventListener
                                   (registry,
                                    (Accessibility_EventListener)
                                         bonobo_object_corba_objref (bonobo_object (listener)),
                                    "Gtk:GtkWidget:button-press-event",
                                    &ev);
        check_ev (&ev, "register:button_press");
        fprintf (stderr, "AT callback registered.\n");

            n_desktops = Accessibility_Registry_getDesktopCount (registry, &ev);

            for (i=0; i<n_desktops; ++i)
              {
                desktop = Accessibility_Registry_getDesktop (registry, i, &ev);
		s = Accessibility_Desktop__get_name (desktop, &ev);
                fprintf (stderr, "desktop %d name: %s\n", i, s);
	        CORBA_free (s);	 
                check_ev (&ev, "desktop:name");
                n_apps = Accessibility_Desktop__get_childCount (desktop, &ev);
                check_ev (&ev, "desktop:childCount");
                fprintf (stderr, "desktop has %d apps:\n", n_apps);
                for (j=0; j<n_apps; ++j)
                  {
                    app = (Accessibility_Application) Accessibility_Desktop_getChildAtIndex (desktop, j, &ev);
                    check_ev (&ev, "desktop:getChildAtIndex");
                    s = Accessibility_Application__get_name (app, &ev);
		    fprintf (stderr, "app %d name: %s\n", j, s);
		    CORBA_free (s);
                    check_ev (&ev, "app:getName");
                  }
              }

            fprintf (stderr, "finished querying desktop(s).\n");
            bonobo_main ();
               /* needed by at because it is a server ? */
        exit(0);
}
