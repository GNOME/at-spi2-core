#include "spi-util.h"


int
AccessibleImage_ref (AccessibleImage *obj)
{
  Accessibility_Image_ref (*obj, &ev);
  return 0;
}



int
AccessibleImage_unref (AccessibleImage *obj)
{
  Accessibility_Image_unref (*obj, &ev);
  return 0;
}



char *
AccessibleImage_getImageDescription (AccessibleImage *obj)
{
  return string_from_corba_string (
				   Accessibility_Image_getImageDescription (*obj, &ev));
}



void
AccessibleImage_getImageSize (AccessibleImage *obj,
                              long *width,
                              long *height)
{
  Accessibility_Image_getImageSize (*obj,
				    (CORBA_long *) width, (CORBA_long *) height, &ev);
}



void
AccessibleImage_getImagePosition (AccessibleImage *obj,
                                  long *x,
                                  long *y,
                                  AccessibleCoordType ctype)
{
  Accessibility_Image_getImagePosition (*obj,
					(CORBA_long *) x, (CORBA_long *) y, (CORBA_short) ctype,
					&ev);
}



