/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2020 SUSE LLC.
 *
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

#ifndef _ATSPI_DEVICE_MUTTER_H_
#define _ATSPI_DEVICE_MUTTER_H_

#include "glib-object.h"

#include "atspi-device.h"
#include "atspi-types.h"

G_BEGIN_DECLS

#define ATSPI_TYPE_DEVICE_MUTTER (atspi_device_mutter_get_type ())
#define ATSPI_DEVICE_MUTTER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATSPI_TYPE_DEVICE_MUTTER, AtspiDeviceMutter))
#define ATSPI_DEVICE_MUTTER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), ATSPI_TYPE_DEVICE_MUTTER, AtspiDeviceMutterClass))
#define ATSPI_IS_DEVICE_MUTTER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATSPI_TYPE_DEVICE_MUTTER))
#define ATSPI_IS_DEVICE_MUTTER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ATSPI_TYPE_DEVICE_MUTTER))
#define ATSPI_DEVICE_MUTTER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), ATSPI_TYPE_DEVICE_MUTTER, AtspiDeviceMutterClass))

typedef struct _AtspiDeviceMutter AtspiDeviceMutter;
struct _AtspiDeviceMutter
{
  AtspiDevice parent;
};

typedef struct _AtspiDeviceMutterClass AtspiDeviceMutterClass;
struct _AtspiDeviceMutterClass
{
  AtspiDeviceClass parent_class;
};

GType atspi_device_mutter_get_type (void);

AtspiDeviceMutter *atspi_device_mutter_new ();

G_END_DECLS

#endif /* _ATSPI_DEVICE_MUTTER_H_ */
