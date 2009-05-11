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

typedef struct
{
  dbus_int32_t startOffset;
  dbus_int32_t endOffset;
  char *content;
} Accessibility_Range;

static AccessibleTextRange **
get_accessible_text_ranges_from_range_seq (GArray *range_seq)
{
  AccessibleTextRange **ranges = NULL;
  AccessibleTextRange *array = NULL;
  int i;
  if (range_seq && range_seq->len > 0) 
    {
      ranges = g_new0 (AccessibleTextRange *, range_seq->len + 1);
    }
  array = g_new0 (AccessibleTextRange, range_seq->len);
  for (i = 0; i < range_seq->len; i++) 
    {
      Accessibility_Range *r = g_array_index (range_seq, Accessibility_Range *, i);
      AccessibleTextRange *range;
      range = &array[i];
      range->start = r->startOffset;
      range->end = r->endOffset;
      range->contents = g_strdup (r->content);
      ranges[i] = range;
    }
  ranges[i] = NULL; /* null-terminated list! */
  // TODO: Figure out whether we're leaking strings
  g_array_free (range_seq, TRUE);

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
  dbus_int32_t retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  cspi_dbus_get_property (obj, spi_interface_text, "characterCount", NULL, "i", &retval);

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
  dbus_int32_t d_startOffset = startOffset, d_endOffset = endOffset;

  cspi_return_val_if_fail (obj != NULL, NULL);

  cspi_dbus_call (obj, spi_interface_text, "getText", NULL, "ii=>s", startOffset, endOffset, &retval);

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
  dbus_int32_t retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  cspi_dbus_get_property (obj, spi_interface_text, "caretOffset", NULL, "i", &retval);

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
  dbus_int32_t d_offset = offset;
  dbus_int32_t retStartOffset, retEndOffset;
  char *retval;	

  if (obj == NULL)
    {
      *startOffset = *endOffset = -1;
      return NULL;
    }

  cspi_dbus_call (obj, spi_interface_text, "getAttributes", NULL, "i=>sii", d_offset, &retval, &retStartOffset, &retEndOffset);

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

  cspi_dbus_call (obj, spi_interface_text, "getAttributes", NULL, "=>s", &retval);

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
  dbus_int32_t d_newOffset = newOffset;
  dbus_bool_t retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  cspi_dbus_call (obj, spi_interface_text, "setCaretOffset", NULL, "i=>b", d_newOffset, &retval);

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
  dbus_int32_t d_offset = offset;
  dbus_uint32_t d_type = type;
  dbus_int32_t retStartOffset, retEndOffset;
  char *retval;

  if (obj == NULL)
    {
      *startOffset = *endOffset = -1;
      return NULL;
    }

  cspi_dbus_call (obj, spi_interface_text, "getTextBeforeOffset", NULL, "iu=>sii", d_offset, d_type, &retval, &retStartOffset, &retEndOffset);
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
  dbus_int32_t d_offset = offset;
  dbus_uint32_t d_type = type;
  dbus_int32_t retStartOffset, retEndOffset;
  char *retval;

  if (obj == NULL)
    {
      *startOffset = *endOffset = -1;
      return NULL;
    }

  cspi_dbus_call (obj, spi_interface_text, "getTextAtOffset", NULL, "iu=>sii", d_offset, d_type, &retval, &retStartOffset, &retEndOffset);

  if (!cspi_check_ev ("getTextAtOffset"))
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
  dbus_int32_t d_offset = offset;
  dbus_uint32_t d_type = type;
  dbus_int32_t retStartOffset, retEndOffset;
  char *retval;

  if (obj == NULL)
    {
      *startOffset = *endOffset = -1;
      return NULL;
    }

  cspi_dbus_call (obj, spi_interface_text, "getTextAfterOffset", NULL, "iu=>sii", d_offset, d_type, &retval, &retStartOffset, &retEndOffset);

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
  dbus_int32_t d_offset = offset;
  dbus_int32_t retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  cspi_dbus_call (obj, spi_interface_text, "getCharacterAtOffset", NULL, "i=>i", d_offset, &retval);

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
  dbus_int32_t d_offset = offset;
  dbus_uint16_t d_type = type;
  dbus_int32_t retX, retY, retWidth, retHeight;

  if (obj == NULL)
    {
      *x = *y = -1;
      *width = *height = -1;
      return;
    }

  cspi_dbus_call (obj, spi_interface_text, "getCharacterExtents", NULL, "in=>iiii", d_offset, d_type, &retX, &retY, &retWidth, &retHeight);

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
  dbus_int32_t d_x = x, d_y = y;
  dbus_uint16_t d_type = type;
  dbus_int32_t retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  cspi_dbus_call (obj, spi_interface_text, "getOffsetAtPoint", NULL, "iin=>i", d_x, d_y, d_type, &retval);

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
  dbus_int32_t d_startOffset = startOffset, d_endOffset = endOffset;
  dbus_int16_t d_type = type;
  dbus_int32_t retX, retY, retWidth, retHeight;

  if (obj == NULL)
    {
      *x = *y = -1;
      *width = *height = -1;
      return;
    }

  cspi_dbus_call (obj, spi_interface_text, "getRangeExtents", NULL, "iin=>iiii", d_startOffset, d_endOffset, d_type, &retX, &retY, &retWidth, &retHeight);

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
  dbus_int32_t d_x = x, d_y = y, d_width = width, d_height = height;
  dbus_uint16_t d_type = type;
  dbus_uint32_t d_clipTypeX = clipTypeX, d_clipTypeY = clipTypeY;
  GArray *range_seq;

  cspi_return_val_if_fail (obj != NULL, NULL);

  cspi_dbus_call (obj, spi_interface_text, "getBoundedRanges", NULL, "iiiinuu=>a(iisv)", d_x, d_y, d_width, d_height, d_type, d_clipTypeX, d_clipTypeY, &range_seq);

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
  dbus_int32_t retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  cspi_dbus_call (obj, spi_interface_text, "getNSelections", NULL, "i", &retval);

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
  dbus_int32_t d_selectionNum = selectionNum;
  dbus_int32_t retStartOffset, retEndOffset;

  if (obj == NULL)
    {
      *endOffset = *startOffset = -1;
      return;
    }

  cspi_dbus_call (obj, spi_interface_text, "getSelection", NULL, "i=>ii", d_selectionNum, &retStartOffset, &retEndOffset);

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
  dbus_int32_t d_startOffset = startOffset, d_endOffset = endOffset;
  dbus_bool_t retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  cspi_dbus_call (obj, spi_interface_text, "addSelection", NULL, "ii=>b", d_startOffset, d_endOffset, &retval);

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
  dbus_int32_t d_selectionNum = selectionNum;
  dbus_bool_t retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  cspi_dbus_call (obj, spi_interface_text, "removeSelection", NULL, "i=>b", d_selectionNum, &retval);

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
  dbus_int32_t d_selectionNum = selectionNum, d_startOffset = startOffset, d_endOffset = endOffset;
  dbus_bool_t retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  cspi_dbus_call (obj, spi_interface_text, "setSelection", NULL, "iii=>b", d_selectionNum, d_startOffset, d_endOffset, &retval);

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

  dbus_int32_t d_offset = offset;
  dbus_bool_t d_includeDefaults = includeDefaults;
  dbus_int32_t retStartOffset, retEndOffset;
  AccessibleAttributeSet *retval;
  GArray *attributes;

  if (obj == NULL)
  {
       *startOffset = *endOffset = -1;
       return NULL;
  }

  cspi_dbus_call (obj, spi_interface_text, "getAttributeRun", NULL, "ib=>asii", d_offset, d_includeDefaults, &attributes, &retStartOffset, &retEndOffset);

  if (!cspi_check_ev ("getAttributeRun"))
    {
      *startOffset = *endOffset = -1;
      retval = NULL;
    }
  else 
  {
      *startOffset = retStartOffset;
      *endOffset   = retEndOffset;
      retval =  _cspi_attribute_set_from_sequence (attributes);
  }

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
   GArray *attributes;

   cspi_return_val_if_fail (obj != NULL, NULL);

  cspi_dbus_call (obj, spi_interface_text, "getDefaultAttributes", NULL, "as", &attributes);
  
  retval = _cspi_attribute_set_from_sequence (attributes);
  return retval;
}
