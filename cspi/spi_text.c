#include <cspi/spi-private.h>

/**
 * AccessibleText_ref:
 * @obj: a pointer to the #AccessibleText object on which to operate.
 *
 * Increment the reference count for an #AccessibleText object.
 *
 **/
void
AccessibleText_ref (AccessibleText *obj)
{
  cspi_object_ref (obj);
}

/**
 * AccessibleText_unref:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Decrement the reference count for an #AccessibleText object.
 *
 **/
void
AccessibleText_unref (AccessibleText *obj)
{
  cspi_object_unref (obj);
}

/**
 * AccessibleText_getCharacterCount:
 * @obj: a pointer to the #AccessibleText object to query.
 *
 * Get the character count of an #AccessibleText object.
 *
 * Returns: a long integer indicating the total number of
 *              characters in the #AccessibleText object.
 *
 **/
long
AccessibleText_getCharacterCount (AccessibleText *obj)
{
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval = (long)
    Accessibility_Text__get_characterCount (CSPI_OBJREF (obj), cspi_ev ());

  cspi_return_val_if_ev ("_getCharacterCount", -1);

  return retval;
}

/**
 * AccessibleText_getText:
 * @obj: a pointer to the #AccessibleText object to query.
 * @startOffset: a #long indicating the start of the desired text range.
 * @endOffset: a #long indicating the first character past the desired range.
 *
 * Get a range of text from an #AccessibleText object.  The number of bytes
 *          in the returned string may exceed endOffset-startOffset, since
 *          UTF-8 is a variable-width encoding.
 *
 * Returns: a text string containing characters from @startOffset
 *          to @endOffset-1, inclusive, encoded as UTF-8.
 *
 **/
char *
AccessibleText_getText (AccessibleText *obj,
                        long int startOffset,
                        long int endOffset)
{
  return (char *)
    Accessibility_Text_getText (CSPI_OBJREF (obj),
				(CORBA_long) startOffset, (CORBA_long) endOffset, cspi_ev ());
}

/**
 * AccessibleText_getCaretOffset:
 * @obj: a pointer to the #AccessibleText object to query.
 *
 * Get the current offset of the text caret in an #AccessibleText object.
 *
 * Returns: a long integer indicating the current position of the text caret.
 *
 **/
long
AccessibleText_getCaretOffset (AccessibleText *obj)
{
  return (long)
    Accessibility_Text__get_caretOffset (CSPI_OBJREF (obj), cspi_ev ());
}


/**
 * AccessibleText_getAttributes:
 * @obj: a pointer to the #AccessibleText object to query.
 * @offset: a long integer indicating the offset from which the attribute
 *        search is based.
 * @startOffset: a #long indicating the start of the desired text range.
 * @endOffset: a #long indicating the first character past the desired range.
 *
 * Get the attributes applied to a range of text from an #AccessibleText
 *          object, and the bounds of the range.
 *
 * Returns: a text string describing the attributes occurring within the
 *          attribute run containing @offset, encoded as UTF-8 and
 *          delimited by ':'
 *
 **/
char *
AccessibleText_getAttributes (AccessibleText *obj,
			      long int offset,
			      long int *startOffset,
			      long int *endOffset)
{
  CORBA_long retStartOffset, retEndOffset;
  char *retval;	
  retval = (char *)
    Accessibility_Text_getAttributes (CSPI_OBJREF (obj),
				      (CORBA_long) offset,
				      &retStartOffset,
				      &retEndOffset,
				      cspi_ev ());
  *startOffset = (long) retStartOffset;
  *endOffset = (long) retEndOffset;
  return retval;
}



/**
 * AccessibleText_setCaretOffset:
 * @obj: a pointer to the #AccessibleText object on which to operate.
 * @newOffset: the offset to which the text caret is to be moved.
 *
 * Set the text caret position for an #AccessibleText object.
 *
 * Returns: #TRUE if successful, #FALSE otherwise.
 *
 **/
