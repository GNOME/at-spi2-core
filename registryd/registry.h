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

#ifndef SPI_REGISTRY_H_
#define SPI_REGISTRY_H_

#include <glib.h>
#include <glib-object.h>

#include <dbus/dbus.h>

typedef struct _SpiRegistry SpiRegistry;
typedef struct _SpiRegistryClass SpiRegistryClass;

#include "deviceeventcontroller.h"

G_BEGIN_DECLS

#define SPI_REGISTRY_TYPE        (spi_registry_get_type ())
#define SPI_REGISTRY(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), SPI_REGISTRY_TYPE, SpiRegistry))
#define SPI_REGISTRY_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), SPI_REGISTRY_TYPE, SpiRegistryClass))
#define SPI_IS_REGISTRY(o)       (G_TYPE_CHECK__INSTANCE_TYPE ((o), SPI_REGISTRY_TYPE))
#define SPI_IS_REGISTRY_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SPI_REGISTRY_TYPE))

struct _SpiRegistry {
  GObject      parent;
  SpiDEController *dec;
  GPtrArray   *apps;
  dbus_int32_t id;

  DBusConnection *bus;
  GList *events;
};

struct _SpiRegistryClass {
  GObjectClass parent_class;
};

GType        spi_registry_get_type (void);
SpiRegistry *spi_registry_new      (DBusConnection *bus);

G_END_DECLS

#endif /* SPI_REGISTRY_H_ */
