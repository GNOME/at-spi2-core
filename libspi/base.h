/* AT-SPI : Assistive Technology Service Provider Interface
 * Copyright 2001 Ximian, Inc.
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
#include <bonobo/bonobo-object.h>
#include <atk/atkobject.h>

G_BEGIN_DECLS

#define SPI_ACCESSIBLE_TYPE        (spi_accessible_get_type ())
#define SPI_ACCESSIBLE(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), SPI_ACCESSIBLE_TYPE, SpiAccessible))
#define SPI_ACCESSIBLE_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), SPI_ACCESSIBLE_TYPE, SpiAccessibleClass))
#define IS_SPI_ACCESSIBLE(o)       (G_TYPE_CHECK__INSTANCE_TYPE ((o), SPI_ACCESSIBLE_TYPE))
#define IS_SPI_ACCESSIBLE_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SPI_ACCESSIBLE_TYPE))

typedef struct {
        BonoboObject parent;

        AtkObject   *atko;
} SpiBase;

typedef struct {
        BonoboObjectClass parent_class;
} SpiBaseClass;

GType      spi_base_get_type       (void);
gpointer   spi_base_construct      (SpiBase   *base,
				    AtkObject *o);
AtkObject *spi_base_get_atk_object (SpiBase   *base);

G_END_DECLS

#endif /* SPI_BASE_H_ */
