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


#include "atkcomponent.h"

static AtkObject* atk_component_real_get_accessible_at_point (AtkComponent *component,
                                                              gint         x,
                                                              gint         y);

GType
atk_component_get_type ()
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo =
    {
      sizeof (AtkComponentIface),
      NULL,
      NULL,

    };

    type = g_type_register_static (G_TYPE_INTERFACE, "AtkComponent", &tinfo, 0);
  }

  return type;
}

/**
 *
 **/
guint
atk_component_add_focus_handler (AtkComponent    *component,
                                 AtkFocusHandler handler)
{
  AtkComponentIface *iface = NULL;
  g_return_val_if_fail (component != NULL, 0);
  g_return_val_if_fail (ATK_IS_COMPONENT (component), 0);

  iface = ATK_COMPONENT_GET_IFACE (component);

  if (iface->add_focus_handler)
    return (iface->add_focus_handler) (component, handler);
  else
    return 0;
}

/**
 *
 **/
void
atk_component_remove_focus_handler (AtkComponent    *component,
                                    guint           handler_id)
{
  AtkComponentIface *iface = NULL;
  g_return_if_fail (component != NULL);
  g_return_if_fail (ATK_IS_COMPONENT (component));

  iface = ATK_COMPONENT_GET_IFACE (component);

  if (iface->remove_focus_handler)
    (iface->remove_focus_handler) (component, handler_id);
}

gboolean
atk_component_contains (AtkComponent    *component,
                        gint            x,
                        gint            y)
{
  AtkComponentIface *iface = NULL;
  g_return_val_if_fail (component != NULL, FALSE);
  g_return_val_if_fail (ATK_IS_COMPONENT (component), FALSE);

  iface = ATK_COMPONENT_GET_IFACE (component);

  if (iface->contains)
    return (iface->contains) (component, x, y);
  else
    return FALSE;
}

AtkObject*
atk_component_get_accessible_at_point (AtkComponent    *component,
                                       gint            x,
                                       gint            y)
{
  AtkComponentIface *iface = NULL;
  g_return_val_if_fail (component != NULL, NULL);
  g_return_val_if_fail (ATK_IS_COMPONENT (component), NULL);

  iface = ATK_COMPONENT_GET_IFACE (component);

  if (iface->get_accessible_at_point)
    return (iface->get_accessible_at_point) (component, x, y);
  else
  {
    /*
     * if this method is not overridden use the default implementation.
     */
    return atk_component_real_get_accessible_at_point (component, x, y);
  }
}

void
atk_component_get_extents    (AtkComponent    *component,
                              gint            *x,
                              gint            *y,
                              gint            *width,
                              gint            *height)
{
  AtkComponentIface *iface = NULL;
  g_return_if_fail (component != NULL);
  g_return_if_fail (ATK_IS_COMPONENT (component));

  iface = ATK_COMPONENT_GET_IFACE (component);

  if (iface->get_extents)
    (iface->get_extents) (component, x, y, width, height);
}

void
atk_component_get_position   (AtkComponent    *component,
                              gint            *x,
                              gint            *y)
{
  AtkComponentIface *iface = NULL;
  g_return_if_fail (component != NULL);
  g_return_if_fail (ATK_IS_COMPONENT (component));

  iface = ATK_COMPONENT_GET_IFACE (component);

  if (iface->get_position)
    (iface->get_position) (component, x, y);
}

void
atk_component_get_position_on_screen (AtkComponent    *component,
                                      gint            *x,
                                      gint            *y)
{
  AtkComponentIface *iface = NULL;
  g_return_if_fail (component != NULL);
  g_return_if_fail (ATK_IS_COMPONENT (component));

  iface = ATK_COMPONENT_GET_IFACE (component);

  if (iface->get_position_on_screen)
    return (iface->get_position_on_screen) (component, x, y);
}

void
atk_component_get_size       (AtkComponent    *component,
                              gint            *x,
                              gint            *y)
{
  AtkComponentIface *iface = NULL;
  g_return_if_fail (component != NULL);
  g_return_if_fail (ATK_IS_COMPONENT (component));

  iface = ATK_COMPONENT_GET_IFACE (component);

  if (iface->get_size)
    (iface->get_size) (component, x, y);
}

void
atk_component_grab_focus (AtkComponent    *component)
{
  AtkComponentIface *iface = NULL;
  g_return_if_fail (component != NULL);
  g_return_if_fail (ATK_IS_COMPONENT (component));

  iface = ATK_COMPONENT_GET_IFACE (component);

  if (iface->grab_focus)
    (iface->grab_focus) (component);
}

void
atk_component_set_extents   (AtkComponent    *component,
                             gint            x,
                             gint            y,
                             gint            width,
                             gint            height)
{
  AtkComponentIface *iface = NULL;
  g_return_if_fail (component != NULL);
  g_return_if_fail (ATK_IS_COMPONENT (component));

  iface = ATK_COMPONENT_GET_IFACE (component);

  if (iface->set_extents)
    (iface->set_extents) (component, x, y, width, height);
}

void
atk_component_set_position   (AtkComponent    *component,
                              gint            x,
                              gint            y)
{
  AtkComponentIface *iface = NULL;
  g_return_if_fail (component != NULL);
  g_return_if_fail (ATK_IS_COMPONENT (component));

  iface = ATK_COMPONENT_GET_IFACE (component);

  if (iface->set_position)
    (iface->set_position) (component, x, y);
}

void
atk_component_set_size       (AtkComponent    *component,
                              gint            x,
                              gint            y)
{
  AtkComponentIface *iface = NULL;
  g_return_if_fail (component != NULL);
  g_return_if_fail (ATK_IS_COMPONENT (component));

  iface = ATK_COMPONENT_GET_IFACE (component);

  if (iface->set_size)
    (iface->set_size) (component, x, y);
}

static AtkObject* 
atk_component_real_get_accessible_at_point (AtkComponent *component,
                                            gint         x,
                                            gint         y)
{
  gint count, i;

  count = atk_object_get_n_accessible_children (ATK_OBJECT (component));

  g_return_val_if_fail (count != 0, NULL);

  for (i = 0; i < count; i++)
  {
    AtkObject *obj;

    obj = atk_object_ref_accessible_child (ATK_OBJECT (component), i);

    if (obj != NULL)
    {
      if (atk_component_contains (ATK_COMPONENT (obj), x, y))
      {
        g_object_unref (obj);
        return obj;
      }
      else
      {
        g_object_unref (obj);
      }
    }
  }
  return NULL;
}
