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

#ifndef SPI_KEYSTROKE_LISTENER_H_
#define SPI_KEYSTROKE_LISTENER_H_

G_BEGIN_DECLS

#include <libspi/devicelistener.h>

#define SPI_KEYSTROKE_LISTENER_TYPE        SPI_DEVICE_LISTENER_TYPE
#define SPI_KEYSTROKE_LISTENER(o)          SPI_DEVICE_LISTENER(o)
#define SPI_KEYSTROKE_LISTENER_CLASS(k)    SPI_DEVICE_LISTENER_CLASS(k)
#define SPI_IS_KEYSTROKE_LISTENER(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), SPI_KEYSTROKE_LISTENER_TYPE))
#define SPI_IS_KEYSTROKE_LISTENER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SPI_KEYSTROKE_LISTENER_TYPE))

typedef struct _SpiDeviceListener SpiKeystrokeListener;

typedef SpiDeviceListenerClass SpiKeystrokeListenerClass;

#define spi_keystroke_listener_get_type spi_device_listener_get_type
#define spi_keystroke_listener_new      spi_device_listener_new

G_END_DECLS

#endif /* KEYSTROKE_SPI_LISTENER_H_ */
