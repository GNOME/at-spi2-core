#include <libbonobo.h>
#include <stdio.h>
#include "spi.h"

static CORBA_Environment ev;
static AccessibilityRegistry registry;

static Accessible *
Obj_Add (Accessible object)
{
  /* TODO: keep list of live object refs */
  Accessible *oref = g_malloc (sizeof (Accessible));
  *oref = object;
  return oref;
}

/*
 *
 * Basic SPI initialization and event loop function prototypes
 *
 */

int
SPI_init (void)
{
  int argc = 0;
  CORBA_Object oclient;
  char *obj_id;

  CORBA_exception_init(&ev);

  if (!bonobo_init (&argc, NULL))
    {
      g_error ("Could not initialize Bonobo");
    }

  obj_id = "OAFIID:Accessibility_Registry:proto0.1";

  oclient = bonobo_activation_activate_from_id (obj_id, 0, NULL, &ev);
  if (ev._major != CORBA_NO_EXCEPTION) {
    fprintf (stderr,
            ("AT-SPI error: during registry activation: %s\n"),
            CORBA_exception_id(&ev));
    CORBA_exception_free(&ev);
    exit(-1);
  }

  if (CORBA_Object_is_nil (oclient, &ev))
    {
      g_error ("Could not locate registry");
      exit(-1);
    }

  registry = (Accessibility_Registry) oclient;

  bonobo_activate ();

  return 0;
}

void
SPI_event_main (boolean isGNOMEApp)
{
  if (isGNOMEApp) bonobo_main();
  else CORBA_ORB_run (bonobo_orb(), &ev);
}

/* Not Yet Implemented */
boolean
SPI_eventIsReady ()
{
  return FALSE;
}

/* Not Yet Implemented */
AccessibleEvent *
SPI_nextEvent (boolean waitForEvent)
{
  return NULL;
}

void
SPI_exit (void)
{
  exit(0);
}

AccessibleEventListener *
createEventListener (AccessibleEventListenerCB callback)
{
  AccessibleEventListener *listener = accessible_event_listener_new ();
  if (callback)
    {
      accessible_event_listener_add_callback (listener, callback);
    }
  return listener;
}

boolean
EventListener_addCallback (AccessibleEventListener *listener,
                           AccessibleEventListenerCB callback)
{
  accessible_event_listener_add_callback (listener, callback);
  return TRUE;
}

boolean
EventListener_removeCallback (AccessibleEventListener *listener,
                           AccessibleEventListenerCB callback)
{
  accessible_event_listener_remove_callback (listener, callback);
  return TRUE;
}

/*
 *
 * Global functions serviced by the registry
 *
 */

boolean
registerGlobalEventListener (AccessibleEventListener *listener,
                             char *eventType)
{
  Accessibility_Registry_registerGlobalEventListener (
                         registry,
                         (Accessibility_EventListener)
                            bonobo_object_corba_objref (bonobo_object (listener)),
                         eventType,
                         &ev);

  if (ev._major != CORBA_NO_EXCEPTION)
    {
    return FALSE;
    }
  else
    {
      return TRUE;
    }
}

int
getDesktopCount ()
{
  return Accessibility_Registry_getDesktopCount (registry, &ev);
}

Accessible*
getDesktop (int n)
{
  return Obj_Add (Accessibility_Registry_getDesktop (registry, (CORBA_short) n, &ev));
}

int
getDesktopList (Accessible **list)
{
  *list = NULL;
  return 0;
}

/* Not Yet Implemented */
void
registerKeystrokeListener (KeystrokeListener *listener)
{
  ;
}

/* Not Yet Implemented */
void
generateKeyEvent (long keyCode, long meta)
{
  ;
}

/* Not Yet Implemented */
void
generateMouseEvent (long x, long y, char *name)
{
  ;
}

/*
 *
 * Accessible function prototypes
 *
 */

int
Accessible_ref (Accessible *obj)
{
  Accessibility_Accessible_ref (*obj, &ev);
  return 0;
}


int
Accessible_unref (Accessible *obj)
{
  Accessibility_Accessible_unref (*obj, &ev);
  return 0;
}

char *
Accessible_getName (Accessible *obj)
{
  return Accessibility_Accessible__get_name (*obj, &ev);
}

char *
Accessible_getDescription (Accessible *obj)
{
  return Accessibility_Accessible__get_description (*obj, &ev);
}

Accessible *
Accessible_getParent (Accessible *obj)
{
  return Obj_Add (Accessibility_Accessible__get_parent (*obj, &ev));
}

long
Accessible_getChildCount (Accessible *obj)
{
  return Accessibility_Accessible__get_childCount (*obj, &ev);
}

