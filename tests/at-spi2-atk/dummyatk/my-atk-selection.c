/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; https://wiki.gnome.org/Accessibility)
 *
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <string.h>

#include "my-atk-object.h"
#include "my-atk-selection.h"

static void atk_selection_interface_init (AtkSelectionIface *iface);

G_DEFINE_TYPE_WITH_CODE (MyAtkSelection,
                         my_atk_selection,
                         MY_TYPE_ATK_OBJECT,
                         G_IMPLEMENT_INTERFACE (ATK_TYPE_SELECTION,
                                                atk_selection_interface_init));

static void
my_atk_selection_init (MyAtkSelection *obj)
{
}

static gboolean
my_atk_selection_add_selection (AtkSelection *selection, gint i)
{
  MyAtkSelection *self = MY_ATK_SELECTION (selection);
  if (!self)
    return FALSE;

  AtkObject *child = atk_object_ref_accessible_child (ATK_OBJECT (selection), i);
  AtkStateSet *ss = atk_object_ref_state_set (child);
  atk_state_set_add_state (ss, ATK_STATE_SELECTED);
  return atk_state_set_contains_state (ss, ATK_STATE_SELECTED);
}

static gboolean
my_atk_selection_clear_selection (AtkSelection *selection)
{
  MyAtkSelection *self = MY_ATK_SELECTION (selection);
  if (!self)
    return FALSE;
  AtkObject *child = NULL;
  AtkStateSet *states = NULL;
  int i;
  int childs = atk_object_get_n_accessible_children (ATK_OBJECT (selection));

  for (i = 0; i < childs; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (selection), i);
      states = atk_object_ref_state_set (child);
      atk_state_set_remove_state (states, ATK_STATE_SELECTED);
    }
  return TRUE;
}

static AtkObject *
my_atk_selection_ref_selection (AtkSelection *selection, gint no)
{
  MyAtkSelection *self = MY_ATK_SELECTION (selection);
  if (!self)
    return FALSE;
  AtkObject *child = NULL;
  AtkStateSet *states = NULL;
  GArray *array = g_array_new (FALSE, FALSE, sizeof (AtkObject *));
  int i;
  int childs = atk_object_get_n_accessible_children (ATK_OBJECT (selection));

  for (i = 0; i < childs; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (selection), i);
      states = atk_object_ref_state_set (child);
      if (atk_state_set_contains_state (states, ATK_STATE_SELECTED))
        g_array_append_val (array, child);
    }

  return g_array_index (array, AtkObject *, no);
}

static gint
my_atk_selection_get_selection_count (AtkSelection *selection)
{
  MyAtkSelection *self = MY_ATK_SELECTION (selection);
  if (!self)
    return FALSE;
  AtkObject *child = NULL;
  AtkStateSet *states = NULL;
  int i, ret = 0;
  int childs = atk_object_get_n_accessible_children (ATK_OBJECT (selection));

  for (i = 0; i < childs; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (selection), i);
      states = atk_object_ref_state_set (child);
      if (atk_state_set_contains_state (states, ATK_STATE_SELECTED))
        ret++;
    }
  return ret;
}

static gboolean
my_atk_selection_is_child_selected (AtkSelection *selection, gint i)
{
  MyAtkSelection *self = MY_ATK_SELECTION (selection);
  if (!self)
    return FALSE;
  AtkObject *child = NULL;
  AtkStateSet *states = NULL;
  child = atk_object_ref_accessible_child (ATK_OBJECT (selection), i);
  states = atk_object_ref_state_set (child);
  if (atk_state_set_contains_state (states, ATK_STATE_SELECTED))
    return TRUE;
  return FALSE;
}

static gboolean
my_atk_selection_remove_selection (AtkSelection *selection, gint no)
{
  MyAtkSelection *self = MY_ATK_SELECTION (selection);
  AtkObject *child = NULL;
  AtkStateSet *states = NULL;
  GArray *array = NULL;
  AtkObject *o = NULL;
  int i;
  int childs;
  gboolean ret = FALSE;

  if (!self)
    return FALSE;
  array = g_array_new (FALSE, FALSE, sizeof (AtkObject *));
  childs = atk_object_get_n_accessible_children (ATK_OBJECT (selection));

  for (i = 0; i < childs; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (selection), i);
      states = atk_object_ref_state_set (child);
      if (atk_state_set_contains_state (states, ATK_STATE_SELECTED))
        g_array_append_val (array, child);
    }
  g_object_unref (states);

  o = g_array_index (array, AtkObject *, no);
  states = atk_object_ref_state_set (o);
  atk_state_set_remove_state (states, ATK_STATE_SELECTED);

  ret = !atk_state_set_contains_state (states, ATK_STATE_SELECTED);
  g_object_unref (states);
  g_object_unref (o);
  g_object_unref (self);
  g_array_free (array, TRUE);

  return ret;
}

static gboolean
my_atk_selection_select_all_selection (AtkSelection *selection)
{
  MyAtkSelection *self = MY_ATK_SELECTION (selection);
  AtkObject *child = NULL;
  AtkStateSet *states = NULL;
  int i;
  int childs;

  if (!self)
    return FALSE;
  childs = atk_object_get_n_accessible_children (ATK_OBJECT (selection));

  for (i = 0; i < childs; i++)
    {
      child = atk_object_ref_accessible_child (ATK_OBJECT (selection), i);
      states = atk_object_ref_state_set (child);
      atk_state_set_add_state (states, ATK_STATE_SELECTED);
      g_object_unref (states);
      g_object_unref (child);
    }

  g_object_unref (self);
  return TRUE;
}

static void
atk_selection_interface_init (AtkSelectionIface *iface)
{
  if (!iface)
    return;

  iface->add_selection = my_atk_selection_add_selection;
  iface->clear_selection = my_atk_selection_clear_selection;
  iface->ref_selection = my_atk_selection_ref_selection;
  iface->get_selection_count = my_atk_selection_get_selection_count;
  iface->is_child_selected = my_atk_selection_is_child_selected;
  iface->remove_selection = my_atk_selection_remove_selection;
  iface->select_all_selection = my_atk_selection_select_all_selection;
}

static void
my_atk_selection_initialize (AtkObject *obj, gpointer data)
{
}

static void
my_atk_selection_finalize (GObject *obj)
{
}

static void
my_atk_selection_class_init (MyAtkSelectionClass *my_class)
{
  AtkObjectClass *atk_class = ATK_OBJECT_CLASS (my_class);
  GObjectClass *gobject_class = G_OBJECT_CLASS (my_class);

  gobject_class->finalize = my_atk_selection_finalize;

  atk_class->initialize = my_atk_selection_initialize;
}
