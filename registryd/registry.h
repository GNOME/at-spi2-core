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
