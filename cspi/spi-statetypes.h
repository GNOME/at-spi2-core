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

#ifndef _SPI_STATETYPES_H_
#define _SPI_STATETYPES_H_

#ifdef  __cplusplus
extern "C" {
#endif

/**
 *AccessibleState:
 *@SPI_STATE_INVALID: Indicates an invalid state
 *@SPI_STATE_ACTIVE: Indicates a window is currently the active window 
 *@SPI_STATE_ARMED: Indicates that the object is armed
 *@SPI_STATE_BUSY: Indicates the current object is busy 
 *@SPI_STATE_CHECKED: Indicates this object is currently checked 
 *@SPI_STATE_COLLAPSED: Indicates this object is collapsed 
 *@SPI_STATE_DEFUNCT: Indicates the user interface object corresponding to this object no longer exists 
 *@SPI_STATE_EDITABLE: Indicates the user can change the contents of this object
 *@SPI_STATE_ENABLED: Indicates that this object is enabled 
 *@SPI_STATE_EXPANDABLE: Indicates this object allows progressive disclosure of its children 
 *@SPI_STATE_EXPANDED: Indicates this object its expanded
 *@SPI_STATE_FOCUSABLE: Indicates this object can accept keyboard focus, which means all events resulting from typing on the keyboard will normally be passed to it when it has focus
 *@SPI_STATE_FOCUSED: Indicates this object currently has the keyboard focus 
 *@SPI_STATE_HORIZONTAL: Indicates the orientation of thsi object is horizontal
 *@SPI_STATE_ICONIFIED: Indicates this object is minimized and is represented only by an icon
 *@SPI_STATE_MODAL: Indicates something must be done with this object before the user can interact with an object in a different window
 *@SPI_STATE_MULTI_LINE: Indicates this (text) object can contain multiple lines of text
 *@SPI_STATE_MULTISELECTABLE: Indicates this object allows more than one of its children to be selected at the same time
 *@SPI_STATE_OPAQUE: Indicates this object paints every pixel within its rectangular region
 *@SPI_STATE_PRESSED: Indicates this object is currently pressed
 *@SPI_STATE_RESIZABLE: Indicates the size of this object is not fixed
 *@SPI_STATE_SELECTABLE: Indicates this object is the child of an object that allows its children to be selected and that this child is one of those children that can be selected
 *@SPI_STATE_SELECTED: Indicates this object is the child of an object that allows its children to be selected and that this child is one of those children that has been selected
 *@SPI_STATE_SENSITIVE: Indicates this object is sensitive
 *@SPI_STATE_SHOWING: Indicates this object, the object's parent, the object's parent's parent, and so on, are all visible
 *@SPI_STATE_SINGLE_LINE: Indicates this (text) object can contain only a single line of text
 *@SPI_STATE_STALE: Indicates that the index associated with this object has changed since the user accessed the object
 *@SPI_STATE_TRANSIENT: Indicates this object is transient
 *@SPI_STATE_VERTICAL: Indicates the orientation of this object is vertical
 *@SPI_STATE_VISIBLE: Indicates this object is visible 
 *@SPI_STATE_MANAGES_DESCENDANTS: Indicates that "active-descendant-changed" 
 * event is sent when children become 'active' (i.e. are selected or navigated 
 * to onscreen).  Used to prevent need to enumerate all children in very large 
 * containers, like tables.
 *@SPI_STATE_INDETERMINATE: Indicates that a check box is in a state other than checked or not checked.
 *@SPI_STATE_TRUNCATED: Indicates that an object is truncated, e.g. a text value in a speradsheet cell.
 *@SPI_STATE_REQUIRED: Indicates that explicit user interaction with an object is required by the user interface, e.g. a required field in a "web-form" interface.
 *@SPI_STATE_INVALID_ENTRY: Indicates that the object has encountered an error condition due to failure of input validation. For instance, a form control may acquire this state in response to invalid or malformed user input.
 *@SPI_STATE_SUPPORTS_AUTOCOMPLETION: Indicates that the object may exhibit "typeahead" behavior in response to user keystrokes, e.g. one keystroke may result in the insertion of several characters into an entry, or result in the auto-selection of an item in a list.  This state supplants @SPI_ROLE_AUTOCOMPLETE.
 *@SPI_STATE_SELECTABLE_TEXT:Indicates that the object in question supports text selection. It should only be exposed on objects which implement the Text interface, in order to distinguish this state from @SPI_STATE_SELECTABLE, which infers that the object in question is a selectable child of an object which implements Selection. While similar, text selection and subelement selection are distinct operations.
 *@SPI_STATE_IS_DEFAULT: Indicates that the object is the "default" active component, i.e. the object which is activated by an end-user press of the "Enter" or "Return" key.  Typically a "close" or "submit" button.
 *@SPI_STATE_VISITED: Indicates that the object (typically a hyperlink) has already been 'activated', and/or its backing data has already been downloaded, rendered, or otherwise "visited".
 *@SPI_STATE_LAST_DEFINED: Not a valid state, used for finding end of enumeration
 *
 *The possibles states of an object.
 **/
typedef enum
{
  SPI_STATE_INVALID,
  SPI_STATE_ACTIVE,
  SPI_STATE_ARMED,
  SPI_STATE_BUSY,
  SPI_STATE_CHECKED,
  SPI_STATE_COLLAPSED,
  SPI_STATE_DEFUNCT,
  SPI_STATE_EDITABLE,
  SPI_STATE_ENABLED,
  SPI_STATE_EXPANDABLE,
  SPI_STATE_EXPANDED,
  SPI_STATE_FOCUSABLE,
  SPI_STATE_FOCUSED,
  SPI_STATE_HORIZONTAL,
  SPI_STATE_ICONIFIED,
  SPI_STATE_MODAL,
  SPI_STATE_MULTI_LINE,
  SPI_STATE_MULTISELECTABLE,
  SPI_STATE_OPAQUE,
  SPI_STATE_PRESSED,
  SPI_STATE_RESIZABLE,
  SPI_STATE_SELECTABLE,
  SPI_STATE_SELECTED,
  SPI_STATE_SENSITIVE,
  SPI_STATE_SHOWING,
  SPI_STATE_SINGLE_LINE,
  SPI_STATE_STALE,
  SPI_STATE_TRANSIENT,
  SPI_STATE_VERTICAL,
  SPI_STATE_VISIBLE,
  SPI_STATE_MANAGES_DESCENDANTS, 
  SPI_STATE_INDETERMINATE, 
  SPI_STATE_TRUNCATED,
  SPI_STATE_REQUIRED,
  SPI_STATE_INVALID_ENTRY,
  SPI_STATE_SUPPORTS_AUTOCOMPLETION,
  SPI_STATE_SELECTABLE_TEXT,
  SPI_STATE_IS_DEFAULT,
  SPI_STATE_VISITED,
  SPI_STATE_LAST_DEFINED
} AccessibleState;

#ifdef  __cplusplus
}
#endif

#endif
