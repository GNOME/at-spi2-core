#ifndef LISTENER_H_
#define LISTENER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-xobject.h>
#include <atk/atkobject.h>
#include <Event.h>

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
