/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#ifndef ACCESSIBLE_H_
#define ACCESSIBLE_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-xobject.h>
#include <atk/atkobject.h>
#include <Accessible.h>

#define ACCESSIBLE_TYPE        (accessible_get_type ())
#define ACCESSIBLE(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), ACCESSIBLE_TYPE, Accessible))
#define ACCESSIBLE_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), ACCESSIBLE_TYPE, AccessibleClass))
#define IS_ACCESSIBLE(o)       (G_TYPE_CHECK__INSTANCE_TYPE ((o), ACCESSIBLE_TYPE))
#define IS_ACCESSIBLE_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), ACCESSIBLE_TYPE))

typedef struct {
        BonoboXObject parent;
        AtkObject *atko;
} Accessible;

typedef struct {
        BonoboXObjectClass parent_class;
        POA_Accessibility_Accessible__epv epv;
} AccessibleClass;

GType                  accessible_get_type   (void);
Accessible             *accessible_new       (AtkObject *o);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ACCESSIBLE_H_ */
