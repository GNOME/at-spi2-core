int
SpiAccessibleAction_ref (
                      SpiAccessibleAction *obj)
{
  Accessibility_SpiAction_ref (*obj, &ev);
  return 0;
}

int
SpiAccessibleAction_unref (SpiAccessibleAction *obj)
{
  Accessibility_SpiAction_unref (*obj, &ev);
  return 0;
}



long
SpiAccessibleAction_getNSpiActions (SpiAccessibleAction *obj)
{
  return (long)
    Accessibility_SpiAction__get_nSpiActions (*obj, &ev);
}


/**
 * SpiAccessibleAction_getDescription:
 * @obj: a pointer to the #SpiAccessibleAction to query.
 *
 * Get the description of 'i-th' action invokable on an
 *      object implementing #SpiAccessibleAction.
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
    Accessibility_SpiAction_getDescription (*obj,
					 (CORBA_long) index,
					 &ev);
}



char *
SpiAccessibleAction_getKeyBinding (SpiAccessibleAction *obj,
				long index)
{
  return (char *) 
    Accessibility_SpiAction_getKeyBinding (*obj,
       (CORBA_long) index,
       &ev);
}



char *
SpiAccessibleAction_getName (SpiAccessibleAction *obj,
			  long index)
{
  return (char *)
				   Accessibility_SpiAction_getName (*obj,
								 (CORBA_long) index,
								 &ev);
}


boolean
SpiAccessibleAction_doSpiAction (SpiAccessibleAction *obj,
                           long index)
{
  return (boolean)
    Accessibility_SpiAction_doSpiAction (*obj,
				   (CORBA_long) index,
				   &ev);
}


