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

#include "atkhyperlink.h"


static void atk_hyperlink_class_init (AtkHyperlinkClass *klass);
static void atk_hyperlink_init       (AtkHyperlink      *link,
                                      AtkHyperlinkClass *klass);

static void atk_hyperlink_action_iface_init (AtkActionIface *iface);

static gpointer  parent_class = NULL;

GType
atk_hyperlink_get_type (void)
{
  static GType type = 0;

  if (!type)
    {
      static const GTypeInfo typeInfo =
      {
        sizeof (AtkHyperlinkClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) atk_hyperlink_class_init,
        (GClassFinalizeFunc) NULL,
        NULL,
        sizeof (AtkObject),
        0,
        (GInstanceInitFunc) atk_hyperlink_init,
      } ;

      static const GInterfaceInfo action_info =
      {
        (GInterfaceInitFunc) atk_hyperlink_action_iface_init,
        (GInterfaceFinalizeFunc) NULL,
        NULL
      };

      type = g_type_register_static (G_TYPE_OBJECT, "AtkHyperlink", &typeInfo, 0) ;
      g_type_add_interface_static (type, ATK_TYPE_ACTION, &action_info);
    }
  return type;
}

static void
atk_hyperlink_class_init (AtkHyperlinkClass *klass)
{
  parent_class = g_type_class_ref (G_TYPE_OBJECT);

}

static void
atk_hyperlink_init  (AtkHyperlink        *link,
                     AtkHyperlinkClass   *klass)
{
}

GObject*
atk_hyperlink_get_anchor (AtkHyperlink *link,
                          gint         i)
{
  AtkHyperlinkClass *klass;

  g_return_val_if_fail ((link != NULL), NULL);
  g_return_val_if_fail (ATK_IS_HYPERLINK (link), NULL);

  klass = ATK_HYPERLINK_GET_CLASS (link);
  g_return_val_if_fail ((klass->get_anchor != NULL), NULL);

  return (klass->get_anchor) (link, i);
}

GObject*
atk_hyperlink_get_object (AtkHyperlink *link,
                          gint         i)
{
  AtkHyperlinkClass *klass;

  g_return_val_if_fail ((link != NULL), NULL);
  g_return_val_if_fail (ATK_IS_HYPERLINK (link), NULL);

  klass = ATK_HYPERLINK_GET_CLASS (link);
  g_return_val_if_fail ((klass->get_object != NULL), NULL);

  return (klass->get_object) (link, i);
}

gint
atk_hyperlink_get_end_index (AtkHyperlink *link)
{
  AtkHyperlinkClass *klass;

  g_return_val_if_fail ((link != NULL), 0);
  g_return_val_if_fail (ATK_IS_HYPERLINK (link), 0);

  klass = ATK_HYPERLINK_GET_CLASS (link);
  g_return_val_if_fail ((klass->get_end_index != NULL), 0);

  return (klass->get_end_index) (link);
}

gint
atk_hyperlink_get_start_index (AtkHyperlink *link)
{
  AtkHyperlinkClass *klass;

  g_return_val_if_fail ((link != NULL), 0);
  g_return_val_if_fail (ATK_IS_HYPERLINK (link), 0);

  klass = ATK_HYPERLINK_GET_CLASS (link);
  g_return_val_if_fail ((klass->get_start_index != NULL), 0);

  return (klass->get_end_index) (link);
}

gboolean
atk_hyperlink_get_is_valid (AtkHyperlink *link)
{
  AtkHyperlinkClass *klass;

  g_return_val_if_fail ((link != NULL), FALSE);
  g_return_val_if_fail (ATK_IS_HYPERLINK (link), FALSE);

  klass = ATK_HYPERLINK_GET_CLASS (link);
  g_return_val_if_fail ((klass->is_valid != NULL), FALSE);

  return (klass->is_valid) (link);
}

static void atk_hyperlink_action_iface_init (AtkActionIface *iface)
{
  /*
   * We do nothing here
   *
   * When we come to derive a class from AtkHyperlink we will provide an
   * implementation of the AtkAction interface. 
   *
   * This depends on being able to override an interface in a derived class
   * which currently (March 2001) is not implemented but will be in GTK+ 2.0.
   */
}
