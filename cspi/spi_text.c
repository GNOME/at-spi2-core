/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#include <cspi/spi-private.h>

static Accessibility_TEXT_BOUNDARY_TYPE
get_accessible_text_boundary_type (AccessibleTextBoundaryType type)
{
  switch (type)
    {
    case SPI_TEXT_BOUNDARY_CHAR:
      return Accessibility_TEXT_BOUNDARY_CHAR;
      break;
    case SPI_TEXT_BOUNDARY_CURSOR_POS:
      /* FixME */
      return Accessibility_TEXT_BOUNDARY_CHAR;
      break;
    case SPI_TEXT_BOUNDARY_WORD_START:
      return Accessibility_TEXT_BOUNDARY_WORD_START;
      break;
    case SPI_TEXT_BOUNDARY_WORD_END:
      return Accessibility_TEXT_BOUNDARY_WORD_END;
      break;
    case SPI_TEXT_BOUNDARY_SENTENCE_START:
      return Accessibility_TEXT_BOUNDARY_SENTENCE_START;
      break;
    case SPI_TEXT_BOUNDARY_SENTENCE_END:
      return Accessibility_TEXT_BOUNDARY_SENTENCE_END;
      break;
    case SPI_TEXT_BOUNDARY_LINE_START:
      return Accessibility_TEXT_BOUNDARY_LINE_START;
      break;
    case SPI_TEXT_BOUNDARY_LINE_END:
      return Accessibility_TEXT_BOUNDARY_LINE_END;
      break;
    case SPI_TEXT_BOUNDARY_ATTRIBUTE_RANGE:
      /* Fixme */
      return Accessibility_TEXT_BOUNDARY_CHAR;
      break;
    default:
      /* FIXME */
      return Accessibility_TEXT_BOUNDARY_CHAR;
    }
}

static Accessibility_TEXT_CLIP_TYPE
get_accessible_text_clip_type (AccessibleTextClipType type)
{
  switch (type)
    {
    case SPI_TEXT_CLIP_NONE:
      return Accessibility_TEXT_CLIP_NONE;
      break;
    case SPI_TEXT_CLIP_MIN:
      return Accessibility_TEXT_CLIP_MIN;
      break;
    case SPI_TEXT_CLIP_MAX:      
      return Accessibility_TEXT_CLIP_MAX;
      break;
    default:
      return Accessibility_TEXT_CLIP_BOTH;
    }
}

static AccessibleTextRange **
get_accessible_text_ranges_from_range_seq (Accessibility_Text_RangeList *range_seq)
{
  AccessibleTextRange **ranges = NULL;
  AccessibleTextRange *array = NULL;
  int i;
  if (range_seq && range_seq->_length > 0) 
    {
      ranges = g_new0 (AccessibleTextRange *, range_seq->_length + 1);
    }
  array = g_new0 (AccessibleTextRange, range_seq->_length);
  for (i = 0; i < range_seq->_length; i++) 
    {
      AccessibleTextRange *range;
      range = &array[i];
      range->start = range_seq->_buffer[i].startOffset;
      range->end = range_seq->_buffer[i].endOffset;
      range->contents = CORBA_string_dup (range_seq->_buffer[i].content);
      ranges[i] = range;
    }
  ranges[i] = NULL; /* null-terminated list! */
  CORBA_free (range_seq);

  return ranges;
}


/**
 * AccessibleText_ref:
 * @obj: a pointer to the #AccessibleText object on which to operate.
 *
 * Increment the reference count for an #AccessibleText object.
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
 **/
