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

#ifndef ACCESSIBLE_H_
#define ACCESSIBLE_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-object.h>
#include <atk/atkobject.h>
#include <libspi/Accessibility.h>

#define ACCESSIBLE_TYPE        (accessible_get_type ())
#define ACCESSIBLE(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), ACCESSIBLE_TYPE, Accessible))
#define ACCESSIBLE_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), ACCESSIBLE_TYPE, AccessibleClass))
#define IS_ACCESSIBLE(o)       (G_TYPE_CHECK__INSTANCE_TYPE ((o), ACCESSIBLE_TYPE))
#define IS_ACCESSIBLE_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), ACCESSIBLE_TYPE))

typedef struct {
        BonoboObject parent;
        AtkObject *atko;
} Accessible;

typedef struct {
        BonoboObjectClass parent_class;
        POA_Accessibility_Accessible__epv epv;
} AccessibleClass;

GType                  accessible_get_type   (void);
Accessible             *accessible_new       (AtkObject *o);

#include "component.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ACCESSIBLE_H_ */
