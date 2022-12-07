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

#include "event-source.h"

typedef struct
{
  GSource source;

  Display *display;
  GPollFD event_poll_fd;
} DisplaySource;

/*---------------------------------------------------------------------------*/

static void (*_spi_default_filter) (XEvent *, void *) = NULL;
static void *_spi_default_filter_data = NULL;

/*---------------------------------------------------------------------------*/

static gboolean
event_prepare (GSource *source, gint *timeout)
{
  Display *display = ((DisplaySource *) source)->display;
  gboolean retval;

  *timeout = -1;
  retval = XPending (display);

  return retval;
}

static gboolean
event_check (GSource *source)
{
  DisplaySource *display_source = (DisplaySource *) source;
  gboolean retval;

  if (display_source->event_poll_fd.revents & G_IO_IN)
    retval = XPending (display_source->display);
  else
    retval = FALSE;

  return retval;
}

static gboolean
event_dispatch (GSource *source, GSourceFunc callback, gpointer user_data)
{
  Display *display = ((DisplaySource *) source)->display;
  XEvent xevent;

  /* TODO - Should this be "if (XPending (display))"?
   *        The effect of this might be to run other main loop functions
   *        before dispatching the next XEvent.
   */
  while (XPending (display))
    {
      XNextEvent (display, &xevent);

      switch (xevent.type)
        {
        case KeyPress:
        case KeyRelease:
          break;
        default:
          if (XFilterEvent (&xevent, None))
            continue;
        }

      if (_spi_default_filter)
        {
          _spi_default_filter (&xevent, _spi_default_filter_data);
        }
    }

  return TRUE;
}

/*---------------------------------------------------------------------------*/

static GSourceFuncs event_funcs = {
  event_prepare,
  event_check,
  event_dispatch,
  NULL
};

static GSource *
display_source_new (Display *display)
{
  GSource *source = g_source_new (&event_funcs, sizeof (DisplaySource));
  DisplaySource *display_source = (DisplaySource *) source;
  g_source_set_name (source, "[at-spi2-core] display_source_funcs");

  display_source->display = display;

  return source;
}

/*---------------------------------------------------------------------------*/

static DisplaySource *spi_display_source = NULL;

void
spi_events_init (Display *display)
{
  GSource *source;

  int connection_number = ConnectionNumber (display);

  source = display_source_new (display);
  spi_display_source = (DisplaySource *) source;

  g_source_set_priority (source, G_PRIORITY_DEFAULT);

  spi_display_source->event_poll_fd.fd = connection_number;
  spi_display_source->event_poll_fd.events = G_IO_IN;

  g_source_add_poll (source, &spi_display_source->event_poll_fd);
  g_source_set_can_recurse (source, TRUE);
  g_source_attach (source, NULL);
}

void
spi_events_uninit ()
{
  if (spi_display_source)
    {
      g_source_destroy ((GSource *) spi_display_source);
      g_source_unref ((GSource *) spi_display_source);
      spi_display_source = NULL;
    }
}

void
spi_set_events (long event_mask)
{
  long xevent_mask = StructureNotifyMask | PropertyChangeMask;
  xevent_mask |= event_mask;

  XSelectInput (spi_display_source->display,
                DefaultRootWindow (spi_display_source->display),
                xevent_mask);
}

void
spi_set_filter (void (*filter) (XEvent *, void *), void *data)
{
  _spi_default_filter = filter;
  _spi_default_filter_data = data;
}

/*END------------------------------------------------------------------------*/
