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

#include "atk.h"

static void            atk_relation_set_class_init       (AtkRelationSetClass  *klass);
static void            atk_relation_set_finalize         (GObject              *object);

GType
atk_relation_set_get_type (void)
{
  static GType type = 0;

  if (!type)
    {
      static const GTypeInfo typeInfo =
      {
        sizeof (AtkObjectClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) atk_relation_set_class_init,
        (GClassFinalizeFunc) NULL,
        NULL,
        sizeof (AtkObject),
        0,
        (GInstanceInitFunc) NULL,
      } ;
      type = g_type_register_static (G_TYPE_OBJECT, "AtkRelatioSet", &typeInfo, 0) ;
    }
  return type;
}

static void
atk_relation_set_class_init (AtkRelationSetClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = atk_relation_set_finalize;
}

/**
 * atk_relation_set_new
 * return values: a new #AtkRelationSet 
 * 
 * Creates a new empty relation set.
 **/
AtkRelationSet*
atk_relation_set_new (void)
{
  AtkRelationSet *relation_set;

  relation_set = g_object_new (ATK_TYPE_RELATION_SET, NULL);
  return relation_set;
}

/**
 * atk_relation_set_contains
 * @set: a #AtkRelationSet
 * @relationtype: a #AtkRelationType
 * return values: %TRUE if @relationtype is the relationship type of a relation in @set.
 *
 * Determines whether the relation set contains a relation that matches the
 * specified type.
 **/
gboolean
atk_relation_set_contains (AtkRelationSet   *set,
                           AtkRelationType  relationship)
{
  GPtrArray *array_item;
  AtkRelation *item;
  gint  i;

  g_return_val_if_fail (set != NULL, FALSE);
  g_return_val_if_fail (ATK_IS_RELATION_SET (set), FALSE);

  array_item = set->relations;
  if (array_item == NULL)
    return FALSE;
  for (i = 0; i < array_item->len; i++)
  {
    item = g_ptr_array_index (array_item, i);
    if (item->relationship == relationship)
      return TRUE;
  }
  return FALSE;
}

/**
 * atk_relation_set_remove
 * @set: a #AtkRelationSet
 * @relation: a #AtkRelation
 *
 * Removes a relation from the relation set.
 *
 * This function unref's the AtkRelation so it will be deleted unless there
 * is another reference to it.
 **/
void
atk_relation_set_remove (AtkRelationSet *set,
                         AtkRelation    *relation)
{
  GPtrArray *array_item;

  g_return_if_fail (set != NULL);
  g_return_if_fail (ATK_IS_RELATION_SET (set));
  g_return_if_fail (relation != NULL);

  array_item = set->relations;
  if (array_item == NULL)
    return;
  
  if (g_ptr_array_remove (array_item, relation))
  {
    g_object_unref (relation);
  }
}

/**
 * atk_relation_set_add
 * @set: a #AtkRelationSet
 * @relation: a #AtkRelation
 *
 * Add a new relation to the current relation set if it is not already
 * present.
 *
 * This function ref's the AtkRelation so the caller of this function
 * should unref it to ensure that it will be destroyed when the AtkRelationSet
 * is destroyed.
 **/
void
atk_relation_set_add (AtkRelationSet *set,
                      AtkRelation    *relation)
{
  g_return_if_fail (set != NULL);
  g_return_if_fail (ATK_IS_RELATION_SET (set));
  g_return_if_fail (relation != NULL);

  if (set->relations == NULL)
  {
    set->relations = g_ptr_array_new ();
  }
  g_ptr_array_add (set->relations, relation);
  g_object_ref (relation);
}

/**
 * atk_relation_set_get_n_relations
 * @set: a #AtkRelationSet
 * return values: a gint representing the number of relations in the set.
 *
 * Determines the number of relations in a relation set.
 **/
gint
atk_relation_set_get_n_relations (AtkRelationSet *set)
{
  g_return_val_if_fail (set != NULL, 0);
  g_return_val_if_fail (ATK_IS_RELATION_SET (set), FALSE);

  if (set->relations == NULL)
    return 0;

  return set->relations->len;
}

/**
 * atk_relation_set_get_relation
 * @set: a #AtkRelationSet
 * @i: a gint representing a position in the set, starting from 0.
 * return values: a #AtkRelation, which is the relation at position i in the set.
 *
 * Determines the relation at the specified position in the relation set.
 **/
AtkRelation*
atk_relation_set_get_relation (AtkRelationSet *set,
                               gint           i)
{
  GPtrArray *array_item;
  AtkRelation* item;

  g_return_val_if_fail (set != NULL, NULL);
  g_return_val_if_fail (ATK_IS_RELATION_SET (set), FALSE);
  g_return_val_if_fail (i >= 0, NULL);

  array_item = set->relations;
  if (array_item == NULL)
    return NULL;
  item = g_ptr_array_index (array_item, i);
  if (item == NULL)
    return NULL;

  return item;
}

/**
 * atk_relation_set_get_relation_type
 * @set: a #AtkRelationSet
 * @relationship: a #AtkRelationType
 * return values: a #AtkRelation, which is a relation matching the specified type.
 *
 * Finds a relation that matches the specified type.
 **/
AtkRelation*
atk_relation_set_get_relation_by_type (AtkRelationSet  *set,
                                       AtkRelationType relationship)
{
  GPtrArray *array_item;
  AtkRelation *item;
  gint i;

  g_return_val_if_fail (set != NULL, NULL);
  g_return_val_if_fail (ATK_IS_RELATION_SET (set), FALSE);

  array_item = set->relations;
  if (array_item == NULL)
    return NULL;
  for (i = 0; i < array_item->len; i++)
  {
    item = g_ptr_array_index (array_item, i);
    if (item->relationship == relationship)
      return item;
  }
  return NULL;
}

static void
atk_relation_set_finalize (GObject *object)
{
  AtkRelationSet     *relation_set;
  GPtrArray             *array;
  gint               i;

  g_return_if_fail (ATK_IS_RELATION_SET (object));

  relation_set = ATK_RELATION_SET (object);
  array = relation_set->relations;

  if (array)
  {
    for (i = 0; i < array->len; i++)
    {
      g_object_unref (g_ptr_array_index (array, i));
    }
    g_ptr_array_free (array, TRUE);
  }
}
