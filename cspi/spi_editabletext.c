int
SpiAccessibleEditableText_ref (SpiAccessibleEditableText *obj)
{
  Accessibility_EditableText_ref (*obj, &ev);
  return 0;
}



int
SpiAccessibleEditableText_unref (SpiAccessibleEditableText *obj)
{
  Accessibility_EditableText_unref (*obj, &ev);
  return 0;
}



boolean
SpiAccessibleEditableText_setAttributes (SpiAccessibleEditableText *obj,
					 const char *attributes,
					 long startPos, long endPos)
{
  return (boolean)
    Accessibility_EditableText_setAttributes (*obj,
						 (CORBA_char *) attributes,
						 (CORBA_long) startPos, (CORBA_long) endPos, &ev);
}



void
SpiAccessibleEditableText_setTextContents (SpiAccessibleEditableText *obj,
                                        const char *newContents)
{
  Accessibility_EditableText_setTextContents (*obj,
					      (CORBA_char *) newContents, &ev);
}



void
SpiAccessibleEditableText_insertText (SpiAccessibleEditableText *obj,
                                   long position,
                                   char *text,
                                   long length)
{
  Accessibility_EditableText_insertText (*obj,
					 (CORBA_long) position, (CORBA_char *) text,
					 (CORBA_long) length, &ev);
}



void
SpiAccessibleEditableText_copyText (SpiAccessibleText *obj,
                                 long startPos,
                                 long endPos)
{
  Accessibility_EditableText_copyText (*obj,
				       (CORBA_long) startPos, (CORBA_long) endPos, &ev);
}



void
SpiAccessibleEditableText_cutText (SpiAccessibleEditableText *obj,
                                long startPos,
                                long endPos)
{
  Accessibility_EditableText_cutText (*obj,
				       (CORBA_long) startPos, (CORBA_long) endPos, &ev);
}



void
SpiAccessibleEditableText_deleteText (SpiAccessibleEditableText *obj,
                                   long startPos,
                                   long endPos)
{
  Accessibility_EditableText_deleteText (*obj,
				       (CORBA_long) startPos, (CORBA_long) endPos, &ev);
}



void
SpiAccessibleEditableText_pasteText (SpiAccessibleEditableText *obj,
                                  long position)
{
  Accessibility_EditableText_pasteText (*obj,
					(CORBA_long) position, &ev);
}

