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

#ifndef APPLICATION_H_
#define APPLICATION_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <atk/atkobject.h>
#include <accessible.h>
#include <application.h>
#include <libspi/Accessibility.h>

#define APPLICATION_TYPE        (application_get_type ())
#define APPLICATION(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), APPLICATION_TYPE, Application))
#define APPLICATION_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), APPLICATION_TYPE, ApplicationClass))
#define IS_APPLICATION(o)       (G_TYPE_CHECK__INSTANCE_TYPE ((o), APPLICATION_TYPE))
#define IS_APPLICATION_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), APPLICATION_TYPE))

typedef struct {
        Accessible parent;
        char *id;
} Application;

typedef struct {
        AccessibleClass parent_class;
        POA_Accessibility_Application__epv epv;
} ApplicationClass;

GType               application_get_type           (void);
Application         *application_new               (char *name, char *desc, char *id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* APPLICATION_H_ */
