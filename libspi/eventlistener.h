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

#ifndef SPI_ACCESSIBLE_EVENT_LISTENER_H_
#define SPI_ACCESSIBLE_EVENT_LISTENER_H_

#include <libspi/listener.h>

G_BEGIN_DECLS

#define SPI_EVENT_LISTENER_TYPE        (spi_event_listener_get_type ())
#define SPI_EVENT_LISTENER(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), SPI_EVENT_LISTENER_TYPE, SpiEventListener))
#define SPI_EVENT_LISTENER_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), SPI_EVENT_LISTENER_TYPE, SpiEventListenerClass))
#define SPI_IS_EVENT_LISTENER(o)       (G_TYPE_CHECK__INSTANCE_TYPE ((o), SPI_EVENT_LISTENER_TYPE))
#define SPI_IS_EVENT_LISTENER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SPI_EVENT_LISTENER_TYPE))

typedef struct _SpiEventListener SpiEventListener;

struct _SpiEventListener {
  SpiListener parent;
};

typedef struct {
  SpiListenerClass parent_class;

  /* signals */
  void (*event) (SpiEventListener    *listener,
		 Accessibility_Event *e);
  
} SpiEventListenerClass;

GType              spi_event_listener_get_type        (void);
SpiEventListener  *spi_event_listener_new             (void);

G_END_DECLS

#endif /* SPI_ACCESSIBLE_EVENT_SPI_LISTENER_H_ */
