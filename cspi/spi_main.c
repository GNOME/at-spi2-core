/*
 *
 * Basic SPI initialization and event loop function prototypes
 *
 */
#include <stdlib.h>
#include <libbonobo.h>
#include <cspi/spi-private.h>

static CORBA_Environment ev = { 0 };
static AccessibilityRegistry registry = CORBA_OBJECT_NIL;

CORBA_Environment *
spi_ev (void)
{
  /* This method is an ugly hack */
  return &ev;
}

AccessibilityRegistry
spi_registry (void)
{
  /* This method is an ugly hack */
  return registry;
}

boolean
spi_exception (void)
{
  boolean retval;

  if (BONOBO_EX (&ev))
    {
      CORBA_exception_free (&ev);
      retval = TRUE;
    }
  else
    {
      retval = FALSE;
    }

  return retval;
}

Accessible *
spi_object_add (Accessible corba_object)
{
  /* TODO: keep list of live object refs */
  Accessible *oref = NULL;

  if (corba_object != CORBA_OBJECT_NIL)
    {
      oref = g_malloc (sizeof (Accessible));
      *oref = corba_object;
    }
 
  return oref;
}

/**
 * SPI_init:
 *
 * Connects to the accessibility registry and initializes the SPI.
 *
 * Returns: 0 on success, otherwise an integer error code.
 **/
int
SPI_init (void)
{
  int argc = 0;
  char *obj_id;
  CORBA_Object oclient;
  static gboolean inited = FALSE;

  if (inited)
    {
      return 1;
    }

  inited = TRUE;

  CORBA_exception_init (&ev);

  if (!bonobo_init (&argc, NULL))
    {
      g_error ("Could not initialize Bonobo");
    }

  obj_id = "OAFIID:Accessibility_Registry:proto0.1";

  oclient = bonobo_activation_activate_from_id (
	  obj_id, 0, NULL, spi_ev ());

  if (ev._major != CORBA_NO_EXCEPTION)
    {
      g_error ("AT-SPI error: during registry activation: %s\n",
	       bonobo_exception_get_text (spi_ev ()));
    }

  if (CORBA_Object_is_nil (oclient, spi_ev ()))
    {
      g_error ("Could not locate registry");
    }

  registry = (Accessibility_Registry) oclient;

  bonobo_activate ();

  return 0;
}

/**
 * SPI_event_main:
 * @isGNOMEApp: a #boolean indicating whether the client of the SPI
 *              will use the Gnome event loop or not.  Clients that have
 *              their own GUIS will usually specify #TRUE here, and must
 *              do so if they use Gnome GUI components.
 *
 * Starts/enters the main event loop for the SPI services.
 *
 * (NOTE: This method does not return control, it is exited via a call to exit()
 * from within an event handler).
 *
 **/
void
SPI_event_main (boolean isGNOMEApp)
{
  if (isGNOMEApp)
    {
      g_atexit (SPI_exit);
      bonobo_main ();
    }
  else
    {
      /* TODO: install signal handlers to do cleanup */
      CORBA_ORB_run (bonobo_orb(), spi_ev ());
      fprintf (stderr, "orb loop exited...\n");
    }
}

/**
 * SPI_eventIsReady:
 *
 * Checks to see if an SPI event is waiting in the event queue.
 * Used by clients that don't wish to use SPI_event_main().
 *
 * Not Yet Implemented.
 *
 * Returns: #TRUE if an event is waiting, otherwise #FALSE.
 *
 **/
boolean
SPI_eventIsReady ()
{
  return FALSE;
}

/**
 * SPI_nextEvent:
 * @waitForEvent: a #boolean indicating whether to block or not.
 *
 * Gets the next event in the SPI event queue; blocks if no event
 * is pending and @waitForEvent is #TRUE.
 * Used by clients that don't wish to use SPI_event_main().
 *
 * Not Yet Implemented.
 *
 * Returns: the next #AccessibleEvent in the SPI event queue.
 *
 **/
AccessibleEvent *
SPI_nextEvent (boolean waitForEvent)
{
  return NULL;
}

/**
 * SPI_exit:
 *
 * Disconnects from the Accessibility Registry and releases resources.
 *
 **/
void
SPI_exit (void)
{
  fprintf (stderr, "bye-bye!\n");	
  exit(0);
}

