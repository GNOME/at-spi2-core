#include <stdio.h>
#include <stdlib.h>
#include <libbonobo.h>
#include "Magnifier.h"
#include "mag_client.h"

static CORBA_Environment ev;

int main(int argc, char ** argv){

  if (!bonobo_init (&argc, argv))
  {
    g_error ("Could not initialize Bonobo");
  }

  CORBA_exception_init (&ev);
  
  if(argc == 1){
	  get_magnifier ();
  }

  else {
	  printf ("setting mag factor to %f\n", (float) atof (argv[1]));
	  magnifier_set_magnification (0, (float) atof (argv[1]),
				       (float) atof (argv[1]));
  }
  sleep (4);
  return 0;
}

Accessibility_Magnifier 
get_magnifier()
{
  static Accessibility_Magnifier magnifier = NULL;
  static gboolean is_error = FALSE;
  CORBA_Object oclient;
  char *obj_id;
  
  if (!magnifier && !is_error)
  {
    CORBA_exception_init (&ev);
    obj_id = "OAFIID:Accessibility_Util_Magnifier:proto0.1";

    oclient = bonobo_activation_activate_from_id (obj_id, 0, NULL, &ev);
    if (ev._major != CORBA_NO_EXCEPTION) {
      fprintf (stderr,
            ("Activation error: during magnifier activation: %s\n"),
            CORBA_exception_id(&ev));
      CORBA_exception_free(&ev);
      is_error = TRUE;
    }

    if (CORBA_Object_is_nil (oclient, &ev))
    {
      g_error ("Could not locate magnifier");
      is_error = TRUE;
    }

    magnifier = (Accessibility_Magnifier) oclient;

    /* bonobo_activate (); ? */
  }

  return magnifier;
}

void
magnifier_set_roi(int zoom_region, int x, int y, int w, int h)
{
  Accessibility_Magnifier magnifier = get_magnifier();

  if (magnifier)
       Accessibility_Magnifier_setROI (magnifier,
				      (const CORBA_short) zoom_region,
				      (const CORBA_long) x,
				      (const CORBA_long) y,
				      (const CORBA_long) x+w,
				      (const CORBA_long) y+h,
				      &ev);
}

void
magnifier_set_magnification (int zoom_region, float mag_factor_x, float mag_factor_y)
{
  Accessibility_Magnifier magnifier = get_magnifier();

  if (magnifier)
       Accessibility_Magnifier_setMagFactor (magnifier,
					     (const CORBA_short) zoom_region,
					     (const CORBA_float) mag_factor_x,
					     (const CORBA_float) mag_factor_y,
					     &ev);
}

