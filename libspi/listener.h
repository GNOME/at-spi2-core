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

#ifndef LISTENER_H_
#define LISTENER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-xobject.h>
#include <atk/atkobject.h>
#include <libspi/Accessibility.h>

#define LISTENER_TYPE        (listener_get_type ())
#define LISTENER(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), LISTENER_TYPE, Listener))
#define LISTENER_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), LISTENER_TYPE, ListenerClass))
#define IS_LISTENER(o)       (G_TYPE_CHECK__INSTANCE_TYPE ((o), LISTENER_TYPE))
#define IS_LISTENER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), LISTENER_TYPE))

typedef struct {
        BonoboXObject parent;
} Listener;

typedef struct {
        BonoboXObjectClass parent_class;
        POA_Accessibility_EventListener__epv epv;
} ListenerClass;

GType               listener_get_type   (void);
Listener            *listener_new       (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LISTENER_H_ */
