/**
 * AccessibleHyperlink_ref:
 * @obj: a pointer to the #AccessibleHyperlink object on which to operate.
 *
 * Increment the reference count for an #AccessibleHyperlink object.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
AccessibleHyperlink_ref (AccessibleHyperlink *obj)
{
  Accessibility_Hyperlink_ref (*obj, &ev);
  return 0;
}



/**
 * AccessibleHyperlink_unref:
 * @obj: a pointer to the #AccessibleHyperlink object on which to operate.
 *
 * Decrement the reference count for an #AccessibleHyperlink object.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
AccessibleHyperlink_unref (AccessibleHyperlink *obj)
{
  Accessibility_Hyperlink_unref (*obj, &ev);
  return 0;
}

/**
 * AccessibleHyperlink_getNAnchors:
 * @obj: a pointer to the #AccessibleHyperlink object on which to operate.
 *
 * Get the total number of anchors which an #AccessibleHyperlink implementor has.
 *       Though typical hyperlinks have only one anchor, client-side image maps and
 *       other hypertext objects may potentially activate or refer to multiple
 *       URIs.  For each anchor there is a corresponding URI and object.
 * @see AccessibleHyperlink_getURI() and AccessibleHyperlink_getObject().
 *
 * Returns: a #long indicating the number of anchors in this hyperlink.
 *
 **/
long
AccessibleHyperlink_getNAnchors (AccessibleHyperlink *obj)
{
  return (long)
    Accessibility_Hyperlink__get_nAnchors (*obj, &ev);
}


/**
 * AccessibleHyperlink_getURI:
 * @obj: a pointer to the #AccessibleHyperlink implementor on which to operate.
 * @i: a (zero-index) long integer indicating which hyperlink anchor to query.
 *
 * Get the URI associated with a particular hyperlink anchor.  
 *
 * Returns: a UTF-8 string giving the URI of the @ith hyperlink anchor.
 *
 **/
char *
AccessibleHyperlink_getURI (AccessibleHyperlink *obj,
                            long int i)
{
  return (char *)
    Accessibility_Hyperlink_getURI (*obj,
				    (CORBA_long) i, &ev);
}

/**
 * AccessibleHyperlink_getObject:
 * @obj: a pointer to the #AccessibleHyperlink implementor on which to operate.
 * @i: a (zero-index) long integer indicating which hyperlink anchor to query.
 *
 * Get the object associated with a particular hyperlink anchor, as an #Accessible. 
 *
 * Returns: an #Accessible that represents the object associated with the @ith anchor
 *        of the specified #AccessibleHyperlink.
 *
 **/
Accessible*
AccessibleHyperlink_getObject (AccessibleHyperlink *obj,
                               long int i)
{
  return Obj_Add (
    Accessibility_Hyperlink_getObject (*obj,
				       (CORBA_long) i, &ev));
}

/**
 * AccessibleHyperlink_getIndexRange:
 * @obj: a pointer to the #AccessibleHyperlink implementor on which to operate.
 * @startIndex: a pointer to a long integer into which the starting
 *       offset of the text associated with this #AccessibleHyperlink is returned.
 * @endIndex: a pointer to a long integer into which the offset of the first character
 *       after the text associated with this #AccessibleHyperlink is returned.
 *
 *
 * Get the starting and ending character offsets of the text range associated with
 *       a #AccessibleHyperlink, in its originating #AccessibleHypertext.
 *
 **/
void
AccessibleHyperlink_getIndexRange (AccessibleHyperlink *obj,
                                   long int *startIndex,
                                   long int *endIndex)
{
  *startIndex = (long)
    Accessibility_Hyperlink__get_startIndex (*obj, &ev);
  *endIndex = (long)
    Accessibility_Hyperlink__get_endIndex (*obj, &ev);
}



/**
 * AccessibleHyperlink_isValid:
 * @obj: a pointer to the #AccessibleHyperlink on which to operate.
 *
 * Tell whether an #AccessibleHyperlink object is still valid with respect to its
 *          originating hypertext object.
 *
 * Returns: #TRUE of the specified #AccessibleHyperlink is still valid with respect
 *          to its originating #AccessibleHypertext object, #FALSE otherwise.
 *
 **/
boolean
AccessibleHyperlink_isValid (AccessibleHyperlink *obj)
{
  return (boolean)
    Accessibility_Hyperlink_isValid (*obj, &ev);
}


