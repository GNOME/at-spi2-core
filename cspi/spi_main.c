
/*
 *
 * Basic SPI initialization and event loop function prototypes
 *
 */

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

/**
 * SPI_event_main:
 * @isGNOMEApp: a #boolean indicating whether the client of the SPI
 *              will use the Gnome event loop or not.
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
  if (isGNOMEApp) {
    g_atexit(SPI_exit);
    bonobo_main();
  }
  else {
    /* TODO: install signal handlers to do cleanup */
    CORBA_ORB_run (bonobo_orb(), &ev);
    fprintf (stderr, "orb loop exited...\n");
  }
}

/**
 * SPI_event_is_ready:
 *
 * Checks to see if an SPI event is waiting in the event queue.
 * Used by clients that don't wish to use SPI_event_main().
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
 *
 * Gets the next event in the SPI event queue; blocks if no event
 * is pending.
 * Used by clients that don't wish to use SPI_event_main().
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
 * Not Yet Implemented.
 *
 **/
void
SPI_exit (void)
{
  fprintf (stderr, "bye-bye!\n");	
  exit(0);
}

