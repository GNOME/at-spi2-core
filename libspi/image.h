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

#ifndef IMAGE_H_
#define IMAGE_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-object.h>
#include <atk/atk.h>
#include <libspi/Accessibility.h>

#define IMAGE_TYPE        (image_get_type ())
#define IMAGE(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), IMAGE_TYPE, Image))
#define IMAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), IMAGE_TYPE, ImageClass))
#define IS_IMAGE(obj)       (G_TYPE_CHECK__INSTANCE_TYPE ((obj), IMAGE_TYPE))
#define IS_IMAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), IMAGE_TYPE))

typedef struct _Image Image;
typedef struct _ImageClass ImageClass;

struct _Image {
  BonoboObject parent;
  AtkObject *atko;
};

struct _ImageClass {
  BonoboObjectClass parent_class;
  POA_Accessibility_Image__epv epv;
};

GType
image_get_type   (void);

Image *
image_interface_new       (AtkObject *obj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* IMAGE_H_ */
