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

#ifndef SPI_DEVICE_EVENT_CONTROLLER_H_
#define SPI_DEVICE_EVENT_CONTROLLER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-object.h>
#include <libspi/Accessibility.h>
#include "keystrokelistener.h"

#define SPI_DEVICE_EVENT_CONTROLLER_TYPE        (spi_device_event_controller_get_type ())
#define SPI_DEVICE_EVENT_CONTROLLER(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), SPI_DEVICE_EVENT_CONTROLLER_TYPE, SpiDeviceEventController))
#define SPI_DEVICE_EVENT_CONTROLLER_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), SPI_DEVICE_EVENT_CONTROLLER_TYPE, SpiDeviceEventControllerClass))
#define IS_SPI_DEVICE_EVENT_CONTROLLER(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), SPI_DEVICE_EVENT_CONTROLLER_TYPE))
#define IS_SPI_DEVICE_EVENT_CONTROLLER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SPI_DEVICE_EVENT_CONTROLLER_TYPE))
#define SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SPI_DEVICE_EVENT_CONTROLLER_TYPE, SpiDeviceEventControllerClass))

typedef struct {
        BonoboObject parent;
        GList *key_listeners;
        GList *mouse_listeners;
	GList *keymask_list;
} SpiDeviceEventController;

typedef struct {
        BonoboObjectClass parent_class;
        POA_Accessibility_DeviceEventController__epv epv;
	gboolean (*check_key_event) (SpiDeviceEventController *controller);
} SpiDeviceEventControllerClass;

GType               spi_device_event_controller_get_type   (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DEVICEEVENTCONTROLLER_H_ */