Accessible *
Accessible_getChildAtIndex (Accessible *obj,
                            long childIndex)
{
  return Obj_Add (Accessibility_Accessible_getChildAtIndex (*obj, childIndex, &ev));
}

long
Accessible_getIndexInParent (Accessible *obj)
{
  return Accessibility_Accessible_getIndexInParent (*obj, &ev);
}

/* Not Yet Implemented */
AccessibleRelation **
Accessible_getRelationSet (Accessible *obj)
{
  return NULL;
}

/* Not Yet Implemented */
char *
Accessible_getRole (Accessible *obj)
{
  return "";
}

/* Not Yet Implemented */
AccessibleStateSet *
Accessible_getStateSet (Accessible *obj)
{
  return NULL;
}

/* Interface query methods */

boolean
Accessible_isComponent (Accessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Component:1.0",
                                             &ev);
  return (iface != NULL) ? TRUE : FALSE;
}

AccessibleComponent *
Accessible_getComponent (Accessible *obj)
{
  AccessibleComponent iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Component:1.0",
                                             &ev);
  return Obj_Add (iface);
}

GenericInterface *
Accessible_queryInterface (Accessible *obj, char *interface_name)
{
  GenericInterface iface;
  iface = Accessibility_Accessible_queryInterface (*obj,
                                                    interface_name,
                                                    &ev);
  return (iface != NULL) ? Obj_Add (iface) : NULL;
}

/*
 *
 * AccessibleApplication function prototypes
 *
 */

int
AccessibleApplication_ref (AccessibleApplication *obj)
{
  Accessibility_Application_ref (*obj, &ev);
  return 0;
}

int
AccessibleApplication_unref (AccessibleApplication *obj)
{
  Accessibility_Application_unref (*obj, &ev);
  return 0;
}

char *
AccessibleApplication_getToolkitName (AccessibleApplication *obj)
{
  return Accessibility_Application__get_toolkitName (*obj, &ev);
}

char *
AccessibleApplication_getVersion (AccessibleApplication *obj)
{
  return Accessibility_Application__get_version (*obj, &ev);
}

long
AccessibleApplication_getID (AccessibleApplication *obj)
{
  return Accessibility_Application__get_id (*obj, &ev);
}

/* Not Yet Implemented */
boolean
AccessibleApplication_pause (AccessibleApplication *obj)
{
  return FALSE;
}

/* Not Yet Implemented */
boolean
AccessibleApplication_resume (AccessibleApplication *obj)
{
  return FALSE;
}

/*
 *
 * AccessibleComponent function implementations
 *
 */

int
AccessibleComponent_ref (AccessibleComponent *obj)
{
  Accessibility_Component_ref (*obj, &ev);
  return 0;
}

int
AccessibleComponent_unref (AccessibleComponent *obj)
{
  Accessibility_Component_unref (*obj, &ev);
  return 0;
}

boolean
AccessibleComponent_contains (AccessibleComponent *obj,
                              long x,
                              long y,
                              AccessibleCoordType ctype)
{
  return Accessibility_Component_contains (*obj,
                                           (CORBA_long) x,
                                           (CORBA_long) y,
                                           ctype,
                                           &ev);
}

Accessible *
AccessibleComponent_getAccessibleAtPoint (AccessibleComponent *obj,
                                          long x,
                                          long y,
                                          AccessibleCoordType ctype)
{
  Accessible child;
  child = Accessibility_Component_getAccessibleAtPoint(*obj,
                                                       (CORBA_long) x,
                                                       (CORBA_long) y,
                                                       ctype,
                                                       &ev);
  return (child != NULL) ? Obj_Add (child) : NULL;
}

void
AccessibleComponent_getExtents (AccessibleComponent *obj,
                                long *x,
                                long *y,
                                long *width,
                                long *height,
                                AccessibleCoordType ctype)
{
  /* TODO: remove assumption that CORBA_long == long in typecast */
  Accessibility_Component_getExtents (*obj,
                                     (CORBA_long *) x,
                                     (CORBA_long *) y,
                                     (CORBA_long *) width,
                                     (CORBA_long *) height,
                                     ctype,
                                     &ev);
}

void
AccessibleComponent_getPosition (AccessibleComponent *obj,
                                 long *x,
                                 long *y,
                                 AccessibleCoordType ctype)
{
  Accessibility_Component_getPosition (*obj,
                                       (CORBA_long *) x,
                                       (CORBA_long *) y,
                                       ctype,
                                       &ev);
}

void
AccessibleComponent_getSize (AccessibleComponent *obj,
                             long *width,
                             long *height)
{
  Accessibility_Component_getSize (*obj,
                                   (CORBA_long *) width,
                                   (CORBA_long *) height,
                                   &ev);
}

/* Not Yet Implemented */
void
AccessibleComponent_grabFocus (AccessibleComponent *obj)
{
  ;
}
