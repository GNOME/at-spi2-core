/*
 *
 * SpiAccessibleComponent function implementations
 *
 */

/**
 * SpiAccessibleComponent_ref:
 * @obj: a pointer to an object implementing #AccessibleComponent on which to operate.
 *
 * Increment the reference count for an #AccessibleComponent.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
SpiAccessibleComponent_ref (SpiAccessibleComponent *obj)
{
  Accessibility_Component_ref (*obj, &ev);
  return 0;
}

/**
 * SpiAccessibleComponent_unref:
 * @obj: a pointer to the object implementing #AccessibleComponent on which to operate.
 *
 * Decrement the reference count for an #AccessibleComponent.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
SpiAccessibleComponent_unref (SpiAccessibleComponent *obj)
{
  Accessibility_Component_unref (*obj, &ev);
  return 0;
}

/**
 * SpiAccessibleComponent_contains:
 * @obj: a pointer to the #AccessibleComponent to query.
 * @x: a #long specifying the x coordinate in question.
 * @y: a #long specifying the y coordinate in question.
 * @ctype: the desired coordinate system of the point (@x, @y)
 *         (e.g. COORD_TYPE_WINDOW, COORD_TYPE_SCREEN).
 *
 * Query whether a given #AccessibleComponent contains a particular point.
 *
 **/
boolean
SpiAccessibleComponent_contains (SpiAccessibleComponent *obj,
                              long x,
                              long y,
                              SpiAccessibleCoordType ctype)
{
  return Accessibility_Component_contains (*obj,
                                           (CORBA_long) x,
                                           (CORBA_long) y,
                                           ctype,
                                           &ev);
}

/**
 * SpiAccessibleComponent_getAccessibleAtPoint:
 * @obj: a pointer to the #AccessibleComponent to query.
 * @x: a #long specifying the x coordinate of the point in question.
 * @y: a #long specifying the y coordinate of the point in question.
 * @ctype: the coordinate system of the point (@x, @y)
 *         (e.g. COORD_TYPE_WINDOW, COORD_TYPE_SCREEN).
 *
 * Get the accessible child at a given coordinate within an #AccessibleComponent.
 *
 * Returns: a pointer to an #Accessible child of the specified component which
 *          contains the point (@x, @y), or NULL of no child contains the point.
 **/
SpiAccessible *
SpiAccessibleComponent_getAccessibleAtPoint (SpiAccessibleComponent *obj,
                                          long x,
                                          long y,
                                          SpiAccessibleCoordType ctype)
{
  SpiAccessible child;
  child = Accessibility_Component_getAccessibleAtPoint(*obj,
                                                       (CORBA_long) x,
                                                       (CORBA_long) y,
                                                       ctype,
                                                       &ev);
  return (child != NULL) ? Obj_Add (child) : NULL;
}

/**
 * SpiAccessibleComponent_getExtents:
 * @obj: a pointer to the #AccessibleComponent to query.
 * @x: a pointer to a #long into which the minimum x coordinate will be returned.
 * @y: a pointer to a #long into which the minimum y coordinate will be returned.
 * @width: a pointer to a #long into which the x extents (width) will be returned.
 * @height: a pointer to a #long into which the y extents (height) will be returned.
 * @ctype: the desired coordinate system into which to return the results,
 *         (e.g. COORD_TYPE_WINDOW, COORD_TYPE_SCREEN).
 *
 * Get the bounding box of the specified #AccessibleComponent.
 *
 **/
void
SpiAccessibleComponent_getExtents (SpiAccessibleComponent *obj,
                                long *x,
                                long *y,
                                long *width,
                                long *height,
                                SpiAccessibleCoordType ctype)
{
  CORBA_long cx, cy, cw, ch;	
  Accessibility_Component_getExtents (*obj,
                                      &cx,
                                      &cy,
                                      &cw,
                                      &ch,
				      ctype,
				      &ev);
  spi_warn_ev (&ev, "SpiAccessibleComponent_getExtents");
  *x = (long) cx;
  *y = (long) cy;
  *width = (long) cw;
  *height = (long) ch;
}

/**
 * SpiAccessibleComponent_getPosition:
 * @obj: a pointer to the #AccessibleComponent to query.
 * @x: a pointer to a #long into which the minimum x coordinate will be returned.
 * @y: a pointer to a #long into which the minimum y coordinate will be returned.
 * @ctype: the desired coordinate system into which to return the results,
 *         (e.g. COORD_TYPE_WINDOW, COORD_TYPE_SCREEN).
 *
 * Get the minimum x and y coordinates of the specified #AccessibleComponent.
 *
 **/
void
SpiAccessibleComponent_getPosition (SpiAccessibleComponent *obj,
                                 long *x,
                                 long *y,
                                 SpiAccessibleCoordType ctype)
{
  Accessibility_Component_getPosition (*obj,
                                       (CORBA_long *) x,
                                       (CORBA_long *) y,
                                       ctype,
                                       &ev);
}

/**
 * SpiAccessibleComponent_getSize:
 * @obj: a pointer to the #AccessibleComponent to query.
 * @width: a pointer to a #long into which the x extents (width) will be returned.
 * @height: a pointer to a #long into which the y extents (height) will be returned.
 *
 * Get the size of the specified #AccessibleComponent.
 *
 **/
void
SpiAccessibleComponent_getSize (SpiAccessibleComponent *obj,
                             long *width,
                             long *height)
{
  Accessibility_Component_getSize (*obj,
                                   (CORBA_long *) width,
                                   (CORBA_long *) height,
                                   &ev);
}

/* Not Yet Implemented */
void
SpiAccessibleComponent_grabFocus (SpiAccessibleComponent *obj)
{
  ;
}
