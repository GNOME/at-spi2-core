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

#ifndef SPI_COMPONENT_H_
#define SPI_COMPONENT_H_

#include <libspi/base.h>
#include <atk/atkcomponent.h>

G_BEGIN_DECLS

#define SPI_COMPONENT_TYPE        (spi_component_get_type ())
#define SPI_COMPONENT(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), SPI_COMPONENT_TYPE, SpiComponent))
#define SPI_COMPONENT_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), SPI_COMPONENT_TYPE, SpiComponentClass))
#define SPI_IS_COMPONENT(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), SPI_COMPONENT_TYPE))
#define SPI_IS_COMPONENT_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SPI_COMPONENT_TYPE))

typedef struct {
        SpiBase parent;
} SpiComponent;

typedef struct {
        SpiBaseClass parent_class;
        POA_Accessibility_Component__epv epv;
} SpiComponentClass;

GType         spi_component_get_type      (void);
SpiComponent *spi_component_interface_new (AtkObject *o);

G_END_DECLS

#endif /* SPI_COMPONENT_H_ */
