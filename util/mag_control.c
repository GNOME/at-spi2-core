#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

  else
    {
      switch (*argv[1])
        {
	case 'z':	
	  printf ("setting mag factor to %f\n", (float) atof (argv[1]+1));
	  magnifier_set_magnification (0, (float) atof (argv[1]+1),
				       (float) atof (argv[1]+1));
	  break;
	case 's':
	  printf ("resizing region 0 to 100x100 at (600, 0)\n");
	  magnifier_resize_region (0, 600, 0, 700, 100);
	  break;
	case 'd':
	  printf ("destroying/clearing all regions.\n");
	  magnifier_clear_all_regions ();
	  break;
 	case 'q':
	  printf ("exiting magnifier.\n");
	  magnifier_exit ();
	  break;
	case 'c':
	  printf ("creating 3x region at 100,100; 300x200\n");
	  magnifier_create_region (3.0, 3.0, 100, 100, 400, 300);
        }
    }
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
magnifier_resize_region (int zoom_region, int x1, int y1, int x2, int y2)
{
  Accessibility_Magnifier magnifier = get_magnifier();

  if (magnifier)
       Accessibility_Magnifier_resizeZoomRegion (magnifier,
						 (const CORBA_short) zoom_region,
						 (const CORBA_long) x1,
						 (const CORBA_long) y1,
						 (const CORBA_long) x2,
						 (const CORBA_long) y2,
						 &ev);
}

void
magnifier_clear_all_regions ()
{
  Accessibility_Magnifier magnifier = get_magnifier();

  if (magnifier)
       Accessibility_Magnifier_clearAllZoomRegions (magnifier,
						    &ev);
}

int
magnifier_create_region (float zx, float zy, int x1, int y1, int x2, int y2)
{
  Accessibility_Magnifier magnifier = get_magnifier();
  int retval = -1;
  
  if (magnifier)
       retval = Accessibility_Magnifier_createZoomRegion (magnifier,
							  (const CORBA_float) zx,
							  (const CORBA_float) zy,
							  (const CORBA_long) x1,
							  (const CORBA_long) y1,
							  (const CORBA_long) x2,
							  (const CORBA_long) y2,
							  &ev);
  return retval;
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

void
magnifier_exit (void)
{
  Accessibility_Magnifier magnifier = get_magnifier();
  if (magnifier)
    Accessibility_Magnifier_exit (magnifier, &ev);
}
