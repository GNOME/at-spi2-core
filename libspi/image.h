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

#ifndef SPI_IMAGE_H_
#define SPI_IMAGE_H_

#include <libspi/base.h>
#include <atk/atkimage.h>

G_BEGIN_DECLS

#define SPI_IMAGE_TYPE            (spi_image_get_type ())
#define SPI_IMAGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), SPI_IMAGE_TYPE, SpiImage))
#define SPI_IMAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), SPI_IMAGE_TYPE, SpiImageClass))
#define SPI_IS_IMAGE(obj)         (G_TYPE_CHECK__INSTANCE_TYPE ((obj), SPI_IMAGE_TYPE))
#define SPI_IS_IMAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SPI_IMAGE_TYPE))

typedef struct _Image SpiImage;
typedef struct _ImageClass SpiImageClass;

struct _Image {
  SpiBase parent;
};

struct _ImageClass {
  SpiBaseClass parent;
  POA_Accessibility_Image__epv epv;
};

GType     spi_image_get_type      (void);
SpiImage *spi_image_interface_new (AtkObject *obj);

G_END_DECLS

#endif /* SPI_IMAGE_H_ */
