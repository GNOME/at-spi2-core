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

#ifndef SELECTION_H_
#define SELECTION_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-object.h>
#include <atk/atk.h>
#include <libspi/Accessibility.h>

#define SELECTION_TYPE        (selection_get_type ())
#define SELECTION(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), SELECTION_TYPE, Selection))
#define SELECTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), SELECTION_TYPE, SelectionClass))
#define IS_SELECTION(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SELECTION_TYPE))
#define IS_SELECTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SELECTION_TYPE))

typedef struct _Selection Selection;
typedef struct _SelectionClass SelectionClass;

struct _Selection {
  BonoboObject parent;
  AtkObject *atko;
};

struct _SelectionClass {
  BonoboObjectClass parent_class;
  POA_Accessibility_Selection__epv epv;
};

GType
selection_get_type   (void);

Selection *
selection_interface_new       (AtkObject *obj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SELECTION_H_ */
