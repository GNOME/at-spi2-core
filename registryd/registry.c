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

/*
 * registry.c: test for accessibility implementation
 *
 */

#ifdef SPI_DEBUG
#include <stdio.h>
#endif
#include <config.h>
#include <bonobo/Bonobo.h>

/*
 * This pulls the CORBA definitions for the "Accessibility::Registry" server
 */
#include "Registry.h"

/*
 * This pulls the definition for the BonoboObject (GType)
 */
#include "registry.h"

/*
 * Our parent GObject type
 */
#define PARENT_TYPE LISTENER_TYPE

/*
 * A pointer to our parent object class
 */
static ListenerClass *registry_parent_class;

/*
 * Implemented GObject::finalize
 */
static void
registry_object_finalize (GObject *object)
{
/*        Registry *registry = REGISTRY (object); */
        GObjectClass *object_class = G_OBJECT_GET_CLASS( object);

        printf("registry_object_finalize called\n");

        object_class->finalize (object);
}

/**
 * registerApplication:
 * @application: a reference to the requesting @Application
 * return values: void
 *
 * Register a new application with the accessibility broker.
 *
 **/
static void
impl_accessibility_registry_register_application (PortableServer_Servant servant,
                                                  const Accessibility_Application application,
                                                  CORBA_Environment * ev)
{
  Registry *registry = REGISTRY (bonobo_object_from_servant (servant));

#ifdef SPI_DEBUG
  fprintf (stderr, "registering app %p\n", application);
#endif

  registry->desktop->applications = g_list_append (registry->desktop->applications, CORBA_Object_duplicate (application, ev));
  /*
   * TODO: change the implementation below to a WM-aware one;
   * e.g. don't add all apps to the Desktop
   */
  /*  registry->desktop->applications = registry->applications;*/
}

/**
 * deregisterApplication:
 * @application: a reference to the @Application
 * to be deregistered.
 * return values: void
 *
 * De-register an application previously registered with the broker.
 *
 **/
static void
impl_accessibility_registry_deregister_application (PortableServer_Servant servant,
                                                    const Accessibility_Application application,
                                                    CORBA_Environment * ev)
{
  Registry *registry = REGISTRY (bonobo_object_from_servant (servant));
  registry->applications = g_list_remove (registry->applications, application);
}

/*
 * CORBA Accessibility::Registry::registerGlobalEventListener method implementation
 */
static void
impl_accessibility_registry_register_global_event_listener
                                            (PortableServer_Servant  servant,
                                             Accessibility_EventListener listener,
                                             const CORBA_char *event_name,
                                             CORBA_Environment      *ev)
{
  /**
   *  TODO:
   *
   *  distinguish between event types
   *  register with app toolkits only for requested event types
   *  maintain list of requested types and number of listeners
   *  find non-strcmp method of matching event types to listeners
   *
   **/

  Registry *registry = REGISTRY (bonobo_object_from_servant (servant));
  /* fprintf(stderr, "registering %x/%x\n", listener, *listener); */
  registry->listeners = g_list_append (registry->listeners, CORBA_Object_duplicate(listener, ev));
  /* fprintf(stderr, "there are now %d listeners registered.\n", g_list_length(registry->listeners)); */
  /* should use hashtable and CORBA_Object_hash (...) */
}

/*
 * CORBA Accessibility::Registry::deregisterGlobalEventListener method implementation
 */
static void
impl_accessibility_registry_deregister_global_event_listener
                                                   (PortableServer_Servant  servant,
                                                    Accessibility_EventListener listener,
                                                    CORBA_Environment      *ev)
{
  Registry *registry = REGISTRY (bonobo_object_from_servant (servant));
  /* TODO: this won't work since 'listener' is a duplicate ref */
  registry->listeners = g_list_remove (registry->listeners, listener);
  /*  fprintf(stderr, "deregister\n"); */
}


/**
 * getDesktopCount:
 * return values: a short integer indicating the current number of
 * @Desktops.
 *
 * Get the current number of desktops.
 *
 **/
static short
impl_accessibility_registry_get_desktop_count (PortableServer_Servant servant,
                                               CORBA_Environment * ev)
{
  /* TODO: implement support for multiple virtual desktops */
  CORBA_short n_desktops;
  n_desktops = (CORBA_short) 1;
  return n_desktops;
}

/**
 * getDesktop:
 * @n: the index of the requested @Desktop.
 * return values: a reference to the requested @Desktop.
 *
 * Get the nth accessible desktop.
 *
 **/