SPIBoolean
AccessibleText_setCaretOffset (AccessibleText *obj,
                               long int newOffset)
{
  return (SPIBoolean)
    Accessibility_Text_setCaretOffset (CSPI_OBJREF (obj),
				       (CORBA_long) newOffset, cspi_ev ());
}

/**
 * AccessibleText_getTextBeforeOffset:
 * @obj: a pointer to the #AccessibleText object on which to operate.
 * @offset: a long integer indicating the offset from which the delimiter
 *        search is based.
 * @type: an #AccessibleTextBoundaryType indicating whether the desired
 *       text string is a word, sentence, line, or attribute run.
 * @startOffset: a pointer to a long integer which is assigned the
 *       starting offset of the returned string, relative to the
 *       original #AccessibleText.
 * @endOffset: a pointer to a long integer which is assigned the
 *       ending offset of the returned string, relative to the original
 *       #AccessibleText.
 *
 * Get delimited text from an #AccessibleText object which precedes a given
 *          text offset.
 *
 * Returns: a UTF-8 string representing the delimited text, both of whose
 *          delimiting boundaries are before the current offset, or
 *          an empty string if no such text exists.
 *
 **/
char *
AccessibleText_getTextBeforeOffset (AccessibleText *obj,
                                    long int offset,
                                    AccessibleTextBoundaryType type,
				    long int *startOffset,
				    long int *endOffset)
{
  char *retval;
  CORBA_long retStartOffset, retEndOffset;
  retval = (char *)
    Accessibility_Text_getTextBeforeOffset (CSPI_OBJREF (obj),
					   (CORBA_long) offset, (Accessibility_TEXT_BOUNDARY_TYPE) type,
					   &retStartOffset, &retEndOffset,
					   cspi_ev ());
  *startOffset = (long) retStartOffset;
  *endOffset = (long) retEndOffset;
  return retval;
}

/**
 * AccessibleText_getTextAtOffset:
 * @obj: a pointer to the #AccessibleText object on which to operate.
 * @offset: a long integer indicating the offset from which the delimiter
 *        search is based.
 * @type: an #AccessibleTextBoundaryType indicating whether the desired
 *       text string is a word, sentence, line, or attribute run.
 * @startOffset: a pointer to a long integer which is assigned the
 *       starting offset of the returned string, relative to the
 *       original #AccessibleText.
 * @endOffset: a pointer to a long integer which is assigned the
 *       ending offset of the returned string, relative to the original
 *       #AccessibleText.
 *
 * Get delimited text from an #AccessibleText object which includes a given
 *          text offset.
 *
 * Returns: a UTF-8 string representing the delimited text, whose
 *          delimiting boundaries bracket the current offset, or
 *          an empty string if no such text exists.
 *
 **/
char *
AccessibleText_getTextAtOffset (AccessibleText *obj,
				long int offset,
				AccessibleTextBoundaryType type,
				long int *startOffset, long int *endOffset)
{
  CORBA_long corbaStartOffset;
  CORBA_long corbaEndOffset;
  char *retval = "";
  retval = Accessibility_Text_getTextAtOffset (CSPI_OBJREF (obj),
					       (CORBA_long) offset,
					       (Accessibility_TEXT_BOUNDARY_TYPE) type,
					       &corbaStartOffset,
					       &corbaEndOffset,
					       cspi_ev ());
  *startOffset = (long) corbaStartOffset;
  *endOffset = (long) corbaEndOffset;
#ifdef CSPI_DEBUG
  fprintf (stderr, "text offsets %ld to %ld\n", *startOffset, *endOffset);
#endif
  return retval;
}


/**
 * AccessibleText_getTextAfterOffset:
 * @obj: a pointer to the #AccessibleText object on which to operate.
 * @offset: a long integer indicating the offset from which the delimiter
 *        search is based.
 * @type: an #AccessibleTextBoundaryType indicating whether the desired
 *       text string is a word, sentence, line, or attribute run.
 * @startOffset: a pointer to a long integer which is assigned the
 *       starting offset of the returned string, relative to the
 *       original #AccessibleText.
 * @endOffset: a pointer to a long integer which is assigned the
 *       ending offset of the returned string, relative to the original
 *       #AccessibleText.
 *
 * Get delimited text from an #AccessibleText object which follows a given
 *          text offset.
 *
 * Returns: a UTF-8 string representing the delimited text, both of whose
 *          delimiting boundaries are after or inclusive of the current
 *          offset, or an empty string if no such text exists.
 *
 **/
