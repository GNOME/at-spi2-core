#include <cspi/spi-private.h>

/**
 * AccessibleImage_ref:
 * @obj: a pointer to the #AccessibleImage implementor on which to operate.
 *
 * Increment the reference count for an #AccessibleImage object.
 *
 * Returns: (no return code implemented yet).
 *
 **/
void
AccessibleImage_ref (AccessibleImage *obj)
{
  cspi_object_ref (obj);
}

/**
 * AccessibleImage_unref:
 * @obj: a pointer to the #AccessibleImage implementor on which to operate.
 *
 * Decrement the reference count for an #AccessibleImage object.
 *
 * Returns: (no return code implemented yet).
 *
 **/
void
AccessibleImage_unref (AccessibleImage *obj)
{
  cspi_object_unref (obj);
}


/**
 * AccessibleImage_getImageDescription:
 * @obj: a pointer to the #AccessibleImage implementor on which to operate.
 *
 * Get the description of the image displayed in an #AccessibleImage object.
 *
 * Returns: a UTF-8 string describing the image.
 *
 **/
char *
AccessibleImage_getImageDescription (AccessibleImage *obj)
{
  return (char *)
    Accessibility_Image__get_imageDescription (CSPI_OBJREF (obj), cspi_ev ());
}



/**
 * AccessibleImage_getImageSize:
 * @obj: a pointer to the #AccessibleImage to query.
 * @width: a pointer to a #long into which the x extents (width) will be returned.
 * @height: a pointer to a #long into which the y extents (height) will be returned.
 *
 * Get the size of the image displayed in a specified #AccessibleImage object.
 *
 **/
void
AccessibleImage_getImageSize (AccessibleImage *obj,
                              long int *width,
                              long int *height)
{
  Accessibility_Image_getImageSize (CSPI_OBJREF (obj),
				    (CORBA_long *) width, (CORBA_long *) height, cspi_ev ());
}



/**
 * AccessibleImage_getImagePosition:
 * @obj: a pointer to the #AccessibleImage implementor to query.
 * @x: a pointer to a #long into which the minimum x coordinate will be returned.
 * @y: a pointer to a #long into which the minimum y coordinate will be returned.
 * @ctype: the desired coordinate system into which to return the results,
 *         (e.g. SPI_COORD_TYPE_WINDOW, SPI_COORD_TYPE_SCREEN).
 *
 * Get the minimum x and y coordinates of the image displayed in a
 *         specified #AccessibleImage implementor.
 *
 **/
void
AccessibleImage_getImagePosition (AccessibleImage *obj,
                                  long *x,
                                  long *y,
                                  AccessibleCoordType ctype)
{
  Accessibility_Image_getImagePosition (CSPI_OBJREF (obj),
					(CORBA_long *) x, (CORBA_long *) y, (CORBA_short) ctype,
					cspi_ev ());
}

/**
 * AccessibleImage_getImageExtents:
 * @obj: a pointer to the #AccessibleImage implementor to query.
 * @x: a pointer to a #long into which the minimum x coordinate will be returned.
 * @y: a pointer to a #long into which the minimum y coordinate will be returned.
 * @width: a pointer to a #long into which the image x extent will be returned.
 * @height: a pointer to a #long into which the image y extent will be returned.
 * @ctype: the desired coordinate system into which to return the results,
 *         (e.g. SPI_COORD_TYPE_WINDOW, SPI_COORD_TYPE_SCREEN).
 *
 * Get the bounding box of the image displayed in a
 *         specified #AccessibleImage implementor.
 *
 **/
void
AccessibleImage_getImageExtents (AccessibleImage *obj,
				 long *x,
				 long *y,
				 long *width,
				 long *height,
				 AccessibleCoordType ctype)
{
  Accessibility_BoundingBox bbox;	
  bbox = Accessibility_Image_getImageExtents (CSPI_OBJREF (obj),
					      (CORBA_short) ctype,
					      cspi_ev ());
  *x = bbox.x;
  *y = bbox.y;
  *width = bbox.width;
  *height = bbox.height;
}
