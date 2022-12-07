/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2009 Nokia.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <glib.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xutil.h>

#include <stdio.h>
#include <stdlib.h>

#include "display.h"

static Display *default_display = NULL;

Display *
spi_set_display (const char *display_name)
{
  /*
   * TODO - Should we ever do anything different might need to
   * close previous display.
   */
  default_display = XOpenDisplay (display_name);
  if (!default_display)
    {
      g_warning ("AT-SPI: Cannot open default display");
      exit (1);
    }
  return default_display;
}

Display *
spi_get_display ()
{
  if (!default_display)
    spi_set_display (NULL);

  return default_display;
}

Window
spi_get_root_window ()
{
  if (!default_display)
    spi_set_display (NULL);

  return DefaultRootWindow (default_display);
}

static int (*old_x_error_handler) (Display *, XErrorEvent *);
static int x_error_code;

static int
spi_x_error_handler (Display *display, XErrorEvent *error)
{
  if (error->error_code)
    x_error_code = error->error_code;
  else
    x_error_code = 0;

  return 0;
}

void
spi_x_error_trap (void)
{
  old_x_error_handler = XSetErrorHandler (spi_x_error_handler);
}

int
spi_x_error_release (void)
{
  XSetErrorHandler (old_x_error_handler);
  return x_error_code;
}
