int
AccessibleEditableText_ref (AccessibleEditableText *obj)
{
  Accessibility_EditableText_ref (*obj, &ev);
  return 0;
}



int
AccessibleEditableText_unref (AccessibleEditableText *obj)
{
  Accessibility_EditableText_unref (*obj, &ev);
  return 0;
}



boolean
AccessibleEditableText_setAttributes (AccessibleEditableText *obj,
				      const char *attributes,
				      long startPos, long endPos)
{
  return (boolean)
    Accessibility_EditableText_setAttributes (*obj,
					      (CORBA_char *) attributes,
					      (CORBA_long) startPos, (CORBA_long) endPos, &ev);
}



void
AccessibleEditableText_setTextContents (AccessibleEditableText *obj,
                                        const char *newContents)
{
  Accessibility_EditableText_setTextContents (*obj,
					      (CORBA_char *) newContents, &ev);
}



void
AccessibleEditableText_insertText (AccessibleEditableText *obj,
                                   long position,
                                   char *text,
                                   long length)
{
  Accessibility_EditableText_insertText (*obj,
					 (CORBA_long) position, (CORBA_char *) text,
					 (CORBA_long) length, &ev);
}



void
AccessibleEditableText_copyText (AccessibleText *obj,
                                 long startPos,
                                 long endPos)
{
  Accessibility_EditableText_copyText (*obj,
				       (CORBA_long) startPos, (CORBA_long) endPos, &ev);
}



void
AccessibleEditableText_cutText (AccessibleEditableText *obj,
                                long startPos,
                                long endPos)
{
  Accessibility_EditableText_cutText (*obj,
				       (CORBA_long) startPos, (CORBA_long) endPos, &ev);
}



void
AccessibleEditableText_deleteText (AccessibleEditableText *obj,
                                   long startPos,
                                   long endPos)
{
  Accessibility_EditableText_deleteText (*obj,
				       (CORBA_long) startPos, (CORBA_long) endPos, &ev);
}



void
AccessibleEditableText_pasteText (AccessibleEditableText *obj,
                                  long position)
{
  Accessibility_EditableText_pasteText (*obj,
					(CORBA_long) position, &ev);
}

