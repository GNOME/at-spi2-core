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

#ifndef VALUE_H_
#define VALUE_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-object.h>
#include <atk/atk.h>
#include <libspi/Accessibility.h>

#define VALUE_TYPE        (value_get_type ())
#define VALUE(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), VALUE_TYPE, Value))
#define ACTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), VALUE_TYPE, ValueClass))
#define IS_VALUE(obj)       (G_TYPE_CHECK__INSTANCE_TYPE ((obj), VALUE_TYPE))
#define IS_VALUE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), VALUE_TYPE))

typedef struct _Value Value;
typedef struct _ValueClass ValueClass;

struct _Value {
  BonoboObject parent;
  AtkValue *atk_value;
};

struct _ValueClass {
  BonoboObjectClass parent_class;
  POA_Accessibility_Value__epv epv;
};

GType
value_get_type   (void);

Value *
value_new       (AtkValue *value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* VALUE_H_ */
