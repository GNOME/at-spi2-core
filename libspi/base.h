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

#ifndef SPI_BASE_H_
#define SPI_BASE_H_

#include <glib/gmacros.h>
#include <atk/atkobject.h>
#include <bonobo/bonobo-object.h>
#include <libspi/Accessibility.h>

G_BEGIN_DECLS

#define SPI_TYPE_BASE        (spi_base_get_type ())
#define SPI_BASE(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), SPI_TYPE_BASE, SpiBase))
#define SPI_BASE_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), SPI_TYPE_BASE, SpiBaseClass))
#define SPI_IS_BASE(o)       (G_TYPE_CHECK__INSTANCE_TYPE ((o), SPI_TYPE_BASE))
#define SPI_IS_BASE_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SPI_TYPE_BASE))

typedef struct {
        BonoboObject parent;
        GObject *gobj;
} SpiBase;

typedef struct {
        BonoboObjectClass parent_class;
} SpiBaseClass;

GType      spi_base_get_type          (void);
void       spi_base_construct         (SpiBase   *base,
				       GObject   *gobject);
void       spi_base_construct_default (SpiBase   *base);
GObject   *spi_base_get_gobject       (SpiBase   *base);

G_END_DECLS

#endif /* SPI_BASE_H_ */
