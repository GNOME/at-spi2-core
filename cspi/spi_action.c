#include <cspi/spi-private.h>

/**
 * AccessibleAction_ref:
 * @obj: a pointer to the #AccessibleAction on which to operate.
 *
 * Increment the reference count for an #AccessibleAction.
 *
 **/
void
AccessibleAction_ref (AccessibleAction *obj)
{
  cspi_object_ref (obj);
}

/**
 * AccessibleAction_unref:
 * @obj: a pointer to the #AccessibleAction on which to operate.
 *
 * Decrement the reference count for an #AccessibleAction.
 *
 **/
void
AccessibleAction_unref (AccessibleAction *obj)
{
  cspi_object_unref (obj);
}

/**
 * AccessibleAction_getNActions:
 * @obj: a pointer to the #AccessibleAction to query.
 *
 * Get the number of actions invokable on an #AccessibleAction implementor.
 *
 * Returns: a #long integer indicatin the number of invokable actions.
 **/
long
AccessibleAction_getNActions (AccessibleAction *obj)
{
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval =
    Accessibility_Action__get_nActions (CSPI_OBJREF (obj), cspi_ev ());

  cspi_return_val_if_ev ("getNActions", -1);

  return retval;
}

/**
 * AccessibleAction_getDescription:
 * @obj: a pointer to the #AccessibleAction implementor to query.
 * @i: a long integer indicating which action to query.
 *
 * Get the description of '@i-th' action invokable on an
 *      object implementing #AccessibleAction.
 *
 * Returns: a UTF-8 string describing the '@i-th' invokable action.
 **/
char *
AccessibleAction_getDescription (AccessibleAction *obj,
                                 long int          i)
{
  char *retval;
  cspi_return_val_if_fail (obj != NULL, NULL);

  retval =
    Accessibility_Action_getDescription (CSPI_OBJREF (obj),
					 (CORBA_long) i,
					 cspi_ev ());

  cspi_return_val_if_ev ("getDescription", NULL);

  return retval;
}

/**
 * AccessibleAction_getKeyBinding:
 * @obj: a pointer to the #AccessibleAction implementor to query.
 * @i: a long integer indicating which action to query.
 *
 * Get the keybindings for the @i-th action invokable on an
 *      object implementing #AccessibleAction, if any are defined.
 *
 * Returns: a UTF-8 string which can be parsed to determine the @i-th
 *       invokable action's keybindings.
 **/
char *
AccessibleAction_getKeyBinding (AccessibleAction *obj,
				long int          i)
{
  char *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  retval =
    Accessibility_Action_getKeyBinding (CSPI_OBJREF (obj),
       (CORBA_long) i,
       cspi_ev ());

  cspi_return_val_if_ev ("getKeyBinding", NULL);

  return retval;
}

/**
 * AccessibleAction_getName:
 * @obj: a pointer to the #AccessibleAction implementor to query.
 * @i: a long integer indicating which action to query.
 *
 * Get the name of the '@i-th' action invokable on an
 *      object implementing #AccessibleAction.
 *
 * Returns: the 'event type' name of the action, as a UTF-8 string.
 **/
char *
AccessibleAction_getName (AccessibleAction *obj,
			  long int          i)
{
  char *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  retval =
   Accessibility_Action_getName (CSPI_OBJREF (obj),
				 (CORBA_long) i,
				 cspi_ev ());

  cspi_return_val_if_ev ("getName", NULL);

  return retval;
}

/**
 * AccessibleAction_doAction:
 * @obj: a pointer to the #AccessibleAction to query.
 * @i: an integer specifying which action to invoke.
 *
 * Invoke the action indicated by #index.
 *
 * Returns: #TRUE if the action is successfully invoked, otherwise #FALSE.
 **/
SPIBoolean
AccessibleAction_doAction (AccessibleAction *obj,
                           long int i)
{
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval = (SPIBoolean)
    Accessibility_Action_doAction (CSPI_OBJREF (obj),
				   (CORBA_long) i,
				   cspi_ev ());

  cspi_return_val_if_ev ("doAction", FALSE);

  return retval;
}
