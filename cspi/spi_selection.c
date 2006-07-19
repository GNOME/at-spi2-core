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
 * AccessibleSelection_ref:
 * @obj: a pointer to the #AccessibleSelection implementor on which to operate.
 *
 * Increment the reference count for an #AccessibleSelection object.
 *
 **/
void
AccessibleSelection_ref (AccessibleSelection *obj)
{
  cspi_object_ref (obj);
}

/**
 * AccessibleSelection_unref:
 * @obj: a pointer to the #AccessibleSelection implementor on which to operate. 
 *
 * Decrement the reference count for an #Accessible object.
 *
 **/
void
AccessibleSelection_unref (AccessibleSelection *obj)
{
  cspi_object_unref (obj);
}

/**
 * AccessibleSelection_getNSelectedChildren:
 * @obj: a pointer to the #AccessibleSelection implementor on which to operate.
 *
 * Get the number of children of an #AccessibleSelection implementor which are
 *        currently selected.
 *
 * Returns: a #long indicating the number of #Accessible children
 *        of the #AccessibleSelection implementor which are currently selected.
 *
 **/
long
AccessibleSelection_getNSelectedChildren (AccessibleSelection *obj)
{
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval =
    Accessibility_Selection__get_nSelectedChildren (CSPI_OBJREF (obj),
						    cspi_ev ());

  cspi_return_val_if_ev ("getNSelectedChildren", -1);

  return retval;
}

/**
 * AccessibleSelection_getSelectedChild:
 * @obj: a pointer to the #AccessibleSelection on which to operate.
 * @selectedChildIndex: a #long indicating which of the selected
 *      children is specified.
 *
 * Get the i-th selected #Accessible child of an #AccessibleSelection.
 *      Note that @childIndex refers to the index in the list of 'selected'
 *      children and generally differs from that used in
 *      #Accessible_getChildAtIndex() or returned by
 *      #Accessible_getIndexInParent(). @selectedChildIndex must lie between 0
 *      and #AccessibleSelection_getNSelectedChildren()-1, inclusive.
 *
 * Returns: a pointer to a selected #Accessible child object,
 *          specified by @childIndex.
 *
 **/
Accessible *
AccessibleSelection_getSelectedChild (AccessibleSelection *obj,
                                      long int selectedChildIndex)
{
  Accessibility_Accessible child;

  cspi_return_val_if_fail (obj != NULL, NULL);
  
  child = Accessibility_Selection_getSelectedChild (
    CSPI_OBJREF (obj),
    selectedChildIndex, cspi_ev ());

  return  cspi_object_add (child);
}

/**
 * AccessibleSelection_selectChild:
 * @obj: a pointer to the #AccessibleSelection on which to operate.
 * @childIndex: a #long indicating which child of the #Accessible
 *              is to be selected.
 *
 * Add a child to the selected children list of an #AccessibleSelection.
 *         For #AccessibleSelection implementors that only allow
 *         single selections, this may replace the (single) current
 *         selection.
 *
 * Returns: #TRUE if the child was successfully selected, #FALSE otherwise.
 **/
SPIBoolean
AccessibleSelection_selectChild (AccessibleSelection *obj,
                                 long int childIndex)
{
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval =
    Accessibility_Selection_selectChild (CSPI_OBJREF (obj),
					 childIndex, cspi_ev ());

  cspi_return_val_if_ev ("selectChild", FALSE);

  return retval;
}

/**
 * AccessibleSelection_deselectSelectedChild:
 * @obj: a pointer to the #AccessibleSelection on which to operate.
 * @selectedChildIndex: a #long indicating which of the selected children
 *              of the #Accessible is to be selected.
 *
 * Remove a child to the selected children list of an #AccessibleSelection.
 *          Note that @childIndex is the index in the selected-children list,
 *          not the index in the parent container.  @selectedChildIndex in this
 *          method, and @childIndex in #AccessibleSelection_selectChild
 *          are asymmettric.
 *
 * Returns: #TRUE if the child was successfully deselected, #FALSE otherwise.
 **/
SPIBoolean
AccessibleSelection_deselectSelectedChild (AccessibleSelection *obj,
                                           long int selectedChildIndex)
{
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval = Accessibility_Selection_deselectSelectedChild (
    CSPI_OBJREF (obj), selectedChildIndex, cspi_ev ());

  cspi_return_val_if_ev ("deselectSelectedChild", FALSE);

  return retval;
}

/**
 * AccessibleSelection_deselectChild:
 * @obj: a pointer to the #AccessibleSelection on which to operate.
 * @childIndex: a #long indicating which of the children
 *              of the #Accessible is to be de-selected.
 *
 * Deselect a specific child of an #AccessibleSelection.
 *          Note that @childIndex is the index of the child
 *          in the parent container.
 * 
 * See #AccessibleSelection_deselectSelectedChild
 *
 * Since AT-SPI 1.8.0
 * 
 * Returns: #TRUE if the child was successfully deselected, #FALSE otherwise.
 **/
SPIBoolean
AccessibleSelection_deselectChild (AccessibleSelection *obj,
				   long int childIndex)
{
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval = Accessibility_Selection_deselectChild (
    CSPI_OBJREF (obj), childIndex, cspi_ev ());

  cspi_return_val_if_ev ("deselectChild", FALSE);

  return retval;
}

/**
 * AccessibleSelection_isChildSelected:
 * @obj: a pointer to the #AccessibleSelection implementor on which to operate.
 * @childIndex: an index into the #AccessibleSelection's list of children.
 *
 * Determine whether a particular child of an #AccessibleSelection implementor
 *        is currently selected.  Note that @childIndex is the index into the
 *        standard #Accessible container's list of children.
 *
 * Returns: #TRUE if the specified child is currently selected,
 *          #FALSE otherwise.
 **/
SPIBoolean
AccessibleSelection_isChildSelected (AccessibleSelection *obj,
                                     long int childIndex)
{
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval = Accessibility_Selection_isChildSelected (
    CSPI_OBJREF (obj),
    childIndex, cspi_ev ());

  cspi_return_val_if_ev ("isChildSelected", FALSE);

  return retval;
}

/**
 * AccessibleSelection_selectAll:
 * @obj: a pointer to the #AccessibleSelection implementor on which to operate.
 *
 * Attempt to select all of the children of an #AccessibleSelection implementor.
 * Not all #AccessibleSelection implementors support this operation.
 *
 * Returns: #TRUE if successful, #FALSE otherwise.
 *
 **/
SPIBoolean
AccessibleSelection_selectAll (AccessibleSelection *obj)
{
  SPIBoolean retval;
  
  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval = Accessibility_Selection_selectAll (CSPI_OBJREF (obj), cspi_ev ());

  cspi_return_val_if_ev ("selectAll", FALSE);

  return retval;
}

/**
 * AccessibleSelection_clearSelection:
 * @obj: a pointer to the #AccessibleSelection implementor on which to operate.
 *
 * Clear the current selection, removing all selected children from the
 *       specified #AccessibleSelection implementor's selection list.
 *
 * Returns: #TRUE if successful, #FALSE otherwise.
 *
 **/
SPIBoolean
AccessibleSelection_clearSelection (AccessibleSelection *obj)
{
  SPIBoolean retval;
  
  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval = Accessibility_Selection_clearSelection (CSPI_OBJREF (obj), cspi_ev ());
  cspi_return_val_if_ev ("clearSelection", FALSE);

  return retval;
}


