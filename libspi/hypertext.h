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

#ifndef HYPERTEXT_H_
#define HYPERTEXT_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-object.h>
#include <atk/atk.h>
#include <libspi/Accessibility.h>

#define HYPERTEXT_TYPE        (hypertext_get_type ())
#define HYPERTEXT(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYPERTEXT_TYPE, Hypertext))
#define HYPERTEXT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), HYPERTEXT_TYPE, HypertextClass))
#define IS_HYPERTEXT(obj)       (G_TYPE_CHECK__INSTANCE_TYPE ((obj), HYPERTEXT_TYPE))
#define IS_HYPETEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), HYPERTEXT_TYPE))

typedef struct _Hypertext Hypertext;
typedef struct _HypertextClass HypertextClass;

struct _Hypertext {
  BonoboObject parent;
  AtkHypertext *atk_hypertext;
};

struct _HypertextClass {
  BonoboObjectClass parent_class;
  POA_Accessibility_Hypertext__epv epv;
};

GType
hypertext_get_type   (void);

Hypertext *
hypertext_new       (AtkHypertext *hypertext);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HYPERTEXT_H_ */
