/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
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

#ifndef MAGNIFIER_H_
#define MAGNIFIER_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-object.h>
#include "Magnifier.h"
#include "mag_image.h" /* TODO: remove this include */

#define MAGNIFIER_TYPE         (magnifier_get_type ())
#define MAGNIFIER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MAGNIFIER_TYPE, Magnifier))
#define MAGNIFIER_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), MAGNIFIER_TYPE, MagnifierClass))
#define IS_MAGNIFIER(o)       (G_TYPE_CHECK__INSTANCE_TYPE ((o), MAGNIFIER_TYPE))
#define IS_MAGNIFIER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), MAGNIFIER_TYPE))

typedef struct {
        BonoboObject parent;
	MagnifierData *mag_data;
} Magnifier;

typedef struct {
        BonoboObjectClass parent_class;
        POA_Accessibility_Magnifier__epv epv;
} MagnifierClass;

GType                  magnifier_get_type   (void);
Magnifier              *magnifier_new  (int argc, char **argv);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MAGNIFIER_H_ */
