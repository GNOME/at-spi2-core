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

#ifndef __ATK_UTIL_H__
#define __ATK_UTIL_H__

#include <atk/atkobject.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * A focus tracker is a function which is called when an object 
 * receives focus.
 */
typedef void  (*AtkFocusTracker) (AtkObject*);
typedef void  (*AtkFocusTrackerInit) (void);


/*
 * Adds the specified function to the list of functions to be called
 * when an object receives focus.
 */
guint    atk_add_focus_tracker     (AtkFocusTracker      focus_tracker);
/*
 * Removes the specified focus tracker from the list of function
 * to be called when any object receives focus
 */
void     atk_remove_focus_tracker  (guint                tracker_id);
/*
 * Specifies the function to be called for focus tracker initialization.
 * removal. This function should be called by an implementation of the
 * ATK interface if any specific work needs to be done to enable
 * focus tracking.
 */
void     atk_focus_tracker_init    (AtkFocusTrackerInit  add_function);
/*
 * Cause the focus tracker functions which have been specified to be
 * executed for the object.
 */
void     atk_focus_tracker_notify  (AtkObject            *object);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __ATK_UTIL_H__ */
