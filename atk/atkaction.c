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
atk_action_get_type (void)
{
  static GType type = 0;

  if (!type) {
    GTypeInfo tinfo =
    {
      sizeof (AtkActionIface),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,

    };

    type = g_type_register_static (G_TYPE_INTERFACE, "AtkAction", &tinfo, 0);
  }

  return type;
}

/**
 * atk_action_do_action:
 * @action: a #GObject instance that implements AtkActionIface
 * @i: the action index corresponding to the action to be performed 
 *
 * Perform the specified action on the object.
 *
 * Returns: %TRUE if success, %FALSE otherwise
 *
 **/
gboolean
atk_action_do_action (AtkAction *obj,
                      gint      i)
{
  AtkActionIface *iface;

  g_return_val_if_fail (ATK_IS_ACTION (obj), FALSE);

  iface = ATK_ACTION_GET_IFACE (obj);

  if (iface->do_action)
    return (iface->do_action) (obj, i);
  else
    return FALSE;
}

/**
 * atk_action_get_n_actions:
 * @action: a #GObject instance that implements AtkActionIface
 * 
 * Gets the number of accessible actions available on the object.
 * If there are more than one, the first one is considered the
 * "default" action of the object.
 *
 * Returns: a the number of actions, or 0 if @action does not
 * implement this interface.
 **/
gint
atk_action_get_n_actions  (AtkAction *obj)
{
  AtkActionIface *iface;

  g_return_val_if_fail (ATK_IS_ACTION (obj), 0);

  iface = ATK_ACTION_GET_IFACE (obj);

  if (iface->get_n_actions)
    return (iface->get_n_actions) (obj);
  else
    return 0;
}

/**
 * atk_action_get_description:
 * @action: a #GObject instance that implements AtkActionIface
 * @i: the action index corresponding to the action to be performed 
 *
 * Returns a description of the specified action of the object.
 *
 * Returns a description string, or %NULL
 * if @action does not implement this interface.
 **/
G_CONST_RETURN gchar*
atk_action_get_description (AtkAction *obj,
                            gint      i)
{
  AtkActionIface *iface;

  g_return_val_if_fail (ATK_IS_ACTION (obj), NULL);

  iface = ATK_ACTION_GET_IFACE (obj);

  if (iface->get_description)
    return (iface->get_description) (obj, i);
  else
    return NULL;
}

/**
 * atk_action_get_name:
 * @action: a #GObject instance that implements AtkActionIface
 * @i: the action index corresponding to the action to be performed 
 *
 * Returns the name of the specified action of the object.
 *
 * Returns a name string, or %NULL
 * if @action does not implement this interface.
 **/
G_CONST_RETURN gchar*
atk_action_get_name (AtkAction *obj,
                     gint      i)
{
  AtkActionIface *iface;

  g_return_val_if_fail (ATK_IS_ACTION (obj), NULL);

  iface = ATK_ACTION_GET_IFACE (obj);

  if (iface->get_name)
    return (iface->get_name) (obj, i);
  else
    return NULL;
}

/**
 * atk_action_get_localized_name:
 * @action: a #GObject instance that implements AtkActionIface
 * @i: the action index corresponding to the action to be performed 
 *
 * Returns the localized name of the specified action of the object.
 *
 * Returns a name string, or %NULL
 * if @action does not implement this interface.
 **/
G_CONST_RETURN gchar*
atk_action_get_localized_name (AtkAction *obj,
                               gint      i)
{
  AtkActionIface *iface;

  g_return_val_if_fail (ATK_IS_ACTION (obj), NULL);

  iface = ATK_ACTION_GET_IFACE (obj);

  if (iface->get_localized_name)
    return (iface->get_localized_name) (obj, i);
  else
    return NULL;
}

/**
 * atk_action_get_keybinding:
 * @action: a #GObject instance that implements AtkActionIface
 * @i: the action index corresponding to the action to be performed 
 *
 * Returns a keybinding associated with this action, if one exists.
 *
 * Returns a string representing the keybinding, or %NULL
 * if there is no keybinding for this action.
 *
 **/
G_CONST_RETURN gchar*
atk_action_get_keybinding (AtkAction *obj,
                           gint      i)
{
  AtkActionIface *iface;

  g_return_val_if_fail (ATK_IS_ACTION (obj), NULL);

  iface = ATK_ACTION_GET_IFACE (obj);

  if (iface->get_keybinding)
    return (iface->get_keybinding) (obj, i);
  else
    return NULL;
}

/**
 * atk_action_set_description:
 * @action: a #GObject instance that implements AtkActionIface
 * @i: the action index corresponding to the action to be performed 
 * @desc: the description to be assigned to this action
 *
 * Sets a description of the specified action of the object.
 *
 * Returns: a gboolean representing if the description was successfully set;
 **/
gboolean
atk_action_set_description (AtkAction   *obj,
                            gint        i,
                            const gchar *desc)
{
  AtkActionIface *iface;

  g_return_val_if_fail (ATK_IS_ACTION (obj), FALSE);

  iface = ATK_ACTION_GET_IFACE (obj);

  if (iface->set_description)
    return (iface->set_description) (obj, i, desc);
  else
    return FALSE;
}
