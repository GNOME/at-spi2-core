/*
 *
 * Basic SPI initialization and event loop function prototypes
 *
 */
#include <string.h>
#include <stdlib.h>
#include <cspi/spi-private.h>

static CORBA_Environment ev = { 0 };
static Accessibility_Registry registry = CORBA_OBJECT_NIL;
static SPIBoolean is_gnome_app = FALSE;
static GSList *live_refs = NULL;

CORBA_Environment *
cspi_ev (void)
{
  /* This method is an ugly hack */
  return &ev;
}

Accessibility_Registry
cspi_registry (void)
{
  return registry;
}

SPIBoolean
cspi_is_gnome_app (void)
{
  return is_gnome_app;	
}

SPIBoolean
cspi_exception (void)
{
  SPIBoolean retval;

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
cspi_object_add (CORBA_Object corba_object)
{
  Accessible *ref;

  if (corba_object != CORBA_OBJECT_NIL)
    {
      ref = g_new (Accessible, 1);

      ref->objref = CORBA_Object_duplicate (corba_object, cspi_ev());
      ref->ref_count = 1;

      live_refs = g_slist_prepend (live_refs, ref);
    }
  else
   {
     ref = NULL;
   }

  return ref;
}

void
cspi_object_ref (Accessible *accessible)
{
  g_return_if_fail (accessible != NULL);

  accessible->ref_count++;
}

void
cspi_object_unref (Accessible *accessible)
{
  g_return_if_fail (accessible != NULL);
	
  if (--accessible->ref_count == 0)
    {
      live_refs = g_slist_remove (live_refs, accessible);

      bonobo_object_release_unref (accessible->objref, cspi_ev ());

      cspi_check_ev (cspi_ev (), "unref");

      memset (accessible, 0xaa, sizeof (Accessible));

      g_free (accessible);
    }
}

static void
cspi_cleanup (void)
{
  GSList *l, *refs;

  refs = live_refs;
  live_refs = NULL;

  for (l = refs; l; l = l->next)
    {
      Accessible *a = l->data;

      g_print ("releasing %p\n", l->data);
      bonobo_object_release_unref (a->objref, NULL);

      g_free (a);
    }

  g_slist_free (refs);

  if (registry != CORBA_OBJECT_NIL)
    {
      bonobo_object_release_unref (registry, NULL);
      registry = CORBA_OBJECT_NIL;
    }
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

  registry = bonobo_activation_activate_from_id (
	  obj_id, 0, NULL, cspi_ev ());

  if (ev._major != CORBA_NO_EXCEPTION)
    {
      g_error ("AT-SPI error: during registry activation: %s\n",
	       bonobo_exception_get_text (cspi_ev ()));
    }

  if (registry == CORBA_OBJECT_NIL)
    {
      g_error ("Could not locate registry");
    }

  Accessibility_Registry_ref (registry, &ev);
  
  bonobo_activate ();
  
  return 0;
}

/**
 * SPI_event_main:
 * @isGNOMEApp: a #SPIBoolean indicating whether the client of the SPI
 *              will use the Gnome event loop or not.  Clients that have
 *              their own GUIS will usually specify #TRUE here, and must
 *              do so if they use Gnome GUI components.
 *
 * Starts/enters the main event loop for the SPI services.
 *
 * (NOTE: This method does not return control, it is exited via a call to SPI_exit()
 * from within an event handler).
 *
 **/
void
SPI_event_main (SPIBoolean isGNOMEApp)
{
  is_gnome_app = isGNOMEApp;
  if (isGNOMEApp)
    {
      g_atexit (cspi_cleanup);
      bonobo_main ();
    }
  else
    {
      /* TODO: install signal handlers to do cleanup */
      CORBA_ORB_run (bonobo_orb (), cspi_ev ());
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
SPIBoolean
SPI_eventIsReady ()
{
  return FALSE;
}

/**
 * SPI_nextEvent:
 * @waitForEvent: a #SPIBoolean indicating whether to block or not.
 *
 * Gets the next event in the SPI event queue; blocks if no event
 * is pending and @waitForEvent is #TRUE.
 * Used by clients that don't wish to use SPI_event_main().
 *
 * Not Yet Implemented.
 *
 * Returns: the next #AccessibleEvent in the SPI event queue.
 **/
AccessibleEvent *
SPI_nextEvent (SPIBoolean waitForEvent)
{
  return NULL;
}

/**
 * SPI_exit:
 *
 * Disconnects from the Accessibility Registry and releases 
 * any floating resources.
 **/
void
SPI_exit (void)
{
  cspi_cleanup();
  if (cspi_is_gnome_app ())
    {
      bonobo_main_quit ();
    }
  else
    {
      CORBA_ORB_shutdown (bonobo_orb (), TRUE, cspi_ev ());	    
    }
  fprintf (stderr, "bye-bye!\n");
}

