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

#ifndef TABLE_H_
#define TABLE_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-object.h>
#include <atk/atk.h>
#include <libspi/Accessibility.h>

#define TABLE_TYPE        (table_get_type ())
#define TABLE(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), TABLE_TYPE, Table))
#define TABLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), TABLE_TYPE, TableClass))
#define IS_TABLE(obj)       (G_TYPE_CHECK__INSTANCE_TYPE ((obj), TABLE_TYPE))
#define IS_TABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TABLE_TYPE))

typedef struct _Table Table;
typedef struct _TableClass TableClass;

struct _Table {
  BonoboObject parent;
  AtkObject *atko;
};

struct _TableClass {
  BonoboObjectClass parent_class;
  POA_Accessibility_Table__epv epv;
};

GType
table_get_type   (void);

Table *
table_interface_new       (AtkObject *obj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TABLE_H_ */
