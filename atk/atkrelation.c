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

#include <glib-object.h>
#include "atkobject.h"
#include "atkrelation.h"

static void            atk_relation_class_init       (AtkRelationClass  *klass);
static void            atk_relation_finalize         (GObject           *object);

GType
atk_relation_get_type (void)
{
  static GType type = 0;

  if (!type)
    {
      static const GTypeInfo typeInfo =
      {
        sizeof (AtkRelationClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) atk_relation_class_init,
        (GClassFinalizeFunc) NULL,
        NULL,
        sizeof (AtkRelation),
        0,
        (GInstanceInitFunc) NULL,
      } ;
      type = g_type_register_static (G_TYPE_OBJECT, "AtkRelation", &typeInfo, 0) ;
    }
  return type;
}

static void
atk_relation_class_init (AtkRelationClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = atk_relation_finalize;
}

/**
 * atk_relation_type_register:
 * @name: a name string
 *
 * Associate @name with a new #AtkRelationType
 *
 * Returns: an #AtkRelationType associated with @name
 **/
AtkRelationType
atk_relation_type_register (const gchar *name)
{
  /* TODO: associate name with new type */
  static guint type = ATK_RELATION_LAST_DEFINED;
  return (++type);
}

/**
 * atk_relation_new:
 * @targets: an array of pointers to #AtkObjects  
 * @n_targets: number of #AtkObjects pointed to by @targets
 * @relationship: an #AtkRelationType with which to create the new
 *  #AtkRelation
 *
 * Create a new relation for the specified key and the specified list
 * of targets.
 *
 * Returns: a pointer to a new #AtkRelation
 **/
AtkRelation*
atk_relation_new (AtkObject       **targets,
                  gint            n_targets,
                  AtkRelationType relationship)
{
  AtkRelation *relation;
  int         i;
  GPtrArray      *array;

  g_return_val_if_fail (targets != NULL, NULL);

  relation = g_object_new (ATK_TYPE_RELATION, NULL);
  array = g_ptr_array_sized_new (n_targets);
  for (i = 0; i < n_targets; i++)
  {
    /*
     * Add a reference to AtkObject being added to a relation
     */
    g_object_ref (targets[i]);
    g_ptr_array_add (array, targets[i]);
  }
  
  relation->target = array;
  relation->relationship = relationship;

  return relation;
}

/**
 * atk_relation_get_relation_type:
 * @relation: an #AtkRelation 
 *
 * Gets the type of @relation
 *
 * Returns: the type of @relation
 **/
AtkRelationType
atk_relation_get_relation_type (AtkRelation *relation)
{
  g_return_val_if_fail (relation != NULL, 0);
  g_return_val_if_fail (ATK_IS_RELATION (relation), 0);
  
  return relation->relationship;
}

/**
 * atk_relation_get_target:
 * @relation: an #AtkRelation
 *
 * Gets the target list of @relation
 *
 * Returns: the target list of @relation
 **/
GPtrArray*
atk_relation_get_target (AtkRelation *relation)
{
  g_return_val_if_fail (relation != NULL, FALSE);
  g_return_val_if_fail (ATK_IS_RELATION (relation), FALSE);

  return relation->target;
}

static void
atk_relation_finalize (GObject *object)
{
  AtkRelation        *relation;

  g_return_if_fail (ATK_IS_RELATION (object));

  relation = ATK_RELATION (object);

  if (relation->target)
  {
    gint i;

    for (i = 0; i < relation->target->len; i++)
    {
      /*
       * Remove a reference to AtkObject being removed from a relation
       */
      g_object_unref (g_ptr_array_index (relation->target, i));
    }
    g_ptr_array_free (relation->target, TRUE);
  } 
}
