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
static AtkFocusTrackerInit  focus_tracker_init = NULL;

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
  
void
atk_focus_tracker_init (AtkFocusTrackerInit    init)
{
  if (focus_tracker_init == NULL)
    focus_tracker_init = init;
}

guint
atk_add_focus_tracker (AtkFocusTracker   focus_tracker)
{
  g_return_val_if_fail ((focus_tracker != NULL), 0);

  if (!init_done)
  {
    if (focus_tracker_init != NULL)
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
