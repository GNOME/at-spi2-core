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

#include <stdlib.h>
#include "spi.h"

void report_focus_event (void *fp);

void report_button_press (void *fp);

int
main(int argc, char **argv)
{
  int i, j;
  int n_desktops;
  int n_apps;
  Accessible *desktop;
  Accessible *application;
  AccessibleEventListener *focus_listener;
  AccessibleEventListener *button_listener;

  SPI_init();

  focus_listener = createEventListener (report_focus_event);
  button_listener = createEventListener (report_button_press);

  registerGlobalEventListener (focus_listener, "focus:");
  registerGlobalEventListener (button_listener, "Gtk:GtkWidget:button-press-event");

  n_desktops = getDesktopCount ();

  for (i=0; i<n_desktops; ++i)
    {
      desktop = getDesktop (i);
      fprintf (stderr, "desktop %d name: %s\n", i, Accessible_getName (desktop));
      n_apps = Accessible_getChildCount (desktop);
      for (j=0; j<n_apps; ++j)
        {
          application = Accessible_getChildAtIndex (desktop, j);
          fprintf (stderr, "app %d name: %s\n", j, Accessible_getName (application));
        }
    }
  SPI_event_main(FALSE);
}

void
report_focus_event (void *p)
{
  AccessibleEvent *ev = (AccessibleEvent *) p;
  fprintf (stderr, "%s event from %s\n", ev->type,
           Accessible_getName (&ev->source));
  if (Accessible_isComponent (&ev->source))
    {
      long x, y, width, height;
      AccessibleComponent *component = Accessible_getComponent (&ev->source);
      fprintf (stderr, "Source implements IDL:Accessibility/Component:1.0\n");
      AccessibleComponent_getExtents (component, &x, &y, &width, &height,
                                      COORD_TYPE_SCREEN);
      fprintf (stderr, "Bounding box: (%ld, %ld) ; (%ld, %ld)\n",
               x, y, x+width, y+height);
    }
}

void
report_button_press (void *p)
{
  AccessibleEvent *ev = (AccessibleEvent *) p;
  fprintf (stderr, "%s event from %s\n", ev->type,
           Accessible_getName (&ev->source));
}
