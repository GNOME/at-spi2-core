/* AT-SPI : Assistive Technology Service Provider Interface
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

#ifndef SPI_REMOTE_OBJECT_H_
#define SPI_REMOTE_OBJECT_H_

#include <atk/atk.h>
#include <libspi/Accessibility.h>

G_BEGIN_DECLS

#define SPI_REMOTE_OBJECT_TYPE        (spi_remote_object_get_type ())
#define SPI_REMOTE_OBJECT(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), SPI_REMOTE_OBJECT_TYPE, SpiRemoteObject))
#define SPI_REMOTE_OBJECT_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), SPI_REMOTE_OBJECT_TYPE, SpiRemoteObjectClass))
#define SPI_IS_REMOTE_OBJECT(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), SPI_REMOTE_OBJECT_TYPE))
#define SPI_IS_REMOTE_OBJECT_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SPI_REMOTE_OBJECT_TYPE))
#define SPI_REMOTE_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((o), SPI_REMOTE_OBJECT_TYPE, SpiRemoteObjectClass))

typedef struct {
	AtkObject parent;
} SpiRemoteObject;

typedef struct {
        AtkObjectClass parent_class;
	Accessibility_Accessible (*get_accessible) (SpiRemoteObject *o);
} SpiRemoteObjectClass;

GType                    spi_remote_object_get_type       (void);
AtkObject               *spi_remote_object_new            (GObject           *o);
Accessibility_Accessible spi_remote_object_get_accessible (SpiRemoteObject   *o);

G_END_DECLS

#endif /* SPI_REMOTE_OBJECT_H_ */

