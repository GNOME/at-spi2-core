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
        sizeof (AtkObjectClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) atk_relation_class_init,
        (GClassFinalizeFunc) NULL,
        NULL,
        sizeof (AtkObject),
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

AtkRelationType
atk_relation_type_register (const gchar *name)
{
  /* TODO: associate name with new type */
  static guint type = ATK_RELATION_LAST_DEFINED;
  return (++type);
}

AtkRelation*
atk_relation_new (AtkObject       **targets,
                  gint            n_targets,
                  AtkRelationType relationship)
{
  AtkRelation *relation;
  int         i;
  GArray      *array;

  g_return_val_if_fail (targets != NULL, NULL);

  relation = g_object_new (ATK_TYPE_RELATION, NULL);
  array = g_array_sized_new (FALSE, FALSE, sizeof (AtkObject *), n_targets);
  for (i = 0; i < n_targets; i++)
  {
    g_array_insert_vals (array, i, &targets[i], sizeof (AtkObject *));
  }
  
  relation->target = array;
  relation->relationship = relationship;

  return relation;
}

AtkRelationType
atk_relation_get_relation_type (AtkRelation *relation)
{
  g_return_val_if_fail (relation != NULL, 0);
  g_return_val_if_fail (ATK_IS_RELATION (relation), 0);
  
  return relation->relationship;
}

GArray*
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
    g_array_free (relation->target, TRUE);
  } 
}
