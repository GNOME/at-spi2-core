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

/**
 * AccessibleEditableText_ref:
 * @obj: a pointer to the #AccessibleEditableText object on which to operate.
 *
 * Increment the reference count for an #AccessibleEditableText object.
 *       Since AccessibleEditableText is derived from AccessibleText,
 *       this is the same as AccessibleText_ref().
 **/
void
AccessibleEditableText_ref (AccessibleEditableText *obj)
{
  cspi_object_ref (obj);
}

/**
 * AccessibleEditableText_unref:
 * @obj: a pointer to the #AccessibleEditableText object on which to operate.
 *
 * Decrement the reference count for an #AccessibleEdiitableText object.
 *       Since AccessibleEditableText is derived from AccessibleText,
 *       this is the same as AccessibleText_unref().
 **/
void
AccessibleEditableText_unref (AccessibleEditableText *obj)
{
  cspi_object_unref (obj);
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
 **/
SPIBoolean
AccessibleEditableText_setAttributes (AccessibleEditableText *obj,
				      const char *attributes,
				      long int startPos,
				      long int endPos)
{
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval = 
    Accessibility_EditableText_setAttributes (CSPI_OBJREF (obj),
					      attributes,
					      startPos,
					      endPos, cspi_ev ());

  cspi_return_val_if_ev ("setAttributes", FALSE);

  return retval;
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
 **/
SPIBoolean
AccessibleEditableText_setTextContents (AccessibleEditableText *obj,
                                        const char *newContents)
{
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval = Accessibility_EditableText_setTextContents (CSPI_OBJREF (obj),
						                          newContents, cspi_ev ());

  cspi_return_val_if_ev ("setTextContents", FALSE);

  return retval;
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
 **/
SPIBoolean
AccessibleEditableText_insertText (AccessibleEditableText *obj,
                                   long int position,
                                   const char *text,
                                   long int length)
{
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval = Accessibility_EditableText_insertText (CSPI_OBJREF (obj),
					 position, text,
					 length, cspi_ev ());

  cspi_return_val_if_ev ("insertText", FALSE);

  return retval;
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
 **/
SPIBoolean
AccessibleEditableText_copyText (AccessibleText *obj,
                                 long int startPos,
                                 long int endPos)
{
  cspi_return_val_if_fail (obj != NULL, FALSE);

  Accessibility_EditableText_copyText (
    CSPI_OBJREF (obj), startPos,
    endPos, cspi_ev ());

  cspi_return_val_if_ev ("copyText", FALSE);

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
 **/
SPIBoolean
AccessibleEditableText_cutText (AccessibleEditableText *obj,
                                long int startPos,
                                long int endPos)
{
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval = Accessibility_EditableText_cutText (
    CSPI_OBJREF (obj),
    startPos, endPos, cspi_ev ());

  cspi_return_val_if_ev ("cutText", FALSE);

  return retval;
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
 **/
SPIBoolean
AccessibleEditableText_deleteText (AccessibleEditableText *obj,
                                   long startPos,
                                   long endPos)
{
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval = Accessibility_EditableText_deleteText (
    CSPI_OBJREF (obj),
    startPos, endPos, cspi_ev ());

  cspi_return_val_if_ev ("deleteText", FALSE);

  return retval;
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
 **/
SPIBoolean
AccessibleEditableText_pasteText (AccessibleEditableText *obj,
                                  long int position)
{
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval = Accessibility_EditableText_pasteText (
    CSPI_OBJREF (obj), position, cspi_ev ());

  cspi_return_val_if_ev ("pasteText", FALSE);

  return retval;
}
