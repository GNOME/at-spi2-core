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

#ifndef REGISTRY_H_
#define REGISTRY_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <Registry.h>
#include "listener.h"
#include "desktop.h"

#define REGISTRY_TYPE        (registry_get_type ())
#define REGISTRY(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), REGISTRY_TYPE, Registry))
#define REGISTRY_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), REGISTRY_TYPE, RegistryClass))
#define IS_REGISTRY(o)       (G_TYPE_CHECK__INSTANCE_TYPE ((o), REGISTRY_TYPE))
#define IS_REGISTRY_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), REGISTRY_TYPE))

typedef struct {
  Listener parent;
  GList *listeners;
  GList *applications;
  Desktop *desktop;
} Registry;

typedef struct {
        ListenerClass parent_class;
        POA_Accessibility_Registry__epv epv;
} RegistryClass;

GType               registry_get_type   (void);
Registry            *registry_new       (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* REGISTRY_H_ */
