#include <cspi/spi-private.h>

/**
 * AccessibleEditableText_ref:
 * @obj: a pointer to the #AccessibleEditableText object on which to operate.
 *
 * Increment the reference count for an #AccessibleEditableText object.
 *       Since AccessibleEditableText is derived from AccessibleText,
 *       this is the same as AccessibleText_ref().
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
AccessibleEditableText_ref (AccessibleEditableText *obj)
{
  cspi_object_ref (obj);
  return 0;
}



/**
 * AccessibleEditableText_unref:
 * @obj: a pointer to the #AccessibleEditableText object on which to operate.
 *
 * Decrement the reference count for an #AccessibleEdiitableText object.
 *       Since AccessibleEditableText is derived from AccessibleText,
 *       this is the same as AccessibleText_unref().
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
AccessibleEditableText_unref (AccessibleEditableText *obj)
{
  cspi_object_unref (obj);
  return 0;
}



/**
 * AccessibleEditableText_setAttributes:
 * @obj: a pointer to the #AccessibleEditableText object to modify.
 * @attributes: a character string indicating the attributes to apply to the range,
 *        delimited by ':'.
 * @startOffset: a #long indicating the start of the desired text range.
 * @endOffset: a #long indicating the first character past the desired range.
 *
 * Set the attributes applied to a range of text from an #AccessibleEditableText
 *          object, and the bounds of the range.
 *
 * Returns: #TRUE if the operation was successful, otherwise #FALSE.
 *
 **/
boolean
AccessibleEditableText_setAttributes (AccessibleEditableText *obj,
				      const char *attributes,
				      long int startPos,
				      long int endPos)
{
  return (boolean)
    Accessibility_EditableText_setAttributes (CSPI_OBJREF (obj),
					      (CORBA_char *) attributes,
					      (CORBA_long) startPos,
					      (CORBA_long) endPos, cspi_ev ());
}



/**
 * AccessibleEditableText_setTextContents:
 * @obj: a pointer to the #AccessibleEditableText object to modify.
 * @newContents: a character string, encoded in UTF-8, which is to
 *      become the new text contents of the #AccessibleEditableText object.
 *
 * Replace the entire text contents of an #AccessibleEditableText object.
 *
 * Returns: #TRUE if the operation was successful, otherwise #FALSE.
 *
 **/
boolean
AccessibleEditableText_setTextContents (AccessibleEditableText *obj,
                                        const char *newContents)
{
  Accessibility_EditableText_setTextContents (CSPI_OBJREF (obj),
					      (CORBA_char *) newContents, cspi_ev ());
  return TRUE; /* TODO: make bonobo method return a boolean */
}



/**
 * AccessibleEditableText_insertText:
 * @obj: a pointer to the #AccessibleEditableText object to modify.
 * @position: an integer indicating the character offset at which to insert
 *       the new text.  
 * @text: a char* pointer to the text to insert, in UTF-8 encoding.
 * @length: (frankly I'm not sure this parameter should be here)
 *
 * Insert text into an #AccessibleEditableText object.
 * As with all character offsets, the specified @position may not be the
 *       same as the resulting byte offset, since the text is in a
 *       variable-width encoding.
 *
 * Returns: #TRUE if the operation was successful, otherwise #FALSE.
 *
 **/
boolean
AccessibleEditableText_insertText (AccessibleEditableText *obj,
                                   long int position,
                                   char *text,
                                   long int length)
{
  Accessibility_EditableText_insertText (CSPI_OBJREF (obj),
					 (CORBA_long) position, (CORBA_char *) text,
					 (CORBA_long) length, cspi_ev ());
  return TRUE;
}



/**
 * AccessibleEditableText_copyText:
 * @obj: a pointer to the #AccessibleEditableText object to modify.
 * @startPos: an integer indicating the starting character offset
 *       of the text to copy.
 * @endPos: an integer indicating the offset of the first character
 *       past the end of the text section to be copied.
 *
 * Copy text from an #AccessibleEditableText object into the clipboard.
 *
 * @see: AccessibleEditableText_pasteText 
 *
 * Returns: #TRUE if the operation was successful, otherwise #FALSE.
 *
 **/
boolean
AccessibleEditableText_copyText (AccessibleText *obj,
                                 long int startPos,
                                 long int endPos)
{
  Accessibility_EditableText_copyText (CSPI_OBJREF (obj),
				       (CORBA_long) startPos, (CORBA_long) endPos, cspi_ev ());
  return TRUE;
}


/**
 * AccessibleEditableText_cutText:
 * @obj: a pointer to the #AccessibleEditableText object to modify.
 * @startPos: an integer indicating the starting character offset
 *       of the text to cut.
 * @endPos: an integer indicating the offset of the first character
 *       past the end of the text section to be cut.
 *
 * Delete text from an #AccessibleEditableText object, copying the
 *       excised portion into the clipboard.
 *
 * @see: AccessibleEditableText_pasteText
 *
 * Returns: #TRUE if operation was successful, #FALSE otherwise.
 *
 **/
boolean
AccessibleEditableText_cutText (AccessibleEditableText *obj,
                                long int startPos,
                                long int endPos)
{
  Accessibility_EditableText_cutText (CSPI_OBJREF (obj),
				       (CORBA_long) startPos, (CORBA_long) endPos, cspi_ev ());
  return TRUE;
}



/**
 * AccessibleEditableText_deleteText:
 * @obj: a pointer to the #AccessibleEditableText object to modify.
 * @startPos: an integer indicating the starting character offset
 *       of the text to delete.
 * @endPos: an integer indicating the offset of the first character
 *       past the end of the text section to be deleted.
 *
 * Delete text from an #AccessibleEditableText object, without copying the
 *       excised portion into the clipboard.
 *
 * @see: AccessibleEditableText_cutText
 *
 * Returns: #TRUE if the operation was successful, otherwise #FALSE.
 *
 **/
boolean
AccessibleEditableText_deleteText (AccessibleEditableText *obj,
                                   long startPos,
                                   long endPos)
{
  Accessibility_EditableText_deleteText (CSPI_OBJREF (obj),
				       (CORBA_long) startPos, (CORBA_long) endPos, cspi_ev ());
  return TRUE;
}



/**
 * AccessibleEditableText_pasteText:
 * @obj: a pointer to the #AccessibleEditableText object to modify.
 * @position: an integer indicating the character offset at which to insert
 *       the new text.  
 *
 * Insert text from the clipboard into an #AccessibleEditableText object.
 * As with all character offsets, the specified @position may not be the
 *       same as the resulting byte offset, since the text is in a
 *       variable-width encoding.
 *
 * Returns: #TRUE if the operation was successful, otherwise #FALSE.
 *
 **/
boolean
AccessibleEditableText_pasteText (AccessibleEditableText *obj,
                                  long int position)
{
  Accessibility_EditableText_pasteText (CSPI_OBJREF (obj),
					(CORBA_long) position, cspi_ev ());
  return TRUE;
}

