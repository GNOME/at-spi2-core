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

#include "atkselection.h"

GType
atk_selection_get_type ()
{
  static GType type = 0;

  if (!type) {
    GTypeInfo tinfo =
    {
      sizeof (AtkSelectionIface),
      NULL,
      NULL,

    };

    type = g_type_register_static (G_TYPE_INTERFACE, "AtkSelection", &tinfo, 0);
  }

  return type;
}

/**
 * atk_selection_add_selection:
 * @value: a GObject instance that implements AtkSelectionIface
 * @return: void
 **/
void
atk_selection_add_selection (AtkSelection *obj,
                             gint         i)
{
  AtkSelectionIface *iface;

  g_return_if_fail (obj != NULL);
  g_return_if_fail (ATK_IS_SELECTION (obj));

  iface = ATK_SELECTION_GET_IFACE (obj);

  if (iface->add_selection)
    (iface->add_selection) (obj, i);
}

/**
 * atk_selection_clear_selection:
 * @value: a GObject instance that implements AtkSelectionIface
 * @return: void
 **/
void
atk_selection_clear_selection (AtkSelection *obj)
{
  AtkSelectionIface *iface;

  g_return_if_fail (obj != NULL);
  g_return_if_fail (ATK_IS_SELECTION (obj));

  iface = ATK_SELECTION_GET_IFACE (obj);

  if (iface->clear_selection)
    (iface->clear_selection) (obj);
}

/**
 * atk_selection_ref_selection:
 * @value: a GObject instance that implements AtkSelectionIface
 * @return: a AtkObject* representing the selected accessible , or NULL
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_value() convenience method.
 **/
AtkObject*
atk_selection_ref_selection (AtkSelection *obj,
                             gint         i)
{
  AtkSelectionIface *iface;

  g_return_val_if_fail (obj != NULL, NULL);
  g_return_val_if_fail (ATK_IS_SELECTION (obj), NULL);

  iface = ATK_SELECTION_GET_IFACE (obj);

  if (iface->ref_selection)
    return (iface->ref_selection) (obj, i);
  else
    return NULL;
}

/**
 * atk_selection_get_selection_count:
 * @value: a GObject instance that implements AtkSelectionIface
 * @return: a gint representing the number of items selected, or 0
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_value() convenience method.
 **/
gint
atk_selection_get_selection_count (AtkSelection *obj)
{
  AtkSelectionIface *iface;

  g_return_val_if_fail (obj != NULL, 0);
  g_return_val_if_fail (ATK_IS_SELECTION (obj), 0);

  iface = ATK_SELECTION_GET_IFACE (obj);

  if (iface->get_selection_count)
    return (iface->get_selection_count) (obj);
  else
    return 0;
}

/**
 * atk_selection_is_child_selected:
 * @value: a GObject instance that implements AtkSelectionIface
 * @return: a gboolean representing the specified child is selected, or 0
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_value() convenience method.
 **/
gboolean
atk_selection_is_child_selected (AtkSelection *obj,
                                 gint         i)
{
  AtkSelectionIface *iface;

  g_return_val_if_fail (obj != NULL, FALSE);
  g_return_val_if_fail (ATK_IS_SELECTION (obj), FALSE);

  iface = ATK_SELECTION_GET_IFACE (obj);

  if (iface->is_child_selected)
    return (iface->is_child_selected) (obj, i);
  else
    return FALSE;
}

/**
 * atk_selection_remove_selection:
 * @value: a GObject instance that implements AtkSelectionIface
 * @return: void
 **/
void
atk_selection_remove_selection (AtkSelection *obj,
                                gint         i)
{
  AtkSelectionIface *iface;

  g_return_if_fail (obj != NULL);
  g_return_if_fail (ATK_IS_SELECTION (obj));

  iface = ATK_SELECTION_GET_IFACE (obj);

  if (iface->remove_selection)
    (iface->remove_selection) (obj, i);
}

/**
 * atk_selection_select_all_selection:
 * @value: a GObject instance that implements AtkSelectionIface
 * @return: void
 **/
void
atk_selection_select_all_selection (AtkSelection *obj)
{
  AtkSelectionIface *iface;

  g_return_if_fail (obj != NULL);
  g_return_if_fail (ATK_IS_SELECTION (obj));

  iface = ATK_SELECTION_GET_IFACE (obj);

  if (iface->select_all_selection)
    (iface->select_all_selection) (obj);
}
