/* #include <libbonobo.h> */
#include <stdio.h>
#include "spi.h"

static CORBA_Environment ev;
static Accessibility_Registry registry;

Accessible *
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
  CORBA_exception_init(&ev);
  CORBA_Object oclient;
  char *obj_id;

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

/*
 *
 * Global functions serviced by the registry
 *
 */

boolean
RegisterGlobalEventListener (AccessibleEventListener *listener,
                             char *eventType)
{
  Accessibility_Registry_registerGlobalEventListener (
                         registry,
                         *listener,
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
GetDesktopCount ()
{
  return Accessibility_Registry_getDesktopCount (registry, &ev);
}

Accessible
*getDesktop (int n)
{
  return Obj_Add (Accessibility_Registry_getDesktop (registry, n, &ev));
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
  return Accessibility_Accessible_ref (*obj, &ev);
}


int
Accessible_unref (Accessible *obj)
{
  return Accessibility_Accessible_unref (*obj, &ev);
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
  return Accessibility_Accessible_getChildCount (*obj, &ev);
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


/*
 *
 * AccessibleApplication function prototypes
 *
 */

int
AccessibleApplication_ref (AccessibleApplication *obj)
{
  return Accessibility_Application_ref (*obj, &ev);
}

int
AccessibleApplication_unref (AccessibleApplication *obj)
{
  return Accessibility_Application_unref (*obj, &ev);
}

char *
AccessibleApplication_getToolkitName (AccessibleApplication *obj)
{
  return Accessibility_Application__getToolkitName (*obj, &ev);
}

char *
AccessibleApplication_getVersion (AccessibleApplication *obj)
{
  return Accessibility_Application__getVersion (*obj, &ev);
}

long
AccessibleApplication_getID (AccessibleApplication *obj)
{
  return Accessibility_Application__getID (*obj, &ev);
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

