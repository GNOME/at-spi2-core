/* AT-SPI -  Assistive Technology Service Provider Interface
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

#ifndef SPI_TEXT_H_
#define SPI_TEXT_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-object.h>
#include <atk/atk.h>
#include <libspi/Accessibility.h>
#include "accessible.h"

#define SPI_TEXT_TYPE        (accessibility_text_get_type ())
#define SPI_TEXT(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), SPI_TEXT_TYPE, SpiText))
#define SPI_TEXT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), SPI_TEXT_TYPE, SpiTextClass))
#define IS_TEXT(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SPI_TEXT_TYPE))
#define IS_TEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SPI_TEXT_TYPE))

typedef struct _Text SpiText;
typedef struct _TextClass SpiTextClass;

struct _Text {
  BonoboObject parent;
  AtkObject *atko;
};

struct _TextClass {
  BonoboObjectClass parent_class;
  POA_Accessibility_Text__epv epv;
};

GType
accessibility_text_get_type   (void);

SpiText *
spi_text_interface_new       (AtkObject *obj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SPI_TEXT_H_ */
