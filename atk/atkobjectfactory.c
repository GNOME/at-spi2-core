/* ATK - Accessibility Toolkit
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

#include "atkobjectfactory.h"
#include "atknoopobjectfactory.h"

static void atk_object_factory_class_init   (AtkObjectFactoryClass        *klass);

static gpointer    parent_class = NULL;

GType
atk_object_factory_get_type ()
{
  static GType type = 0;

  if (!type) {
    GTypeInfo tinfo =
    {
      sizeof (AtkObjectFactoryClass),
      (GBaseInitFunc) NULL, /* base init */
      (GBaseFinalizeFunc) NULL, /* base finalize */
      (GClassInitFunc) atk_object_factory_class_init, /* class init */
      (GClassFinalizeFunc) NULL, /* class finalize */
      NULL, /* class data */
      sizeof (AtkObjectFactory), /* instance size */
      0, /* nb preallocs */
      (GInstanceInitFunc) NULL, /* instance init */
      NULL /* value table */
    };

    type = g_type_register_static (G_TYPE_OBJECT, "AtkObjectFactory", &tinfo, 0);
  }
  return type;
}

static void 
atk_object_factory_class_init (AtkObjectFactoryClass *klass)
{
  parent_class = g_type_class_ref (G_TYPE_OBJECT);

}

/**
 *atk_object_factory_create_accessible:
 *@factory: The #AtkObjectFactory associated with @obj's
 *object type
 *@obj: a #GObject 
 * 
 *Provides a #AtkObject that implements an accessability interface 
 *on behalf of @obj
 *
 *Returns: an #AtkObject that implements an accessability interface
 *on behalf of @obj
 **/
AtkObject* 
atk_object_factory_create_accessible (AtkObjectFactory *factory,
                                      GObject          *obj)
{
  AtkObjectFactoryClass *klass;
  AtkObject *accessible = NULL;

  g_return_val_if_fail ((factory != NULL), NULL);
  g_return_val_if_fail (ATK_IS_OBJECT_FACTORY (factory), NULL);
  g_return_val_if_fail (obj != NULL, NULL);
  g_return_val_if_fail (G_IS_OBJECT (obj), NULL);

  klass = ATK_OBJECT_FACTORY_GET_CLASS (factory);

  if (klass->create_accessible)
  {
      accessible = klass->create_accessible (obj);
  }
  return accessible;
} 

/**
 *atk_object_factory_invalidate:
 *@factory: an #AtkObjectFactory
 *
 *Inform @factory that it is no longer being used to create
 * accessables. When called, @factory may need to inform
 * #AtkObject's which it has created that they need to be re-instantiated.
 * Note: primarily used for runtime replacement of #AtkObjectFactory's
 * in object registries.
 **/
void 
atk_object_factory_invalidate (AtkObjectFactory *factory)
{
  AtkObjectFactoryClass *klass;

  g_return_if_fail (factory != NULL);
  g_return_if_fail (ATK_OBJECT_FACTORY (factory));

  klass = ATK_OBJECT_FACTORY_GET_CLASS (factory);
  if (klass->invalidate)
     (klass->invalidate) (factory);
}
