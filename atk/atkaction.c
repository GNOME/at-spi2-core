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

#include "atkaction.h"

GType
atk_action_get_type ()
{
  static GType type = 0;

  if (!type) {
    GTypeInfo tinfo =
    {
      sizeof (AtkActionIface),
      NULL,
      NULL,

    };

    type = g_type_register_static (G_TYPE_INTERFACE, "AtkAction", &tinfo, 0);
  }

  return type;
}

/**
 * atk_action_do_action:
 * @action: a GObject instance that implements AtkActionIface
 * @i: a %gint indicating the action to be performed 
 *
 * This function would be called by an application with
 * the argument being a AtkObject object cast to (AtkAction).
 * The function will just check that * the corresponding
 * function pointer is not NULL and will call it.
 * The "real" implementation of the function for accessible will be
 * provided in a support library
 *
 **/
void
atk_action_do_action (AtkAction *obj,
                      gint      i)
{
  AtkActionIface *iface;

  g_return_if_fail (obj != NULL);
  g_return_if_fail (ATK_IS_ACTION (obj));

  iface = ATK_ACTION_GET_IFACE (obj);

  if (iface->do_action)
    (iface->do_action) (obj, i);
}

/**
 * atk_action_get_n_actions:
 * @action: a GObject instance that implements AtkActionIface
 *
 * 
 * This function would be called by an application with
 * the argument being a AtkObject object cast to (AtkAction).
 * The function will just check that * the corresponding
 * function pointer is not NULL and will call it.
 * The "real" implementation of the function for accessible will be
 * provided in a support library
 *
 * Returns a %gint representing the number of actions , or 0
 * if value does not implement this interface.
 **/
gint
atk_action_get_n_actions  (AtkAction *obj)
{
  AtkActionIface *iface;

  g_return_val_if_fail (obj != NULL, 0);
  g_return_val_if_fail (ATK_IS_ACTION (obj), 0);

  iface = ATK_ACTION_GET_IFACE (obj);

  if (iface->get_n_actions)
    return (iface->get_n_actions) (obj);
  else
    return 0;
}

/**
 * atk_action_get_description:
 * @action: a GObject instance that implements AtkActionIface
 * @i: a %gint indicating the action
 *
 * 
 * This function would be called by an application with
 * the argument being a AtkObject object cast to (AtkAction).
 * The function will just check that * the corresponding
 * function pointer is not NULL and will call it.
 * The "real" implementation of the function for accessible will be
 * provided in a support library
 *
 * Returns a #gchar representing the description, or %NULL
 * if value does not implement this interface.
 **/
G_CONST_RETURN gchar*
atk_action_get_description (AtkAction *obj,
                            gint      i)
{
  AtkActionIface *iface;

  g_return_val_if_fail (obj != NULL, NULL);
  g_return_val_if_fail (ATK_IS_ACTION (obj), NULL);

  iface = ATK_ACTION_GET_IFACE (obj);

  if (iface->get_description)
    return (iface->get_description) (obj, i);
  else
    return NULL;
}

/**
 * atk_action_get_keybinding:
 * @action: a GObject instance that implements AtkActionIface
 * @i: a %gint indicating the action
 *
 * 
 * This function would be called by an application with
 * the argument being a AtkObject object cast to (AtkAction).
 * The function will just check that * the corresponding
 * function pointer is not NULL and will call it.
 * The "real" implementation of the function for accessible will be
 * provided in a support library
 *
 * Returns a #gchar representing the keybinding, or %NULL
 * if there is no keybinding for this action.
 *
 **/
G_CONST_RETURN gchar*
atk_action_get_keybinding (AtkAction *obj,
                           gint      i)
{
  AtkActionIface *iface;

  g_return_val_if_fail (obj != NULL, NULL);
  g_return_val_if_fail (ATK_IS_ACTION (obj), NULL);

  iface = ATK_ACTION_GET_IFACE (obj);

  if (iface->get_keybinding)
    return (iface->get_keybinding) (obj, i);
  else
    return NULL;
}
