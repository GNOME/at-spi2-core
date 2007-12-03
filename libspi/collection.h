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

#ifndef SPI_COLLECTION_H_
#define SPI_COLLECTION_H_

#include <libspi/base.h>

G_BEGIN_DECLS

#define SPI_COLLECTION_TYPE         (spi_collection_get_type ())
#define SPI_COLLECTION(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), SPI_COLLECTION_TYPE, SpiCollection))
#define SPI_COLLECTION_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SPI_COLLECTION_TYPE, SpiCollectionClass))
#define IS_COLLECTION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SPI_COLLECTION_TYPE))
#define IS_COLLECTION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SPI_COLLECTION_TYPE))

typedef struct _MatchRulePrivate MatchRulePrivate;

struct _MatchRulePrivate {

     Accessibility_StateSet states; 
     Accessibility_Collection_MatchType statematchtype;
     Accessibility_AttributeSet  *attributes;
     Accessibility_Collection_MatchType attributematchtype;
     Accessibility_RoleSet *roles;
     Accessibility_Collection_MatchType rolematchtype;
     CORBA_char *interfaces;
     Accessibility_Collection_MatchType interfacematchtype;
     gboolean invert;

};

typedef struct _SpiCollection SpiCollection;
typedef struct _SpiCollectionClass SpiCollectionClass;

struct _SpiCollection {
  SpiBase parent;
  MatchRulePrivate *_mrp;

};

struct _SpiCollectionClass {
  SpiBaseClass parent_class;
  POA_Accessibility_Collection__epv epv;
};

GType        spi_collection_get_type       (void);

void         spi_collection_construct      (SpiCollection *collection);
					    
SpiCollection *spi_collection_interface_new  (AtkObject *obj);

G_END_DECLS

#endif /* SPI_COLLECTION_H_ */


