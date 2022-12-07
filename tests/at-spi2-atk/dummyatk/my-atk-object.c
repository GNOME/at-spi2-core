/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; https://wiki.gnome.org/Accessibility)
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <atk/atk.h>
#include <stdio.h>

#include "my-atk-object.h"

GType my_atk_object_get_type (void);

G_DEFINE_TYPE (MyAtkObject,
               my_atk_object,
               ATK_TYPE_OBJECT);

void
my_atk_object_add_child (MyAtkObject *parent,
                         MyAtkObject *child)
{
  g_ptr_array_add (parent->children, child);
  g_object_ref_sink (child);

  atk_object_set_parent (ATK_OBJECT (child), ATK_OBJECT (parent));

  g_signal_emit_by_name (parent, "children-changed::add",
                         parent->children->len - 1,
                         child);
}

void
my_atk_object_remove_child (MyAtkObject *parent,
                            MyAtkObject *child)
{
  gint i;
  for (i = parent->children->len - 1; i >= 0; i--)
    {
      if (g_ptr_array_index (parent->children, i) == child)
        break;
    }
  g_return_if_fail (i < 0);
  g_ptr_array_remove_index (parent->children, i);
  g_signal_emit_by_name (parent, "children-changed::remove", i, child);
}

static void
my_atk_object_set_parent (AtkObject *accessible, AtkObject *parent)
{
  MyAtkObject *self = MY_ATK_OBJECT (accessible);
  AtkObject *parent_old = atk_object_get_parent (accessible);

  if (parent_old == parent)
    return;

  AtkObjectClass *klass = ATK_OBJECT_CLASS (my_atk_object_parent_class);
  klass->set_parent (accessible, parent);

  if (parent_old != NULL)
    my_atk_object_remove_child (MY_ATK_OBJECT (parent_old), self);
}

static gint
my_atk_object_get_n_children (AtkObject *accessible)
{
  MyAtkObject *self = MY_ATK_OBJECT (accessible);
  return self->children->len;
}

static AtkObject *
my_atk_object_ref_child (AtkObject *accessible, gint i)
{
  MyAtkObject *self = MY_ATK_OBJECT (accessible);

  g_return_val_if_fail (i >= 0 || i <= self->children->len, NULL);

  AtkObject *child = ATK_OBJECT (g_ptr_array_index (self->children, i));

  return (child == NULL) ? NULL : g_object_ref (child);
}

static gint
my_atk_object_get_index_in_parent (AtkObject *accessible)
{
  AtkObject *parent = atk_object_get_parent (accessible);
  if (parent == NULL)
    return -1; /*root object so no parent*/

  MyAtkObject *parent_my = MY_ATK_OBJECT (parent);

  int i = parent_my->children->len;
  for (; i >= 0; i--)
    {
      if (g_ptr_array_index (parent_my->children, i) == accessible)
        break;
    }

  g_return_val_if_fail (i >= 0, -1);

  return i;
}

static AtkRelationSet *
my_atk_object_ref_relation_set (AtkObject *accessible)
{
  MyAtkObject *obj = MY_ATK_OBJECT (accessible);
  if (obj->relation_set == NULL)
    obj->relation_set = atk_relation_set_new ();
  return g_object_ref (ATK_RELATION_SET (obj->relation_set));
}

static AtkStateSet *
my_atk_object_ref_state_set (AtkObject *accessible)
{
  MyAtkObject *obj = MY_ATK_OBJECT (accessible);
  if (obj->state_set == NULL)
    obj->state_set = atk_state_set_new ();
  return g_object_ref (ATK_STATE_SET (obj->state_set));
}

static AtkAttributeSet *
my_atk_object_get_attributes (AtkObject *accessible)
{
  AtkAttributeSet *attributes = NULL;
  AtkAttribute *attr;

  attr = g_new0 (AtkAttribute, 1);
  attr->name = g_strdup ("atspi1");
  attr->value = g_strdup ("test1");
  attributes = g_slist_prepend (attributes, attr);

  attr = g_new0 (AtkAttribute, 1);
  attr->name = g_strdup ("atspi2");
  attr->value = g_strdup ("test2");
  attributes = g_slist_prepend (attributes, attr);

  return attributes;
}

static void
my_atk_object_init (MyAtkObject *self)
{
  self->children = g_ptr_array_new_full (10, g_object_unref);
}

static void
my_atk_object_class_init (MyAtkObjectClass *my_class)
{
  AtkObjectClass *object_class = ATK_OBJECT_CLASS (my_class);

  object_class->set_parent = my_atk_object_set_parent;
  object_class->get_n_children = my_atk_object_get_n_children;
  object_class->ref_child = my_atk_object_ref_child;
  object_class->get_index_in_parent = my_atk_object_get_index_in_parent;
  object_class->ref_state_set = my_atk_object_ref_state_set;
  object_class->get_attributes = my_atk_object_get_attributes;
  object_class->ref_relation_set = my_atk_object_ref_relation_set;
}
