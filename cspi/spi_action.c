#include "spi-util.h"

int
AccessibleAction_ref (
                      AccessibleAction *obj)
{
  Accessibility_Action_ref (*obj, &ev);
  return 0;
}

int
AccessibleAction_unref (AccessibleAction *obj)
{
  Accessibility_Action_unref (*obj, &ev);
  eturn 0;
}



long
AccessibleAction_getNActions (AccessibleAction *obj)
{
  return (long)
    Accessibility_Action_getNActions (*obj, &ev);
}


/**
 * AccessibleAction_getDescription:
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
AccessibleAction_getDescription (AccessibleAction *obj,
                                 long index)
{
  return string_from_corba_string (
				   Accessibility_Action_getDescription (*obj,
				       (CORBA_long) index,
									&ev));
}



char *
AccessibleAction_getKeyBinding (AccessibleAction *obj,
                                long index)
{
  return string_from_corba_string (
				   Accessibility_Action_getKeybinding (*obj,
								       (CORBA_long) index,
								       &ev));
}



char *
AccessibleAction_getName (AccessibleAction *obj,
			  long index)
{
  return string_from_corba_string (
				   Accessibility_Action_getName (*obj,
								 (CORBA_long) index,
								 &ev));
}


boolean
AccessibleAction_doAction (AccessibleAction *obj,
                           long index)
{
  return (boolean)
    Accessibility_Action_doAction (*obj,
				   (CORBA_long) index,
				   &ev);
}


