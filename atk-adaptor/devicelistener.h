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

#ifndef SPI_DEVICE_LISTENER_H_
#define SPI_DEVICE_LISTENER_H_

#include <bonobo/bonobo-object.h>
#include <atk/atkobject.h>
#include <libspi/Accessibility.h>
#include <libspi/keymasks.h>

G_BEGIN_DECLS

#define SPI_DEVICE_LISTENER_TYPE        (spi_device_listener_get_type ())
#define SPI_DEVICE_LISTENER(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), SPI_DEVICE_LISTENER_TYPE, SpiDeviceListener))
#define SPI_DEVICE_LISTENER_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), SPI_DEVICE_LISTENER_TYPE, SpiDeviceListenerClass))
#define SPI_IS_DEVICE_LISTENER(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), SPI_DEVICE_LISTENER_TYPE))
#define SPI_IS_DEVICE_LISTENER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SPI_DEVICE_LISTENER_TYPE))

typedef struct _SpiDeviceListener SpiDeviceListener;

struct _SpiDeviceListener {
        BonoboObject parent;
};

typedef struct {
        BonoboObjectClass parent_class;
        POA_Accessibility_DeviceEventListener__epv epv;

	gboolean (*device_event) (SpiDeviceListener *listener,
				  const Accessibility_DeviceEvent *key);
} SpiDeviceListenerClass;

GType                  spi_device_listener_get_type        (void);
SpiDeviceListener     *spi_device_listener_new             (void);

G_END_DECLS

#endif /* DEVICE_SPI_LISTENER_H_ */
