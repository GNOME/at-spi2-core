int
SpiAccessibleText_ref (SpiAccessibleText *obj)
{
  Accessibility_SpiText_ref (*obj, &ev);
  return 0;
}



int
SpiAccessibleText_unref (SpiAccessibleText *obj)
{
  Accessibility_SpiText_unref (*obj, &ev);
  return 0;
}



long
SpiAccessibleText_getCharacterCount (SpiAccessibleText *obj)
{
  long retval;

  CORBA_exception_init (&ev);
  retval = (long)
    Accessibility_SpiText__get_characterCount (*obj, &ev);

  spi_check_ev (&ev, "SpiAccessibleText_getCharacterCount");

  return retval;
}



char *
SpiAccessibleText_getText (SpiAccessibleText *obj,
                        long startOffset,
                        long endOffset)
{
  return (char *)
    Accessibility_SpiText_getText (*obj,
				(CORBA_long) startOffset, (CORBA_long) endOffset, &ev);
}




long
SpiAccessibleText_getCaretOffset (SpiAccessibleText *obj)
{
  return (long)
    Accessibility_SpiText__get_caretOffset (*obj, &ev);
}



char *
SpiAccessibleText_getAttributes (SpiAccessibleText *obj,
				 long offset,
				 long *startOffset,
				 long *endOffset)
{
  CORBA_long retStartOffset, retEndOffset;
  char *retval;	
  retval = (char *)
    Accessibility_SpiText_getAttributes (*obj,
				      (CORBA_long) offset,
				      &retStartOffset,
				      &retEndOffset,
				      &ev);
  *startOffset = (long) retStartOffset;
  *endOffset = (long) retEndOffset;
  return retval;
}



boolean
SpiAccessibleText_setCaretOffset (SpiAccessibleText *obj,
                               long newOffset)
{
  return (boolean)
    Accessibility_SpiText_setCaretOffset (*obj,
				       (CORBA_long) newOffset, &ev);
}



char *
SpiAccessibleText_getTextBeforeOffset (SpiAccessibleText *obj,
                                    long offset,
                                    SPI_TEXT_BOUNDARY_TYPE type,
				    long *startOffset, long *endOffset)
{
  char *retval;
  CORBA_long retStartOffset, retEndOffset;
  retval = (char *)
    Accessibility_SpiText_getTextBeforeOffset (*obj,
					   (CORBA_long) offset, (Accessibility_TEXT_BOUNDARY_TYPE) type,
					   &retStartOffset, &retEndOffset,
					   &ev);
  *startOffset = (long) retStartOffset;
  *endOffset = (long) retEndOffset;
  return retval;
}



char *
SpiAccessibleText_getTextAtOffset (SpiAccessibleText *obj,
				long offset,
				SPI_TEXT_BOUNDARY_TYPE type,
				long *startOffset, long *endOffset)
{
  CORBA_long corbaStartOffset;
  CORBA_long corbaEndOffset;
  char *retval = "";
  retval = Accessibility_SpiText_getTextAtOffset (*obj,
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
SpiAccessibleText_getTextAfterOffset (SpiAccessibleText *obj,
                                    long offset,
                                    SPI_TEXT_BOUNDARY_TYPE type,
				    long *startOffset, long *endOffset)
{
  char *retval;
  CORBA_long retStartOffset, retEndOffset;
  retval = (char *)
    Accessibility_SpiText_getTextAfterOffset (*obj,
					   (CORBA_long) offset, (Accessibility_TEXT_BOUNDARY_TYPE) type,
					   &retStartOffset, &retEndOffset,
					   &ev);
  *startOffset = (long) retStartOffset;
  *endOffset = (long) retEndOffset;
  return retval;
}



unsigned long
SpiAccessibleText_getCharacterAtOffset (SpiAccessibleText *obj,
                                     long offset)
{
  return (unsigned long)
    Accessibility_SpiText_getCharacterAtOffset (*obj,
					     (CORBA_long) offset, &ev);
}



void
SpiAccessibleText_getCharacterExtents (SpiAccessibleText *obj,
                                    long offset,
                                    long *x,
                                    long *y,
                                    long *width,
                                    long *height,
				    SpiAccessibleCoordType type)
{
  CORBA_long retX, retY, retWidth, retHeight;
  Accessibility_SpiText_getCharacterExtents (*obj,
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
SpiAccessibleText_getOffsetAtPoint (SpiAccessibleText *obj,
                                 long x,
                                 long y,
				 SpiAccessibleCoordType type)
{
  return (long)
    Accessibility_SpiText_getOffsetAtPoint (*obj,
					 (CORBA_long) x, (CORBA_long) y, (CORBA_short) type, &ev);
}



long
SpiAccessibleText_getNSpiSelections (SpiAccessibleText *obj)
{
  return (long)
    Accessibility_SpiText_getNSpiSelections (*obj, &ev);
}



void
SpiAccessibleText_getSelection (SpiAccessibleText *obj,
			     long selectionNum, long *startOffset,
			     long *endOffset)
{
  CORBA_long retStartOffset, retEndOffset;
  Accessibility_SpiText_getSelection (*obj,
				   (CORBA_long) selectionNum,
				   &retStartOffset, &retEndOffset, &ev);
  
  *startOffset = (long) retStartOffset;
  *endOffset = (long) retEndOffset;
}



boolean
SpiAccessibleText_addSpiSelection (SpiAccessibleText *obj,
			     long startOffset, long endOffset)
{
  return (boolean)
    Accessibility_SpiText_addSpiSelection (*obj,
				     (CORBA_long) startOffset, (CORBA_long) endOffset,
				     &ev);
}



boolean
SpiAccessibleText_removeSelection (SpiAccessibleText *obj,
				long selectionNum)
{
  return (boolean)
    Accessibility_SpiText_removeSelection (*obj,
					(CORBA_long) selectionNum, &ev);
}



boolean
SpiAccessibleText_setSelection (SpiAccessibleText *obj,
			     long selectionNum,
			     long startOffset,
			     long endOffset)
{
  return (boolean)
    Accessibility_SpiText_setSelection (*obj,
				     (CORBA_long) selectionNum,
				     (CORBA_long) startOffset,
				     (CORBA_long) endOffset, &ev);
}



