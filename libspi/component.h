/* ATK -  Accessibility Toolkit
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

#ifndef SPI_COMPONENT_H_
#define SPI_COMPONENT_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-object.h>
#include <atk/atk.h>
#include <libspi/Accessibility.h>

#define SPI_COMPONENT_TYPE        (accessibility_component_get_type ())
#define SPI_COMPONENT(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), SPI_COMPONENT_TYPE, SpiComponent))
#define SPI_COMPONENT_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), SPI_COMPONENT_TYPE, SpiComponentClass))
#define IS_SPI_COMPONENT(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), SPI_COMPONENT_TYPE))
#define IS_SPI_COMPONENT_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SPI_COMPONENT_TYPE))

typedef struct {
        BonoboObject parent;
        AtkObject *atko;
} SpiComponent;

typedef struct {
        BonoboObjectClass parent_class;
        POA_Accessibility_Component__epv epv;
} SpiComponentClass;

GType                  accessibility_component_get_type   (void);
SpiComponent              *spi_component_interface_new       (AtkObject *o);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SPI_COMPONENT_H_ */
