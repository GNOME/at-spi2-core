int
SpiAccessibleAction_ref (
                      SpiAccessibleAction *obj)
{
  Accessibility_Action_ref (*obj, &ev);
  return 0;
}

int
SpiAccessibleAction_unref (SpiAccessibleAction *obj)
{
  Accessibility_Action_unref (*obj, &ev);
  return 0;
}



long
SpiAccessibleAction_getNActions (SpiAccessibleAction *obj)
{
  return (long)
    Accessibility_Action__get_nActions (*obj, &ev);
}


/**
 * SpiAccessibleAction_getDescription:
 * @obj: a pointer to the #AccessibleAction to query.
 *
 * Get the description of 'i-th' action invokable on an
 *      object implementing #AccessibleAction.
 *
 * Not Yet Implemented.
 *
 * Returns: a UTF-8 string describing the 'i-th' invokable action.
 *
 **/
char *
SpiAccessibleAction_getDescription (SpiAccessibleAction *obj,
                                 long index)
{
  return (char *)
    Accessibility_Action_getDescription (*obj,
					 (CORBA_long) index,
					 &ev);
}



char *
SpiAccessibleAction_getKeyBinding (SpiAccessibleAction *obj,
				long index)
{
  return (char *) 
    Accessibility_Action_getKeyBinding (*obj,
       (CORBA_long) index,
       &ev);
}



char *
SpiAccessibleAction_getName (SpiAccessibleAction *obj,
			  long index)
{
  return (char *)
				   Accessibility_Action_getName (*obj,
								 (CORBA_long) index,
								 &ev);
}


boolean
SpiAccessibleAction_doAction (SpiAccessibleAction *obj,
                           long index)
{
  return (boolean)
    Accessibility_Action_doAction (*obj,
				   (CORBA_long) index,
				   &ev);
}


