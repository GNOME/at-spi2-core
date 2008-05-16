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

#ifndef SPI_RELATION_H_
#define SPI_RELATION_H_

#include <libspi/base.h>
#include <atk/atkrelation.h>

G_BEGIN_DECLS

#define SPI_RELATION_TYPE            (spi_relation_get_type ())
#define SPI_RELATION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), SPI_RELATION_TYPE, SpiRelation))
#define SPI_RELATION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), SPI_RELATION_TYPE, SpiRelationClass))
#define SPI_IS_RELATION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SPI_RELATION_TYPE))
#define SPI_IS_RELATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SPI_RELATION_TYPE))

typedef struct _SpiRelation SpiRelation;
typedef struct _SpiRelationClass SpiRelationClass;

struct _SpiRelation {
  SpiBase parent;
};

struct _SpiRelationClass {
  SpiBaseClass parent_class;
  POA_Accessibility_Relation__epv epv;
};

GType        spi_relation_get_type (void);
SpiRelation *spi_relation_new      (AtkRelation *relation);

G_END_DECLS

#endif /* SPI_RELATION_H_ */
