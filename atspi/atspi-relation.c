/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 * Copyright 2010, 2011 Novell, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "atspi-private.h"

/**
 * AtspiRelation:
 *
 * An interface via which non-hierarchical relationships
 * are indicated.
 *
 * An interface via which non-hierarchical relationships
 * are indicated. An instance of this interface represents
 * a "one-to-many" correspondence.
 */

/**
 * atspi_relation_get_relation_type:
 * @obj: a pointer to the #AtspiRelation object to query.
 *
 * Gets the type of relationship represented by an #AtspiRelation.
 *
 * Returns: an #AtspiRelationType indicating the type of relation
 *         encapsulated in this #AtspiRelation object.
 *
 **/
AtspiRelationType
atspi_relation_get_relation_type (AtspiRelation *obj)
{
  return obj->relation_type;
}

/**
 * atspi_relation_get_n_targets:
 * @obj: a pointer to the #AtspiRelation object to query.
 *
 * Gets the number of objects which this relationship has as its
 *       target objects (the subject is the #AtspiAccessible from which this
 *       #AtspiRelation originated).
 *
 * Returns: a #gint indicating how many target objects which the
 *       originating #AtspiAccessible object has the #AtspiRelation
 *       relationship with.
 **/
gint
atspi_relation_get_n_targets (AtspiRelation *obj)
{
  return obj->targets->len;
}

/**
 * atspi_relation_get_target:
 * @obj: a pointer to the #AtspiRelation object to query.
 * @i: a (zero-index) #gint indicating which (of possibly several) target is requested.
 *
 * Gets the @i-th target of a specified #AtspiRelation relationship.
 *
 * Returns: (transfer full): an #AtspiAccessible which is the @i-th object
 *          with which the originating #AtspiAccessible has relationship
 *          specified in the #AtspiRelation object.
 *
 **/
AtspiAccessible *
atspi_relation_get_target (AtspiRelation *obj, gint i)
{
  g_return_val_if_fail (obj, NULL);

  g_return_val_if_fail (i >= 0 && i < obj->targets->len, NULL);
  return g_object_ref (g_array_index (obj->targets, AtspiAccessible *, i));
}

AtspiRelation *
_atspi_relation_new_from_iter (DBusMessageIter *iter)
{
  DBusMessageIter iter_struct, iter_array;
  dbus_uint32_t d_type;
  AtspiRelation *relation = g_object_new (ATSPI_TYPE_RELATION, NULL);

  if (!relation)
    return NULL;

  dbus_message_iter_recurse (iter, &iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &d_type);
  relation->relation_type = d_type;
  dbus_message_iter_next (&iter_struct);

  relation->targets = g_array_new (TRUE, TRUE, sizeof (AtspiAccessible *));
  dbus_message_iter_recurse (&iter_struct, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
    {
      AtspiAccessible *accessible;
      accessible = _atspi_dbus_consume_accessible (&iter_array);
      relation->targets = g_array_append_val (relation->targets, accessible);
      /* Iter was moved already, so no need to call dbus_message_iter_next */
    }
  return relation;
}

G_DEFINE_TYPE (AtspiRelation, atspi_relation, G_TYPE_OBJECT)

static void
atspi_relation_init (AtspiRelation *relation)
{
}

static void
atspi_relation_finalize (GObject *object)
{
  AtspiRelation *relation = ATSPI_RELATION (object);
  gint i;

  for (i = 0; i < relation->targets->len; i++)
    g_object_unref (g_array_index (relation->targets, AtspiAccessible *, i));
  g_array_free (relation->targets, TRUE);

  G_OBJECT_CLASS (atspi_relation_parent_class)->finalize (object);
}

static void
atspi_relation_class_init (AtspiRelationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = atspi_relation_finalize;
}
