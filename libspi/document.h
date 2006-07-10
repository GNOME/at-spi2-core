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

#ifndef SPI_DOCUMENT_H_
#define SPI_DOCUMENT_H_

#include <libspi/base.h>
#include <atk/atkdocument.h>

G_BEGIN_DECLS

#define SPI_DOCUMENT_TYPE         (spi_document_get_type ())
#define SPI_DOCUMENT(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), SPI_DOCUMENT_TYPE, SpiDocument))
#define SPI_DOCUMENT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SPI_DOCUMENT_TYPE, SpiDocumentClass))
#define IS_DOCUMENT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj, SPI_DOCUMENT_TYPE))
#define IS_DOCUMENT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SPI_DOCUMENT_TYPE))

typedef struct _SpiDocument SpiDocument;
typedef struct _SpiDocumentClass SpiDocumentClass;

struct _SpiDocument {
  SpiBase parent;
};

struct _SpiDocumentClass {
  SpiBaseClass parent_class;
  POA_Accessibility_Document__epv epv;
};

GType        spi_document_get_type       (void);
void         spi_document_construct      (SpiDocument *document,
				          AtkObject *obj);
SpiDocument *spi_document_interface_new  (AtkObject *obj);

G_END_DECLS

#endif /* SPI_DOCUMENT_H_ */
