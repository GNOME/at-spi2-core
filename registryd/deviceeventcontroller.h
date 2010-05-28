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

#ifndef SPI_DEVICE_EVENT_CONTROLLER_H_
#define SPI_DEVICE_EVENT_CONTROLLER_H_

#include <X11/Xlib.h>
#include <dbus/dbus.h>

typedef struct _SpiDEController SpiDEController;

#include "registry.h"

G_BEGIN_DECLS

#define SPI_DEVICE_EVENT_CONTROLLER_TYPE        (spi_device_event_controller_get_type ())
#define SPI_DEVICE_EVENT_CONTROLLER(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), SPI_DEVICE_EVENT_CONTROLLER_TYPE, SpiDEController))
#define SPI_DEVICE_EVENT_CONTROLLER_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), SPI_DEVICE_EVENT_CONTROLLER_TYPE, SpiDEControllerClass))
#define SPI_IS_DEVICE_EVENT_CONTROLLER(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), SPI_DEVICE_EVENT_CONTROLLER_TYPE))
#define SPI_IS_DEVICE_EVENT_CONTROLLER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SPI_DEVICE_EVENT_CONTROLLER_TYPE))
#define SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SPI_DEVICE_EVENT_CONTROLLER_TYPE, SpiDEControllerClass))

struct _SpiDEController {
	GObject parent;
	DBusConnection *bus;
	SpiRegistry    *registry;
	GList          *key_listeners;
	GList          *mouse_listeners;
	GList          *keygrabs_list;
	Display        *xevie_display;
	GQueue *message_queue;
	guint message_queue_idle;
};

typedef struct {
  GObjectClass parent_class;
} SpiDEControllerClass;

GType            spi_device_event_controller_get_type (void);
SpiDEController *spi_device_event_controller_new      (SpiRegistry    *registry,
                                                       DBusConnection *bus);

void spi_device_event_controller_start_poll_mouse (SpiRegistry *registry);
void spi_device_event_controller_stop_poll_mouse (void);

void spi_remove_device_listeners (SpiDEController *controller, const char *bus_name);

SpiDEController *spi_registry_dec_new (SpiRegistry *reg, DBusConnection *bus);
G_END_DECLS

#endif /* DEVICEEVENTCONTROLLER_H_ */
