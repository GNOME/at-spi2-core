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

#ifndef DESKTOP_H_
#define DESKTOP_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-xobject.h>
#include <atk/atkobject.h>
#include <accessible.h>
#include <application.h>
#include <libspi/Accessibility.h>

#define DESKTOP_TYPE        (desktop_get_type ())
#define DESKTOP(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), DESKTOP_TYPE, Desktop))
#define DESKTOP_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), DESKTOP_TYPE, DesktopClass))
#define IS_DESKTOP(o)       (G_TYPE_CHECK__INSTANCE_TYPE ((o), DESKTOP_TYPE))
#define IS_DESKTOP_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), DESKTOP_TYPE))

typedef struct {
        Accessible parent;
        GList *applications; /* TODO: maybe change this so it's generated on-demand ? */
} Desktop;

typedef struct {
        AccessibleClass parent_class;
        POA_Accessibility_Desktop__epv epv;
} DesktopClass;

GType               desktop_get_type           (void);
void                desktop_add_application    (Application *app);
void                desktop_remove_application (Application *app);
Desktop             *desktop_new               (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DESKTOP_H_ */
