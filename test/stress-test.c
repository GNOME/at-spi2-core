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

#include <unistd.h>
#include <stdlib.h>
#include "../cspi/spi-private.h" /* A hack for now */
#include <glib-object.h>
#include <gtk/gtk.h>
#include <atk/atk.h>
#include <atk/atknoopobject.h>
#include <bonobo-activation/bonobo-activation-register.h>
#include <bonobo/bonobo-main.h>
#include <libspi/libspi.h>

int
main (int argc, char **argv)
{
	int i;
	Accessibility_Event e;
	Accessibility_Registry registry;
	CORBA_Environment ev;
	SpiAccessible *source;
	GObject *object;
	AtkObject *atko;
	GTimer *timer;

	gtk_init (&argc, &argv);
	SPI_init ();
	object = g_object_new (GTK_TYPE_BUTTON, NULL);
	atko = atk_no_op_object_new (object);

	sleep (1);

	g_print ("starting stress test...\n");

	CORBA_exception_init (&ev);

	registry = bonobo_activation_activate_from_id (
		"OAFIID:Accessibility_Registry:1.0", 0, NULL, &ev);
  
  if (ev._major != CORBA_NO_EXCEPTION)
    {
      g_error ("Accessibility app error: exception during "
	       "registry activation from id: %s\n",
	       CORBA_exception_id (&ev));
      CORBA_exception_free (&ev);
    }

  if (registry == CORBA_OBJECT_NIL)
    {
      g_error ("Could not locate registry");
    }

	bonobo_activate ();

	source = spi_accessible_new (atko);
	
	e.type = "object:test";
	e.source = BONOBO_OBJREF (source);
	e.detail1 = 0;
	e.detail2 = 0;

	timer = g_timer_new ();
	g_timer_start (timer);

	for (i = 0; i < 500; ++i) {
		Accessibility_Accessible_ref (e.source, &ev);
		Accessibility_Registry_notifyEvent (registry, &e, &ev);
	}
	g_timer_stop (timer);
	g_print ("%d notifications sent in  %f sec.\n", i,
		 g_timer_elapsed (timer, NULL));

	SPI_event_main ();

	sleep (15);
	
  return SPI_exit ();
}