long
AccessibleText_getCharacterCount (AccessibleText *obj)
{
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval = Accessibility_Text__get_characterCount (CSPI_OBJREF (obj), cspi_ev ());

  cspi_return_val_if_ev ("getCharacterCount", -1);

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
 **/
char *
AccessibleText_getText (AccessibleText *obj,
                        long int startOffset,
                        long int endOffset)
{
  char *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  retval =
    Accessibility_Text_getText (CSPI_OBJREF (obj),
				startOffset,
				endOffset,
				cspi_ev ());

  cspi_return_val_if_ev ("getText", NULL);

  return retval;
}

/**
 * AccessibleText_getCaretOffset:
 * @obj: a pointer to the #AccessibleText object to query.
 *
 * Get the current offset of the text caret in an #AccessibleText object.
 *
 * Returns: a long integer indicating the current position of the text caret.
 **/
long
AccessibleText_getCaretOffset (AccessibleText *obj)
{
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval =
    Accessibility_Text__get_caretOffset (CSPI_OBJREF (obj), cspi_ev ());

  cspi_return_val_if_ev ("getCaretOffset", -1);

  return retval;
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
 *          The text attributes correspond to CSS attributes where possible,
 *          keys and values are delimited from one another via ":", and
 *          the delimiter between key/value pairs is ";". Thus 
 *          "font-size:10;foreground-color:0,0,0" would be a valid
 *          return string.
 *
 * Returns: a text string describing the attributes occurring within the
 *          attribute run containing @offset, encoded as UTF-8.
 **/
char *
AccessibleText_getAttributes (AccessibleText *obj,
			      long int offset,
			      long int *startOffset,
			      long int *endOffset)
{
  CORBA_long retStartOffset, retEndOffset;
  char *retval;	

  if (obj == NULL)
    {
      *startOffset = *endOffset = -1;
      return NULL;
    }

  retval = Accessibility_Text_getAttributes (CSPI_OBJREF (obj),
				      offset,
				      &retStartOffset,
				      &retEndOffset,
				      cspi_ev ());

  if (!cspi_check_ev ("getAttributes"))
    {
      *startOffset = *endOffset = -1;
      retval = NULL;
    }
  else
    {
      *startOffset = retStartOffset;
      *endOffset   = retEndOffset;
    }

  return retval;
}

/**
 * AccessibleText_getDefaultAttributes:
 * @obj: a pointer to the #AccessibleText object to query.
 *
 * Get the default attributes applied to an #AccessibleText
 *          object.
 *          The text attributes correspond to CSS attributes where possible,
 *          keys and values are delimited from one another via ":", and
 *          the delimiter between key/value pairs is ";". Thus 
 *          "font-size:10;foreground-color:0,0,0" would be a valid
 *          return string.  The combination of this attribute set and
 *          the attributes reported by #AccessibleText_getAttributes
 *          describes the entire set of text attributes over a range.
 *
 * @Since: AT-SPI 1.4
 *
 * Returns: a text string describing the default attributes 
 *          applied to a text object, (exclusive of explicitly-set
 *          attributes), encoded as UTF-8.
 **/
char *
AccessibleText_getDefaultAttributes (AccessibleText *obj)
{
  char *retval;	

  if (obj == NULL)
    {
      return NULL;
    }

  retval = Accessibility_Text_getDefaultAttributes (CSPI_OBJREF (obj),
						    cspi_ev ());

  if (!cspi_check_ev ("getAttributes"))
    {
      retval = NULL;
    }

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
 **/
SPIBoolean
AccessibleText_setCaretOffset (AccessibleText *obj,
                               long int newOffset)
{
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval =
    Accessibility_Text_setCaretOffset (CSPI_OBJREF (obj),
				       newOffset, cspi_ev ());

  cspi_return_val_if_ev ("setCaretOffset", FALSE);

  return retval;
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

  if (obj == NULL)
    {
      *startOffset = *endOffset = -1;
      return NULL;
    }

  retval = Accessibility_Text_getTextBeforeOffset (CSPI_OBJREF (obj),
					   offset,
			get_accessible_text_boundary_type (type),
					   &retStartOffset, &retEndOffset,
					   cspi_ev ());
  if (!cspi_check_ev ("getTextBeforeOffset"))
    {
      *startOffset = *endOffset = -1;
      retval = NULL;
    }
  else
    {
      *startOffset = retStartOffset;
      *endOffset = retEndOffset;
    }
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
 **/
char *
AccessibleText_getTextAtOffset (AccessibleText *obj,
				long int offset,
				AccessibleTextBoundaryType type,
				long int *startOffset, long int *endOffset)
{
  CORBA_long corbaStartOffset;
  CORBA_long corbaEndOffset;
  char      *retval;

  if (obj == NULL)
    {
      *startOffset = *endOffset = -1;
      return NULL;
    }

  retval = Accessibility_Text_getTextAtOffset (CSPI_OBJREF (obj),
					       offset,
			  get_accessible_text_boundary_type (type),
					       &corbaStartOffset,
					       &corbaEndOffset,
					       cspi_ev ());

  if (!cspi_check_ev ("getTextAtOffset"))
    {
      *startOffset = *endOffset = -1;
      retval = NULL;
    }
  else
    {
      *startOffset = corbaStartOffset;
      *endOffset   = corbaEndOffset;
    }
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
 **/
char *
AccessibleText_getTextAfterOffset (AccessibleText *obj,
				   long int offset,
				   AccessibleTextBoundaryType type,
				   long int *startOffset, long int *endOffset)
{
  char *retval;
  CORBA_long retStartOffset, retEndOffset;

  if (obj == NULL)
    {
      *startOffset = *endOffset = -1;
      return NULL;
    }

  retval = Accessibility_Text_getTextAfterOffset (CSPI_OBJREF (obj),
					   offset,
			     get_accessible_text_boundary_type (type),
					   &retStartOffset, &retEndOffset,
					   cspi_ev ());

  if (!cspi_check_ev ("getTextAfterOffset"))
    {
      *startOffset = *endOffset = -1;
      retval = NULL;
    }
  else
    {
      *startOffset = retStartOffset;
      *endOffset   = retEndOffset;
    }
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
 **/
unsigned long
AccessibleText_getCharacterAtOffset (AccessibleText *obj,
                                     long int offset)
{
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval =
    Accessibility_Text_getCharacterAtOffset (CSPI_OBJREF (obj),
					     offset,
					     cspi_ev ());

  cspi_return_val_if_ev ("getCharacterAtOffset", -1);

  return retval;
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

  if (obj == NULL)
    {
      *x = *y = -1;
      *width = *height = -1;
      return;
    }

  Accessibility_Text_getCharacterExtents (CSPI_OBJREF (obj),
					  offset,
					  &retX,
					  &retY,
					  &retWidth,
					  &retHeight,
					  type, cspi_ev ());

  if (!cspi_check_ev ("getCharacterExtents"))
    {
      *x = *y = -1;
      *width = *height = -1;
    }
  else
    {
      *x = retX;
      *y = retY;
      *width = retWidth;
      *height = retHeight;
    }
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
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval =
    Accessibility_Text_getOffsetAtPoint (CSPI_OBJREF (obj),
					 x,
					 y,
					 type, cspi_ev ());

  cspi_return_val_if_ev ("getOffsetAtPoint", -1);

  return retval;
}

/**
 * AccessibleText_getRangeExtents:
 * @obj: a pointer to the #AccessibleText object on which to operate.
 * @startOffset: an integer indicating the offset of the first text character for
 *        whom boundary information is requested.
 * @endOffset: an integer indicating the offset of the text character 
 *        after the last character for whom boundary information is requested.
 * @x: a pointer to a long integer into which the nominal x coordinate
 *     of the corresponding bounding box will be returned.
 * @y:a pointer to a long integer into which the nominal y coordinate
 *     of the corresponding bounding box will be returned.
 * @width:a pointer to a long integer into which the width
 *     of the corresponding bounding box will be returned.
 * @height: a pointer to a long integer into which the height
 *     of the corresponding bounding box will be returned.
 * @type: an #AccessibleCoordType indicating the coordinate system to use
 *        for the returned values.
 *
 * Get the bounding box for text within a range in an  #AccessibleText object.
 *
 * @Since: AT-SPI 1.2
 **/
void
AccessibleText_getRangeExtents (AccessibleText *obj,
				long int startOffset,
				long int endOffset,
				long int *x,
				long int *y,
				long int *width,
				long int *height,
				AccessibleCoordType type)
{
  CORBA_long retX, retY, retWidth, retHeight;

  if (obj == NULL)
    {
      *x = *y = -1;
      *width = *height = -1;
      return;
    }

  Accessibility_Text_getRangeExtents (CSPI_OBJREF (obj),
				      startOffset,
				      endOffset,
				      &retX,
				      &retY,
				      &retWidth,
				      &retHeight,
				      type, cspi_ev ());

  if (!cspi_check_ev ("getRangeExtents"))
    {
      *x = *y = -1;
      *width = *height = -1;
    }
  else
    {
      *x = retX;
      *y = retY;
      *width = retWidth;
      *height = retHeight;
    }
}

/**
 * AccessibleText_getBoundedRanges:
 * @obj: a pointer to the #AccessibleText object on which to operate.
 * @x: the 'starting' x coordinate of the bounding box.
 * @y: the 'starting' y coordinate of the bounding box.
 * @width: the x extent of the bounding box.
 * @height: the y extent of the bounding box.
 * @type: an #AccessibleCoordType indicating the coordinate system to use
 *        for the returned values.
 * @clipTypeX: an #AccessibleTextClipType indicating how to treat characters that
 *        intersect the bounding box's x extents.
 * @clipTypeY: an #AccessibleTextClipType indicating how to treat characters that
 *        intersect the bounding box's y extents.
 *
 * Get the ranges of text from an #AccessibleText object which lie within the
 *          bounds defined by (@x, @y) and (@x+@width, @y+@height).  
 *
 * @Since: AT-SPI 1.2
 *
 * Returns: a null-terminated list of pointers to AccessibleTextRange structs 
 *          detailing the bounded text.
 **/
AccessibleTextRange **
AccessibleText_getBoundedRanges (AccessibleText *obj,
				 long int x,
				 long int y,
				 long int width,
				 long int height,
				 AccessibleCoordType type,
				 AccessibleTextClipType clipTypeX,
				 AccessibleTextClipType clipTypeY)
{
  Accessibility_Text_RangeList *range_seq;

  cspi_return_val_if_fail (obj != NULL, NULL);

  range_seq =
    Accessibility_Text_getBoundedRanges (CSPI_OBJREF (obj), 
					 x, y, width, height,
					 type, 
					 get_accessible_text_clip_type (clipTypeX), 
					 get_accessible_text_clip_type (clipTypeY),
					 cspi_ev ());

  cspi_return_val_if_ev ("getBoundedRanges", NULL); 
 
  return get_accessible_text_ranges_from_range_seq (range_seq);
}

/**
 * AccessibleTextRange_freeRanges:
 * @ranges: a pointer to an array of AccessibleTextRange structs.
 *
 * Free the memory used by a list of AccessibleTextRange structs.
 * The argument passed in should be an array of pointers 
 * AccessibleTextRange structs.  
 *
 * @Since: AT-SPI 1.2
 **/
void
AccessibleTextRange_freeRanges (AccessibleTextRange **ranges)
{
  /* this was a contiguously allocated block, only free the first element */
  g_free (ranges[0]); 
  g_free (ranges);
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
 **/
long
AccessibleText_getNSelections (AccessibleText *obj)
{
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval =
    Accessibility_Text_getNSelections (CSPI_OBJREF (obj), cspi_ev ());

  cspi_return_val_if_ev ("getNSelections", -1);

  return retval;
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
 **/
void
AccessibleText_getSelection (AccessibleText *obj,
			     long int selectionNum,
			     long int *startOffset,
			     long int *endOffset)
{
  CORBA_long retStartOffset, retEndOffset;

  if (obj == NULL)
    {
      *endOffset = *startOffset = -1;
      return;
    }

  Accessibility_Text_getSelection (CSPI_OBJREF (obj),
				   selectionNum,
				   &retStartOffset, &retEndOffset,
				   cspi_ev ());

  if (!cspi_check_ev ("getSelection"))
    {
      *startOffset = *endOffset = -1;
    }
  else
    {
      *startOffset = retStartOffset;
      *endOffset   = retEndOffset;
    }
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
 **/
SPIBoolean
AccessibleText_addSelection (AccessibleText *obj,
			     long int startOffset, long int endOffset)
{
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval =
    Accessibility_Text_addSelection (
      CSPI_OBJREF (obj), startOffset,
      endOffset, cspi_ev ());

  cspi_return_val_if_ev ("addSelection", FALSE);

  return retval;
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
 **/
SPIBoolean
AccessibleText_removeSelection (AccessibleText *obj,
				long int selectionNum)
{
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval =
    Accessibility_Text_removeSelection (
      CSPI_OBJREF (obj), selectionNum, cspi_ev ());

  cspi_return_val_if_ev ("removeSelection", FALSE);

  return retval;
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
 **/
SPIBoolean
AccessibleText_setSelection (AccessibleText *obj,
			     long int selectionNum,
			     long int startOffset,
			     long int endOffset)
{
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval = Accessibility_Text_setSelection (CSPI_OBJREF (obj),
				   selectionNum,
				   startOffset,
				   endOffset, cspi_ev ());

  cspi_return_val_if_ev ("setSelection", FALSE);

  return retval;
}


/**
 * AccessibleText_getAttributeRun:
 * @obj: a pointer to the #AccessibleText object to query.
 * @offset: a long integer indicating the offset from which the attribute
 *        search is based.
 * @startOffset: a #long indicating the start of the desired text range.
 * @endOffset: a #long indicating the first character past the desired range.
 * @includeDefaults: a #bool if False, the call should only return those 
 *                 attributes which are explicitly set on the current attribute 
 *                 run, omitting any attributes which are inherited from the 
 *                 default values.
 *
 *  @Since: AT-SPI 1.7
 *
 * Returns: the AttributeSet defined at offset, optionally including the 'default' attributes.
 **/

AccessibleAttributeSet *
AccessibleText_getAttributeRun (AccessibleText *obj,
				long int offset,
				long int *startOffset,
				long int *endOffset,
			        long int includeDefaults){

  CORBA_long retStartOffset, retEndOffset;
  AccessibleAttributeSet *retval;
  Accessibility_AttributeSet *attributes;

  if (obj == NULL)
  {
       *startOffset = *endOffset = -1;
       return NULL;
  }

  attributes = Accessibility_Text_getAttributeRun (CSPI_OBJREF (obj),
					       offset,
					       &retStartOffset,
					       &retEndOffset,
					       (includeDefaults)? TRUE : FALSE,
					       cspi_ev ());

  if (!cspi_check_ev ("getAttributeRun"))
    {
      *startOffset = *endOffset = -1;
      retval = NULL;
    }
  else 
  {
      *startOffset = retStartOffset;
      *endOffset   = retEndOffset;
  }

  retval =  _cspi_attribute_set_from_sequence (attributes);

  return retval;
				     
}

/**
 * AccessibleText_getDefaultAttributeSet:
 * @obj: a pointer to the #AccessibleText object to query.
 *
 *
 *  @Since: AT-SPI 1.7
 *
 * Returns: an AttributeSet containing the text attributes 
 * which apply to all text in the object by virtue of the
 * default settings of the document, view, or user agent; e.g.
 * those attributes which are implied rather than explicitly 
 * applied to the text object. For instance, an object whose 
 * entire text content has been explicitly marked as 'bold' 
 * will report the 'bold' attribute via getAttributeRun(), 
 * whereas an object whose text weight is inspecified may 
 * report the default or implied text weight in the default AttributeSet.
 *
 **/

AccessibleAttributeSet *
AccessibleText_getDefaultAttributeSet (AccessibleText *obj){
   AccessibleAttributeSet *retval;
   Accessibility_AttributeSet *attributes;

   cspi_return_val_if_fail (obj != NULL, NULL);

  attributes = Accessibility_Text_getDefaultAttributeSet (CSPI_OBJREF (obj), cspi_ev ());
  cspi_return_val_if_ev ("getDefaultAttributeSet", NULL);
  
  retval = _cspi_attribute_set_from_sequence (attributes);
  retval = NULL;
  return retval;
}
