/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#ifndef DESKTOP_H_
#define DESKTOP_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-xobject.h>
#include <atk/atkobject.h>
#include <atksimpleobject.h>
#include <accessible.h>
#include <application.h>
#include <Desktop.h>

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
