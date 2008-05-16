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

#ifndef SPI_HYPERLINK_H_
#define SPI_HYPERLINK_H_

#include <libspi/base.h>
#include <atk/atkhyperlink.h>
#include <atk/atkhyperlinkimpl.h>

G_BEGIN_DECLS

#define SPI_HYPERLINK_TYPE        (spi_hyperlink_get_type ())
#define SPI_HYPERLINK(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), SPI_HYPERLINK_TYPE, SpiHyperlink))
#define SPI_HYPERLINK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), SPI_HYPERLINK_TYPE, SpiHyperlinkClass))
#define SPI_IS_HYPERLINK(obj)       (G_TYPE_CHECK__INSTANCE_TYPE ((obj), SPI_HYPERLINK_TYPE))
#define SPI_IS_HYPERLINK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SPI_HYPERLINK_TYPE))

typedef struct _Hyperlink      SpiHyperlink;
typedef struct _HyperlinkClass SpiHyperlinkClass;

struct _Hyperlink {
  SpiBase parent;
};

struct _HyperlinkClass {
  SpiBaseClass parent_class;
  POA_Accessibility_Hyperlink__epv epv;
};

GType         spi_hyperlink_get_type (void);
SpiHyperlink *spi_hyperlink_new      (AtkHyperlink *object);

G_END_DECLS

#endif /* SPI_HYPERLINK_H_ */
