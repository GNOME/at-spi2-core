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
 * AccessibleAction_ref:
 * @obj: a pointer to the #AccessibleAction on which to operate.
 *
 * Increment the reference count for an #AccessibleAction.
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
					 i,
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
 *      The keybindings string format is as follows:
 *        there are multiple parts to a keybinding string (typically 3).
 *        They are delimited with ";".  The first is the action's
 *        keybinding which is usable if the object implementing the action
 *        is currently posted to the screen, e.g. if a menu is posted 
 *        then these keybindings for the corresponding menu-items are
 *        available.  The second keybinding substring is the full key sequence
 *        necessary to post the action's widget and activate it, e.g. for
 *        a menu item such as "File->Open" it would both post the menu and
 *        activate the item.  Thus the second keybinding string is available
 *        during the lifetime of the containing toplevel window as a whole,
 *        whereas the first keybinding string only works while the object
 *        implementing AtkAction is posted.  The third (and optional)
 *        keybinding string is the "keyboard shortcut" which invokes the 
 *        action without posting any menus. 
 *        Meta-keys are indicated by the conventional strings
 *        "<Control>", "<Alt>", "<Shift>", "<Mod2>",
 *        etc. (we use the same string as gtk_accelerator_name() in 
 *        gtk+-2.X.
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
       i,
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
				 i,
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

  retval = Accessibility_Action_doAction (CSPI_OBJREF (obj),
				   i,
				   cspi_ev ());

  cspi_return_val_if_ev ("doAction", FALSE);

  return retval;
}
