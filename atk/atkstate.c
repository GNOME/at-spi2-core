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

#include "atkstate.h"

#include <string.h>

static guint type = ATK_STATE_LAST_DEFINED;

#define NUM_POSSIBLE_STATES               (sizeof(AtkState)*8)

static gchar* state_names[NUM_POSSIBLE_STATES] = {
 "invalid",
 "active",
 "armed",
 "busy",
 "checked",
 "defunct",
 "editable",
 "enabled",
 "expandable",
 "expanded",
 "focusable",
 "focused",
 "horizontal",
 "iconified",
 "modal",
 "multi-line",
 "multiselectable",
 "opaque",
 "pressed",
 "resizeable",
 "selectable",
 "selected",
 "sensitive",
 "showing",
 "single-line",
 "stale",
 "transient",
 "vertical",
 "visible"
};

/**
 * atk_state_type_register:
 * @name: a character string describing the new state.
 *
 * Register a new object state.
 *
 * Returns: an #AtkState value for the new state.
 **/
AtkStateType
atk_state_type_register (const gchar *name)
{

  if (type < NUM_POSSIBLE_STATES -1)
  {
    state_names[++type] = g_strdup (name); 
    return (type);
  }
  return ATK_STATE_INVALID; /* caller needs to check */
}

/**
 * atk_state_type_get_name:
 * @type: The #AtkStateType whose name is required
 *
 * Gets the description string describing the #AtkStateType @type.
 *
 * Returns: the string describing the state
 */
G_CONST_RETURN gchar*
atk_state_type_get_name (AtkStateType state)
{
  gint n;

  if (state < type)
    {
      n = state; 
      return state_names[n];
    }

  return NULL;
}

/**
 * atk_state_type_for_name:
 * @name: a character string state name
 *
 * Gets the #AtkStateType corresponding to the description string @name.
 *
 * Returns: an #AtkStateType corresponding to @name 
 */
AtkStateType
atk_state_type_for_name (const gchar *name)
{
  gint i;

  g_return_val_if_fail (name != NULL, 0);
  g_return_val_if_fail (strlen (name) > 0, 0);

  for (i = 0; i < type; i++)
  {
    if (state_names[i] == NULL)
      continue; 
    if (!strcmp(name, state_names[i])) 
      return i;
  }
  return 0;
}
