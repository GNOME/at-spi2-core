int
AccessibleText_ref (AccessibleText *obj)
{
  Accessibility_Text_ref (*obj, &ev);
  return 0;
}



int
AccessibleText_unref (AccessibleText *obj)
{
  Accessibility_Text_unref (*obj, &ev);
  return 0;
}



long
AccessibleText_getCharacterCount (AccessibleText *obj)
{
  long retval;

  CORBA_exception_init (&ev);
  retval = (long)
    Accessibility_Text__get_characterCount (*obj, &ev);

  spi_check_ev (&ev, "AccessibleText_getCharacterCount");

  return retval;
}



char *
AccessibleText_getText (AccessibleText *obj,
                        long startOffset,
                        long endOffset)
{
  return (char *)
    Accessibility_Text_getText (*obj,
				(CORBA_long) startOffset, (CORBA_long) endOffset, &ev);
}




long
AccessibleText_getCaretOffset (AccessibleText *obj)
{
  return (long)
    Accessibility_Text__get_caretOffset (*obj, &ev);
}



char *
AccessibleText_getAttributes (AccessibleText *obj,
				 long offset,
				 long *startOffset,
				 long *endOffset)
{
  CORBA_long retStartOffset, retEndOffset;
  char *retval;	
  retval = (char *)
    Accessibility_Text_getAttributes (*obj,
				      (CORBA_long) offset,
				      &retStartOffset,
				      &retEndOffset,
				      &ev);
  *startOffset = (long) retStartOffset;
  *endOffset = (long) retEndOffset;
  return retval;
}



boolean
AccessibleText_setCaretOffset (AccessibleText *obj,
                               long newOffset)
{
  return (boolean)
    Accessibility_Text_setCaretOffset (*obj,
				       (CORBA_long) newOffset, &ev);
}



char *
AccessibleText_getTextBeforeOffset (AccessibleText *obj,
                                    long offset,
                                    TEXT_BOUNDARY_TYPE type,
				    long *startOffset, long *endOffset)
{
  char *retval;
  CORBA_long retStartOffset, retEndOffset;
  retval = (char *)
    Accessibility_Text_getTextBeforeOffset (*obj,
					   (CORBA_long) offset, (Accessibility_TEXT_BOUNDARY_TYPE) type,
					   &retStartOffset, &retEndOffset,
					   &ev);
  *startOffset = (long) retStartOffset;
  *endOffset = (long) retEndOffset;
  return retval;
}



char *
AccessibleText_getTextAtOffset (AccessibleText *obj,
				long offset,
				TEXT_BOUNDARY_TYPE type,
				long *startOffset, long *endOffset)
{
  CORBA_long corbaStartOffset;
  CORBA_long corbaEndOffset;
  char *retval = "";
  retval = Accessibility_Text_getTextAtOffset (*obj,
					       (CORBA_long) offset,
					       (Accessibility_TEXT_BOUNDARY_TYPE) type,
					       &corbaStartOffset,
					       &corbaEndOffset,
					       &ev);
  *startOffset = (long) corbaStartOffset;
  *endOffset = (long) corbaEndOffset;
#ifdef SPI_DEBUG
  fprintf (stderr, "text offsets %ld to %ld\n", *startOffset, *endOffset);
#endif
  return retval;
}



char *
AccessibleText_getTextAfterOffset (AccessibleText *obj,
                                    long offset,
                                    TEXT_BOUNDARY_TYPE type,
				    long *startOffset, long *endOffset)
{
  char *retval;
  CORBA_long retStartOffset, retEndOffset;
  retval = (char *)
    Accessibility_Text_getTextAfterOffset (*obj,
					   (CORBA_long) offset, (Accessibility_TEXT_BOUNDARY_TYPE) type,
					   &retStartOffset, &retEndOffset,
					   &ev);
  *startOffset = (long) retStartOffset;
  *endOffset = (long) retEndOffset;
  return retval;
}



unsigned long
AccessibleText_getCharacterAtOffset (AccessibleText *obj,
                                     long offset)
{
  return (unsigned long)
    Accessibility_Text_getCharacterAtOffset (*obj,
					     (CORBA_long) offset, &ev);
}



void
AccessibleText_getCharacterExtents (AccessibleText *obj,
                                    long offset,
                                    long *x,
                                    long *y,
                                    long *width,
                                    long *height,
				    AccessibleCoordType type)
{
  CORBA_long retX, retY, retWidth, retHeight;
  Accessibility_Text_getCharacterExtents (*obj,
					  (CORBA_long) offset,
					  &retX,
					  &retY,
					  &retWidth,
					  &retHeight,
					  (CORBA_short) type, &ev);
  *x = (long) retX;
  *y = (long) retY;
  *width = (long) retWidth;
  *height = (long) retHeight;
}



long
AccessibleText_getOffsetAtPoint (AccessibleText *obj,
                                 long x,
                                 long y,
				 AccessibleCoordType type)
{
  return (long)
    Accessibility_Text_getOffsetAtPoint (*obj,
					 (CORBA_long) x, (CORBA_long) y, (CORBA_short) type, &ev);
}



long
AccessibleText_getNSelections (AccessibleText *obj)
{
  return (long)
    Accessibility_Text_getNSelections (*obj, &ev);
}



void
AccessibleText_getSelection (AccessibleText *obj,
			     long selectionNum, long *startOffset,
			     long *endOffset)
{
  CORBA_long retStartOffset, retEndOffset;
  Accessibility_Text_getSelection (*obj,
				   (CORBA_long) selectionNum,
				   &retStartOffset, &retEndOffset, &ev);
  
  *startOffset = (long) retStartOffset;
  *endOffset = (long) retEndOffset;
}



boolean
AccessibleText_addSelection (AccessibleText *obj,
			     long startOffset, long endOffset)
{
  return (boolean)
    Accessibility_Text_addSelection (*obj,
				     (CORBA_long) startOffset, (CORBA_long) endOffset,
				     &ev);
}



boolean
AccessibleText_removeSelection (AccessibleText *obj,
				long selectionNum)
{
  return (boolean)
    Accessibility_Text_removeSelection (*obj,
					(CORBA_long) selectionNum, &ev);
}



boolean
AccessibleText_setSelection (AccessibleText *obj,
			     long selectionNum,
			     long startOffset,
			     long endOffset)
{
  return (boolean)
    Accessibility_Text_setSelection (*obj,
				     (CORBA_long) selectionNum,
				     (CORBA_long) startOffset,
				     (CORBA_long) endOffset, &ev);
}