char *
AccessibleText_getTextAfterOffset (AccessibleText *obj,
				   long int offset,
				   AccessibleTextBoundaryType type,
				   long int *startOffset, long int *endOffset)
{
  char *retval;
  CORBA_long retStartOffset, retEndOffset;
  retval = (char *)
    Accessibility_Text_getTextAfterOffset (CSPI_OBJREF (obj),
					   (CORBA_long) offset, (Accessibility_TEXT_BOUNDARY_TYPE) type,
					   &retStartOffset, &retEndOffset,
					   cspi_ev ());
  *startOffset = (long) retStartOffset;
  *endOffset = (long) retEndOffset;
  return retval;
}



/**
 * AccessibleText_getCharacterAtOffset:
 * @obj: a pointer to the #AccessibleText object on which to operate.
 * @offset: a long integer indicating the text offset where the desired
 *          character is located.
 *
 * Get the character at a given offset for an #AccessibleText object.
 *
 * Returns: an #unsigned long integer which represents the
 *        UCS-4 unicode code point of the given character, or
 *        0xFFFFFFFF if the character in question cannot be represented
 *        in the UCS-4 encoding.
 *
 **/
unsigned long
AccessibleText_getCharacterAtOffset (AccessibleText *obj,
                                     long int offset)
{
  return (unsigned long)
    Accessibility_Text_getCharacterAtOffset (CSPI_OBJREF (obj),
					     (CORBA_long) offset, cspi_ev ());
}

/**
 * AccessibleText_getCharacterExtents:
 * @obj: a pointer to the #AccessibleText object on which to operate.
 * @offset: an integer indicating the offset of the text character for
 *        whom boundary information is requested.
 * @x: a pointer to a long integer into which the nominal x coordinate
 *     of the corresponding glyph will be returned.
 * @y:a pointer to a long integer into which the nominal y coordinate
 *     of the corresponding glyph will be returned.
 * @width:a pointer to a long integer into which the width
 *     of the corresponding glyph will be returned.
 * @height: a pointer to a long integer into which the height
 *     of the corresponding glyph will be returned.
 * @type: an #AccessibleCoordType indicating the coordinate system to use
 *        for the returned values.
 *
 * Get the bounding box containing the glyph representing
 *        the character at a particular text offset.
 *
 **/
void
AccessibleText_getCharacterExtents (AccessibleText *obj,
                                    long int offset,
                                    long int *x,
                                    long int *y,
                                    long int *width,
                                    long int *height,
				    AccessibleCoordType type)
{
  CORBA_long retX, retY, retWidth, retHeight;
  Accessibility_Text_getCharacterExtents (CSPI_OBJREF (obj),
					  (CORBA_long) offset,
					  &retX,
					  &retY,
					  &retWidth,
					  &retHeight,
					  (CORBA_short) type, cspi_ev ());
  *x = (long) retX;
  *y = (long) retY;
  *width = (long) retWidth;
  *height = (long) retHeight;
}


/**
 * AccessibleText_getOffsetAtPoint:
 * @obj: a pointer to the #AccessibleText object on which to operate.
 * @x: the x coordinate of the point to be queried.
 * @y: the y coordinate of the point to be queried.
 * @type: an #AccessibleCoordType indicating the coordinate system in which
 *       the values should be returned.
 *
 * Get the bounding box for a glyph at a certain #AccessibleText offset.
 *
 * Returns: the offset (as a long integer) at the point (@x, @y)
 *       in the specified coordinate system.
 *
 **/
