int
SpiAccessibleImage_ref (SpiAccessibleImage *obj)
{
  Accessibility_Image_ref (*obj, &ev);
  return 0;
}



int
SpiAccessibleImage_unref (SpiAccessibleImage *obj)
{
  Accessibility_Image_unref (*obj, &ev);
  return 0;
}



char *
SpiAccessibleImage_getImageDescription (SpiAccessibleImage *obj)
{
  return (char *)
    Accessibility_Image__get_imageDescription (*obj, &ev);
    }



void
SpiAccessibleImage_getImageSize (SpiAccessibleImage *obj,
                              long *width,
                              long *height)
{
  Accessibility_Image_getImageSize (*obj,
				    (CORBA_long *) width, (CORBA_long *) height, &ev);
}



void
SpiAccessibleImage_getImagePosition (SpiAccessibleImage *obj,
                                  long *x,
                                  long *y,
                                  SpiAccessibleCoordType ctype)
{
  Accessibility_Image_getImagePosition (*obj,
					(CORBA_long *) x, (CORBA_long *) y, (CORBA_short) ctype,
					&ev);
}



