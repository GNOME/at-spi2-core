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

#ifndef EDITABLE_TEXT_H_
#define EDITABLE_TEXT_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-object.h>
#include <atk/atk.h>
#include <libspi/Accessibility.h>

#define EDITABLE_TEXT_TYPE        (editable_text_get_type ())
#define EDITABLE_TEXT(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), EDITABLE_TEXT_TYPE, EditableText))
#define EDITABLE_TEXT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), EDITABLE_TEXT_TYPE, EditableText))
#define IS_EDITABLE_TEXT(obj)       (G_TYPE_CHECK__INSTANCE_TYPE ((obj), EDITABLE_TEXT_TYPE))
#define IS_EDITABLE_TEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EDITABLE_TEXT_TYPE))

typedef struct _EditableText EditableText;
typedef struct _EditableTextClass EditableTextClass;

struct _EditableText {
  BonoboObject parent;
  AtkEditableText *atk_editable_text;
};

struct _EditableTextClass {
  BonoboObjectClass parent_class;
  POA_Accessibility_EditableText__epv epv;
};

GType
editable_text_get_type   (void);

EditableText *
editable_text_new       (AtkEditableText *editable);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* EDITABLE_TEXT_H_ */
