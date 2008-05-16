/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2007 IBM Corp.
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

#ifndef SPI_MATCHRULE_H_
#define SPI_MATCHRULE_H_

#include <libspi/base.h>

G_BEGIN_DECLS

#define SPI_MATCHRULE_TYPE         (spi_matchrule_get_type ())
#define SPI_MATCHRULE(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), SPI_MATCHRULE_TYPE, SpiMatchrule))
#define SPI_MATCHRULE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SPI_MATCHRULE_TYPE, SpiMatchruleClass))
#define IS_MATCHRULE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj, SPI_MATCHRULE_TYPE))
#define IS_MATCHRULE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SPI_MATCHRULE_TYPE))

typedef struct _SpiMatchrule SpiMatchrule;
typedef struct _SpiMatchruleClass SpiMatchruleClass;

struct _SpiMatchrule {
     SpiBase parent;

};

struct _SpiMatchruleClass {
  SpiBaseClass parent_class;
  POA_Accessibility_MatchRule__epv epv;
};

GType        spi_matchrule_get_type       (void);
void         spi_matchrule_construct      (SpiMatchrule *matchrule);
					    
SpiMatchrule *spi_matchrule_interface_new  (void);

G_END_DECLS

#endif /* SPI_MATCHRULE_H_ */
