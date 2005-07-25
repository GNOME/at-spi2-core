/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002, 2003 Sun Microsystems Inc.,
 * Copyright 2001, 2002, 2003 Ximian, Inc.
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../cspi/spi-private.h" /* A hack for now */

static void report_window_event  (const AccessibleEvent *event, void *user_data);
static AccessibleEventListener *window_listener;

int
main (int argc, char **argv)
{
  SPI_init ();

  window_listener = SPI_createAccessibleEventListener (
	  report_window_event, NULL);

  SPI_registerGlobalEventListener (window_listener,
				   "window:minimize");
  SPI_registerGlobalEventListener (window_listener,
				   "window:maximize");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:restore");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:activate");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:deactivate");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:create");
  SPI_registerGlobalEventListener (window_listener,
		  		   "window:destroy");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:lower");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:raise");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:resize");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:shade");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:unshade");
  SPI_event_main ();

  putenv ("AT_BRIDGE_SHUTDOWN=1");

  /*
   * TODO: Add a key event listener that calls test_exit, to
   * deregister and cleanup appropriately.
   */

  return SPI_exit ();
}

void
report_window_event (const AccessibleEvent *event, void *user_data)
{
  char *t, *s = Accessible_getName (event->source);
  t = AccessibleWindowEvent_getTitleString (event);
  if (t == NULL) t = "";
  fprintf (stderr, "%s %s %s\n", event->type, s, t);
  SPI_freeString (s);
  SPI_freeString (t);
}

