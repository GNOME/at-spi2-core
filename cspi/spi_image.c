#include <cspi/spi-private.h>

/**
 * AccessibleImage_ref:
 * @obj: a pointer to the #AccessibleImage implementor on which to operate.
 *
 * Increment the reference count for an #AccessibleImage object.
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
 **/
char *
AccessibleImage_getImageDescription (AccessibleImage *obj)
{
  char *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  retval = 
    Accessibility_Image__get_imageDescription (CSPI_OBJREF (obj),
					       cspi_ev ());

  cspi_return_val_if_ev ("getImageDescription", NULL);

  return NULL;
}

/**
 * AccessibleImage_getImageSize:
 * @obj: a pointer to the #AccessibleImage to query.
 * @width: a pointer to a #long into which the x extents (width) will be returned.
 * @height: a pointer to a #long into which the y extents (height) will be returned.
 *
 * Get the size of the image displayed in a specified #AccessibleImage object.
 **/
void
AccessibleImage_getImageSize (AccessibleImage *obj,
                              long int *width,
                              long int *height)
{
  CORBA_long w, h;

  cspi_return_if_fail (obj != NULL);

  Accessibility_Image_getImageSize (CSPI_OBJREF (obj),
				    &w, &h, cspi_ev ());

  if (!cspi_check_ev ("getImageSize"))
    {
      *width = 0;
      *height = 0;
    }
  else
    {
      *width = w;
      *height = h;
    }
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
 **/
void
AccessibleImage_getImagePosition (AccessibleImage *obj,
                                  long *x,
                                  long *y,
                                  AccessibleCoordType ctype)
{
  CORBA_long cx, cy;

  cspi_return_if_fail (obj != NULL);

  Accessibility_Image_getImagePosition (CSPI_OBJREF (obj),
					&cx, &cy, ctype, cspi_ev ());

  if (!cspi_check_ev ("getImagePosition"))
    {
      *x = 0;
      *y = 0;
    }
  else
    {
      *x = cx;
      *y = cy;
    }
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

  cspi_return_if_fail (obj != NULL);

  bbox = Accessibility_Image_getImageExtents (CSPI_OBJREF (obj),
					      (CORBA_short) ctype,
					      cspi_ev ());

  if (!cspi_check_ev ("getImageExtents"))
    {
      *x = *y = *width = *height = 0;
    }
  else
    {
      *x = bbox.x;
      *y = bbox.y;
      *width = bbox.width;
      *height = bbox.height;
    }
}
