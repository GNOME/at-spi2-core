#include <cspi/spi-private.h>

/**
 * AccessibleHypertext_ref:
 * @obj: a pointer to the #AccessibleHypertext object on which to operate.
 *
 * Increment the reference count for an #AccessibleHypertext object.
 *       Since AccessibleHypertext is derived from AccessibleText,
 *       this is the same as AccessibleText_unref().
 **/
void
AccessibleHypertext_ref (AccessibleHypertext *obj)
{
  cspi_object_ref (obj);
}

/**
 * AccessibleHypertext_unref:
 * @obj: a pointer to the #AccessibleHypertext object on which to operate.
 *
 * Decrement the reference count for an #AccessibleHypertext object.
 *       Since AccessibleHypertext is derived from AccessibleText,
 *       this is the same as AccessibleText_unref().
 **/
void
AccessibleHypertext_unref (AccessibleHypertext *obj)
{
  cspi_object_unref (obj);
}

/**
 * AccessibleHypertext_getNLinks:
 * @obj: a pointer to the #AccessibleHypertext implementor on which to operate.
 *
 * Get the total number of #AccessibleHyperlinks which an
 *        #AccessibleHypertext implementor has.
 *
 * Returns: a #long indicating the number of #AccessibleHyperlinks
 *        of the #AccessibleHypertext implementor, or -1 if
 *        the number cannot be determined (for example, if the
 *        #AccessibleHypertext object is so large that it is not
 *        all currently in the memory cache).
 **/
long
AccessibleHypertext_getNLinks (AccessibleHypertext *obj)
{
  long retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval =
    Accessibility_Hypertext_getNLinks (CSPI_OBJREF (obj), cspi_ev ());

  cspi_return_val_if_ev ("getNLinks", -1);

  return retval;
}

/**
 * AccessibleHypertext_getLink:
 * @obj: a pointer to the #AccessibleHypertext implementor on which to operate.
 * @linkIndex: a (zero-index) long integer indicating which hyperlink to query.
 *
 * Get the #AccessibleHyperlink object at a specified index.
 *
 * Returns: the #AccessibleHyperlink object specified by #linkIndex.
 **/
AccessibleHyperlink *
AccessibleHypertext_getLink (AccessibleHypertext *obj,
                             long int             linkIndex)
{
  AccessibleHyperlink *retval;
	
  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval = cspi_object_add (
    Accessibility_Hypertext_getLink (CSPI_OBJREF (obj),
				     (CORBA_long) linkIndex,
				     cspi_ev ()));
  
  cspi_return_val_if_ev ("getLink", NULL); 

  return retval;
}

/**
 * AccessibleHypertext_getLinkIndex:
 * @obj: a pointer to the #AccessibleHypertext implementor on which to operate.
 * @characterOffset: an integer specifying the character offset to query.
 *
 * Get the index of the #AccessibleHyperlink object at a specified
 *        character offset.
 *
 * Returns: the linkIndex of the #AccessibleHyperlink active at
 *        character offset @characterOffset, or -1 if there is
 *        no hyperlink at the specified character offset.
 **/
long
AccessibleHypertext_getLinkIndex (AccessibleHypertext *obj,
                                  long int             characterOffset)
{
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval = 
    Accessibility_Hypertext_getLinkIndex (CSPI_OBJREF (obj),
					  (CORBA_long) characterOffset,
					  cspi_ev ());

  cspi_return_val_if_ev ("getLinkIndex", -1);

  return retval;
}