long
AccessibleText_getOffsetAtPoint (AccessibleText *obj,
                                 long int x,
                                 long int y,
				 AccessibleCoordType type)
{
  return (long)
    Accessibility_Text_getOffsetAtPoint (CSPI_OBJREF (obj),
					 (CORBA_long) x, (CORBA_long) y, (CORBA_short) type, cspi_ev ());
}


/**
 * AccessibleText_getNSelections:
 * @obj: a pointer to the #AccessibleText object on which to operate.
 *
 * Get the number of active non-contiguous selections for an
 *          #AccessibleText object.
 *
 * Returns: a long integer indicating the current
 *          number of non-contiguous text selections active
 *          within an #AccessibleText object.
 *
 **/
long
AccessibleText_getNSelections (AccessibleText *obj)
{
  return (long)
    Accessibility_Text_getNSelections (CSPI_OBJREF (obj), cspi_ev ());
}



/**
 * AccessibleText_getSelection:
 * @obj: a pointer to the #AccessibleText object on which to operate.
 * @selectionNum: an integer indicating which selection to query.
 * @startOffset: a pointer to a long integer into which the start offset
 *           of the selection will be returned.
 * @endOffset: a pointer to a long integer into which the start offset
 *           of the selection will be returned.
 *
 * Get the bounds of the @selectionNum-th active text selection for an
 *         #AccessibleText object.
 *
 **/
void
AccessibleText_getSelection (AccessibleText *obj,
			     long int selectionNum,
			     long int *startOffset,
			     long int *endOffset)
{
  CORBA_long retStartOffset, retEndOffset;
  Accessibility_Text_getSelection (CSPI_OBJREF (obj),
				   (CORBA_long) selectionNum,
				   &retStartOffset, &retEndOffset, cspi_ev ());
  
  *startOffset = (long) retStartOffset;
  *endOffset = (long) retEndOffset;
}



/**
 * AccessibleText_addSelection:
 * @obj: a pointer to the #AccessibleText object on which to operate.
 * @startOffset: the starting offset of the desired new selection.
 * @endOffset: the offset of the first character after the new selection.
 *
 * Select some text (add a text selection) in an #AccessibleText object.
 *
 * Returns: #TRUE if successful, #FALSE otherwise.
 *
 **/
SPIBoolean
AccessibleText_addSelection (AccessibleText *obj,
			     long int startOffset, long int endOffset)
{
  return (SPIBoolean)
    Accessibility_Text_addSelection (CSPI_OBJREF (obj),
				     (CORBA_long) startOffset, (CORBA_long) endOffset,
				     cspi_ev ());
}


/**
 * AccessibleText_removeSelection:
 * @obj: a pointer to the #AccessibleText object on which to operate.
 * @selectionNum: an integer indicating which (possibly of several)
 *         text selection to remove.
 *
 * De-select a text selection.
 *
 * Returns: #TRUE if successful, #FALSE otherwise.
 *
 **/
SPIBoolean
AccessibleText_removeSelection (AccessibleText *obj,
				long int selectionNum)
{
  return (SPIBoolean)
    Accessibility_Text_removeSelection (CSPI_OBJREF (obj),
					(CORBA_long) selectionNum, cspi_ev ());
}

/**
 * AccessibleText_setSelection:
 * @obj: a pointer to the #AccessibleText object on which to operate.
 * @selectionNum: a zero-offset index indicating which text selection to modify.
 * @startOffset: a long int, the new starting offset for the selection.
 * @endOffset: a long int, the desired new offset of the first character
 *             after the selection.
 *
 * Change the bounds of an existing #AccessibleText text selection.
 *
 * Returns: #TRUE if successful, #FALSE otherwise.
 *
 **/
SPIBoolean
AccessibleText_setSelection (AccessibleText *obj,
			     long int selectionNum,
			     long int startOffset,
			     long int endOffset)
{
  return (SPIBoolean)
    Accessibility_Text_setSelection (CSPI_OBJREF (obj),
				     (CORBA_long) selectionNum,
				     (CORBA_long) startOffset,
				     (CORBA_long) endOffset, cspi_ev ());
}



