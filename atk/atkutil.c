/* ATK -  Accessibility Toolkit
 * Copyright 2001 Sun Microsystems Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "atkutil.h"

/*
 * This file supports the addition and removal of multiple focus handlers
 * as long as they are all called in the same thread.
 */
static AtkFocusTrackerInit  focus_tracker_init = (AtkFocusTrackerInit) NULL;

static gboolean init_done = FALSE;

/*
 * Array of FocusTracker structs
 */
static GArray *trackers = NULL;
static guint  index = 0;

struct _FocusTracker {
  guint index;
  AtkFocusTracker func;
};
typedef struct _FocusTracker FocusTracker;
  
/**
 * atk_focus_tracker_init:
 * @add_function: Function to be called for focus tracker initialization
 *
 * Specifies the function to be called for focus tracker initialization.
 * This function should be called by an implementation of the
 * ATK interface if any specific work needs to be done to enable
 * focus tracking.
 **/
void
atk_focus_tracker_init (AtkFocusTrackerInit    init)
{
  if (!focus_tracker_init)
    focus_tracker_init = init;
}

/**
 * atk_add_focus_tracker:
 * @focus_tracker: Function to be added to the list of functions to be called
 * when an object receives focus. 
 *
 * Adds the specified function to the list of functions to be called
 * when an object receives focus.
 *
 * Returns: added focus tracker id, or 0 on failure.
 **/
guint
atk_add_focus_tracker (AtkFocusTracker   focus_tracker)
{
  g_return_val_if_fail (focus_tracker, 0);

  if (!init_done)
  {
    if (focus_tracker_init)
    {
      focus_tracker_init ();
    }
    trackers = g_array_sized_new (FALSE, TRUE, sizeof (FocusTracker), 0);
    init_done = TRUE;
  }
  if (init_done)
  {
    FocusTracker item;

    item.index = ++index;
    item.func = focus_tracker;
    trackers = g_array_append_val (trackers, item); 
    return index;
  }
  else
  {
    return 0;
  }
}

/**
 * atk_remove_focus_tracker:
 * @tracker_id: the id of the focus tracker to remove
 *
 * Removes the specified focus tracker from the list of functions
 * to be called when any object receives focus
 *
 **/
void
atk_remove_focus_tracker (guint            tracker_id)
{
  FocusTracker *item;
  guint i;

  if (trackers == NULL)
    return;

  if (tracker_id == 0)
    return;

  for (i = 0; i < trackers->len; i++)
  {
    item = &g_array_index (trackers, FocusTracker, i);
    if (item->index == tracker_id)
    {
      trackers = g_array_remove_index (trackers, i);
      break;
    }
  }
}

/**
 * atk_focus_tracker_notify:
 * @object: an #AtkObject
 *
 * Cause the focus tracker functions which have been specified to be
 * executed for the object.
 *
 **/
void
atk_focus_tracker_notify (AtkObject       *object)
{
  FocusTracker *item;
  guint i;

  if (trackers == NULL)
    return;

  for (i = 0; i < trackers->len; i++)
  {
    item = &g_array_index (trackers, FocusTracker, i);
    g_return_if_fail (item != NULL);
    item->func (object);
  }
}