static Accessibility_Desktop
impl_accessibility_registry_get_desktop (PortableServer_Servant servant,
                                         const CORBA_short n,
                                         CORBA_Environment * ev)
{
  Registry *registry = REGISTRY (bonobo_object_from_servant (servant));

  /* TODO: implement support for multiple virtual desktops */
  if (n == 0)
    {
      return (Accessibility_Desktop)
        CORBA_Object_duplicate (
             bonobo_object_corba_objref (bonobo_object (registry->desktop)), ev);
    }
  else
    {
      return (Accessibility_Desktop) CORBA_OBJECT_NIL;
    }
}

/**
 * getDesktopList:
 * return values: a sequence containing references to
 * the @Desktops.
 *
 * Get a list of accessible desktops.
 *
 **/
static Accessibility_DesktopSeq *
impl_accessibility_registry_get_desktop_list (PortableServer_Servant servant,
                                              CORBA_Environment * ev)
{
  /* TODO: implement support for multiple virtual desktops */
  return (Accessibility_DesktopSeq *) NULL;
}

static CORBA_Object
impl_accessibility_registry_get_device_event_controller (PortableServer_Servant servant,
                                                         CORBA_Environment * ev)
{
  /* TODO: not yet implemented! */
  return CORBA_OBJECT_NIL;
}

static void
impl_registry_notify_event (PortableServer_Servant servant,
                            const Accessibility_Event *e,
                            CORBA_Environment *ev)
{
  int n;
  int len;
  Registry *registry = REGISTRY (bonobo_object_from_servant (servant));

  /**
   *  TODO:
   *
   *  distinguish between event types
   *  find non-strcmp method of matching event types to listeners
   *
   **/

  len = g_list_length (registry->listeners);
  /* fprintf(stderr, "%d listeners registered\n", len); */

  for (n=0; n<len; ++n)
    {
#ifdef SPI_DEBUG
      fprintf(stderr, "notifying listener #%d\n", n);
      fprintf(stderr, "event name %s\n", Accessibility_Accessible__get_name(e->target, ev));
#endif
      Accessibility_EventListener_notifyEvent (
               (Accessibility_EventListener) g_list_nth_data (registry->listeners, n),
               e,
               ev);
    }
}

static void
registry_class_init (RegistryClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        POA_Accessibility_Registry__epv *epv = &klass->epv;

        registry_parent_class = g_type_class_ref (LISTENER_TYPE);

        object_class->finalize = registry_object_finalize;

        epv->registerApplication = impl_accessibility_registry_register_application;
        epv->deregisterApplication = impl_accessibility_registry_deregister_application;
        epv->registerGlobalEventListener = impl_accessibility_registry_register_global_event_listener;
        epv->deregisterGlobalEventListener = impl_accessibility_registry_deregister_global_event_listener;
        epv->getDesktopCount = impl_accessibility_registry_get_desktop_count;
        epv->getDesktop = impl_accessibility_registry_get_desktop;
        epv->getDesktopList = impl_accessibility_registry_get_desktop_list;
        epv->getDeviceEventController = impl_accessibility_registry_get_device_event_controller;
        ((ListenerClass *) klass)->epv.notifyEvent = impl_registry_notify_event;
}

static void
registry_init (Registry *registry)
{
  registry->listeners = NULL;
  registry->applications = NULL;
  registry->desktop = desktop_new();
}

GType
registry_get_type (void)
{
        static GType type = 0;

        if (!type) {
                static const GTypeInfo tinfo = {
                        sizeof (RegistryClass),
                        (GBaseInitFunc) NULL,
                        (GBaseFinalizeFunc) NULL,
                        (GClassInitFunc) registry_class_init,
                        (GClassFinalizeFunc) NULL,
                        NULL, /* class data */
                        sizeof (Registry),
                        0, /* n preallocs */
                        (GInstanceInitFunc) registry_init,
                        NULL /* value table */
                };
                /*
                 *   Here we use bonobo_x_type_unique instead of
                 * gtk_type_unique, this auto-generates a load of
                 * CORBA structures for us. All derived types must
                 * use bonobo_x_type_unique.
                 */
                type = bonobo_x_type_unique (
                        PARENT_TYPE,
                        POA_Accessibility_Registry__init,
                        NULL,
                        G_STRUCT_OFFSET (RegistryClass, epv),
                        &tinfo,
                        "Registry");
        }

        return type;
}

Registry *
registry_new (void)
{
    Registry *retval =
               REGISTRY (g_object_new (registry_get_type (), NULL));
    return retval;
}
