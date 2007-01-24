/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2002 Ximian, Inc.
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

#ifndef __SPI_LISTENER_IMPL_H__
#define __SPI_LISTENER_IMP_H__

#include <libspi/eventlistener.h>
#include <libspi/devicelistener.h>
#include <cspi/spi-impl.h>
#include <cspi/spi-listener.h>

G_BEGIN_DECLS

#define CSPI_EVENT_LISTENER_TYPE        (cspi_event_listener_get_type ())
#define CSPI_EVENT_LISTENER(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), CSPI_EVENT_LISTENER_TYPE, CSpiEventListener))
#define CSPI_EVENT_LISTENER_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), CSPI_EVENT_LISTENER_TYPE, CSpiEventListenerClass))
#define CSPI_IS_EVENT_LISTENER(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), CSPI_EVENT_LISTENER_TYPE))
#define CSPI_IS_EVENT_LISTENER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), CSPI_EVENT_LISTENER_TYPE))

typedef struct {
	SpiEventListener parent;
	GList           *callbacks;
} CSpiEventListener;
typedef SpiEventListenerClass CSpiEventListenerClass;

GType cspi_event_listener_get_type (void);

#define CSPI_DEVICE_LISTENER_TYPE        (cspi_device_listener_get_type ())
#define CSPI_DEVICE_LISTENER(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), CSPI_DEVICE_LISTENER_TYPE, CSpiDeviceListener))
#define CSPI_DEVICE_LISTENER_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), CSPI_DEVICE_LISTENER_TYPE, CSpiDeviceListenerClass))
#define CSPI_IS_DEVICE_LISTENER(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), CSPI_DEVICE_LISTENER_TYPE))
#define CSPI_IS_DEVICE_LISTENER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), CSPI_DEVICE_LISTENER_TYPE))

typedef struct {
	SpiDeviceListener parent;
	GList            *callbacks;
} CSpiDeviceListener;
typedef SpiDeviceListenerClass CSpiDeviceListenerClass;

CORBA_Object
cspi_device_listener_get_corba (AccessibleDeviceListener *listener);
GType cspi_device_listener_get_type (void);
gpointer cspi_device_listener_new (void);
void cspi_device_listener_add_cb (AccessibleDeviceListener  *al,
                                  AccessibleDeviceListenerCB callback,
                                  void                      *user_data);
void cspi_device_listener_remove_cb (AccessibleDeviceListener  *al,
                                     AccessibleDeviceListenerCB callback);
void cspi_device_listener_unref (AccessibleDeviceListener *listener);


G_END_DECLS

#endif /* __SPI_LISTENER_IMPL_H__ */
