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

#ifndef ACTION_H_
#define ACTION_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-object.h>
#include <atk/atk.h>
#include <libspi/Accessibility.h>

#define ACTION_TYPE        (action_get_type ())
#define ACTION(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), ACTION_TYPE, Action))
#define ACTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), ACTION_TYPE, ActionClass))
#define IS_ACTION(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ACTION_TYPE))
#define IS_ACTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ACTION_TYPE))

typedef struct _Action Action;
typedef struct _ActionClass ActionClass;

struct _Action {
  BonoboObject parent;
  AtkObject *atko;
};

struct _ActionClass {
  BonoboObjectClass parent_class;
  POA_Accessibility_Action__epv epv;
};

GType
action_get_type   (void);

Action *
action_interface_new       (AtkObject *obj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ACTION_H_ */
