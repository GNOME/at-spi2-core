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

#ifndef SPI_EDITABLE_TEXT_H_
#define SPI_EDITABLE_TEXT_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-object.h>
#include <atk/atk.h>
#include <libspi/Accessibility.h>
#include "text.h"

#define SPI_EDITABLE_TEXT_TYPE        (spi_editable_text_get_type ())
#define SPI_EDITABLE_TEXT(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), SPI_EDITABLE_TEXT_TYPE, SpiEditableText))
#define SPI_EDITABLE_TEXT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), SPI_EDITABLE_TEXT_TYPE, SpiEditableText))
#define IS_SPI_EDITABLE_TEXT(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SPI_EDITABLE_TEXT_TYPE))
#define IS_SPI_EDITABLE_TEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SPI_EDITABLE_TEXT_TYPE))

typedef struct _EditableText SpiEditableText;
typedef struct _EditableTextClass SpiEditableTextClass;

struct _EditableText {
  SpiText parent;
};

struct _EditableTextClass {
  SpiTextClass parent_class;
  POA_Accessibility_EditableText__epv epv;
};

GType
spi_editable_text_get_type   (void);

SpiEditableText *
spi_editable_text_interface_new       ( AtkObject *obj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SPI_EDITABLE_TEXT_H_ */
