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


#include <stdlib.h>
#include <libbonobo.h>
#include "Magnifier.h"
#include "mag_client.h"

static CORBA_Environment ev;

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
					     ((CORBA_float) mag_factor_x),
					     ((CORBA_float) mag_factor_y),
					     &ev);
}

