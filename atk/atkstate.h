/* ATK -  Accessibility Toolkit
 * Copyright 2001 Sun Microsystems Inc.
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

#ifndef __ATK_STATE_H__
#define __ATK_STATE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <glib-object.h>

typedef enum
{
  ATK_STATE_INVALID,
  /* Indicates a window is currently the active window */
  ATK_STATE_ACTIVE,
  /* Indicates that the object is armed */
  ATK_STATE_ARMED,
  /* Indicates the current object is busy */
  ATK_STATE_BUSY,
  /* Indicates this object is currently checked */
  ATK_STATE_CHECKED,
  /* Indicates this object is collapsed */
  ATK_STATE_COLLAPSED,
  /*
   * Indicates the user interface object corresponding to this object
   * no longer exists.
   */
  ATK_STATE_DEFUNCT,
  /* Indicates the user can change the contents of this object */
  ATK_STATE_EDITABLE,
  /* Indicates this object allows progressive disclosure of its children */
  ATK_STATE_EXPANDABLE,
  /* Indicates this object its expanded */
  ATK_STATE_EXPANDED,
  /*
   * Indicates this object can accept keyboard focus, which means all
   * events resulting from typing on the keyboard will normally be passed
   * to it when it has focus
   */
  ATK_STATE_FOCUSABLE,
  /* Indicates this object currently has the keyboard focus */
  ATK_STATE_FOCUSED,
  /* Indicates the orientation of this object is horizontal */
  ATK_STATE_HORIZONTAL,
  /* Indicates this object is minimized and is represented only by an icon */
  ATK_STATE_ICONIFIED,
  /*
   * Indicates something must be done with this object before the user can
   * interact with an object in a different window.
   */
  ATK_STATE_MODAL,
  /* Indicates this (text) object can contain multiple lines of text */
  ATK_STATE_MULTI_LINE,
  /*
   * Indicates this object allows more than one of its children to be
   * selected at the same time
   */
  ATK_STATE_MULTISELECTABLE,
  /* Indicates this object paints every pixel within its rectangular region. */
  ATK_STATE_OPAQUE,
  /* Indicates this object is currently pressed */
  ATK_STATE_PRESSED,
  /* Indicates the size of this object is not fixed */
  ATK_STATE_RESIZABLE,
  /*
   * Indicates this object is the child of an object that allows its
   * children to be selected and that this child is one of those children
   * that can be selected.
   */
  ATK_STATE_SELECTABLE,
  /*
   * Indicates this object is the child of an object that allows its
   * children to be selected and that this child is one of those children
   * that has been selected.
   */
  ATK_STATE_SELECTED,
  /* Indicates this object is sensitive */
  ATK_STATE_SENSITIVE,
  /*
   * Indicates this object, the object's parent, the object's parent's
   * parent, and so on, are all visible
   */
  ATK_STATE_SHOWING,
  /* Indicates this (text) object can contain only a single line of text */
  ATK_STATE_SINGLE_LINE,
  /* Indicates this object is transient */
  ATK_STATE_TRANSIENT,
  /* Indicates the orientation of this object is vertical */
  ATK_STATE_VERTICAL,
  /* Indicates this object is visible */
  ATK_STATE_VISIBLE,
  ATK_STATE_LAST_DEFINED
} AtkStateType;

typedef guint64      AtkState;

AtkStateType atk_state_type_register            (const gchar *name);

G_CONST_RETURN gchar* atk_state_type_get_name   (AtkStateType type);
AtkStateType          atk_state_type_for_name   (const gchar  *name);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ATK_STATE_H__ */
