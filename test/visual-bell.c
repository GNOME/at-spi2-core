/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
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
#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>

static	Display *display;
static  Window flash_window = None;

static void
visual_bell_notify (XkbAnyEvent *xkb_ev)
{
	Window root;
	int width, height;
	root = RootWindow (display, DefaultScreen (display));
	width = DisplayWidth (display, DefaultScreen (display));
	height = DisplayHeight (display, DefaultScreen (display));
	if (flash_window == None)
	{
		Visual *visual = CopyFromParent;
		XVisualInfo info_return;
		XSetWindowAttributes xswa;
		int depth = CopyFromParent;
		xswa.save_under = True;
		xswa.override_redirect = True;
		/* TODO: use XGetVisualInfo and determine which is an
		   overlay, if one is present.  Not sure how to tell
		   this yet... */
		if (XMatchVisualInfo (display,
				      DefaultScreen (display),
				      8,
				      PseudoColor,
				      &info_return)) {
			depth = 8;
			visual = info_return.visual;
		}
		else
		{
			fprintf (stderr, "could not create overlay visual, using default root visual type\n");
		}
		flash_window = XCreateWindow (display, root,
					      0, 0, width, height,
					      0, depth,
					      InputOutput,
					      visual,
					      CWSaveUnder | CWOverrideRedirect,
					      &xswa);
		XSelectInput (display, flash_window, ExposureMask);
		XMapWindow (display, flash_window);
	}
	else
	{
		/* just draw something in the window */
		GC gc = XCreateGC (display, flash_window, 0, NULL);
		XMapWindow (display, flash_window);
		XSetForeground (display, gc,
				WhitePixel (display, DefaultScreen (display)));
		XFillRectangle (display, flash_window, gc,
				0, 0, width, height);
		XSetForeground (display, gc,
				BlackPixel (display, DefaultScreen (display)));
		XFillRectangle (display, flash_window, gc,
				0, 0, width, height);
	}
	XFlush (display);
}

int main (int argc, char **argv)
{
	XEvent xev;
	int ir, xkb_base_event_type, reason_return;
	char *display_name = getenv ("DISPLAY");

	if (!display_name) display_name = ":0.0";
	
	display = XkbOpenDisplay (display_name,
				  &xkb_base_event_type,
				  &ir, NULL, NULL, &reason_return);
	if (!display)
	{
		fprintf (stderr, "Could not connect to display! (%d)\n",
			 reason_return);
		exit (-1);
	}
	
	XkbSelectEvents (display,
			 XkbUseCoreKbd,
			 XkbBellNotifyMask,
			 XkbBellNotifyMask);

	/* comment this out to prevent bell on startup */
	XkbBell (display, None, 100, None);

	while (1)
	{
		XNextEvent (display, &xev);
		if (xev.type == Expose)
		{
			XExposeEvent *exev = (XExposeEvent *) &xev;
			if (exev->window == flash_window)
			{
				XUnmapWindow (display, flash_window);
                                /* discard pending bells */
				XSync (display, True); 
				XFlush (display);
			}
		}
		else if (xev.type == xkb_base_event_type)
		{
			XkbAnyEvent *xkb_ev = (XkbAnyEvent *) &xev;
			
			switch (xkb_ev->xkb_type)
			{
			case XkbBellNotify:
				/* flash something */
				visual_bell_notify (xkb_ev);
				break;
			default:
			}
		}
	}
}

