/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#ifndef APPLICATION_H_
#define APPLICATION_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <atk/atkobject.h>
#include <accessible.h>
#include <application.h>
#include <Application.h>

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
