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

#include "my-atk-action.h"
#include "my-atk-object.h"

typedef struct _MyAtkActionInfo MyAtkActionInfo;

struct _MyAtkActionInfo
{
  gchar *name;
  gchar *description;
  gchar *keybinding;

  MyAtkActionFunc do_action_func;
};

static void atk_action_interface_init (AtkActionIface *iface);

struct _MyAtkActionPrivate
{
  GQueue *action_queue;
  guint action_idle_handler;
  GList *action_list;
  GList *children;
};

G_DEFINE_TYPE_WITH_CODE (MyAtkAction,
                         my_atk_action,
                         MY_TYPE_ATK_OBJECT,
                         G_IMPLEMENT_INTERFACE (ATK_TYPE_ACTION,
                                                atk_action_interface_init)
                             G_ADD_PRIVATE (MyAtkAction));

static void
my_atk_action_initialize (AtkObject *obj, gpointer data)
{
}

static void
my_atk_action_init (MyAtkAction *action_obj)
{
  MyAtkActionPrivate *priv = my_atk_action_get_instance_private (action_obj);
  action_obj->priv = priv;
  priv->action_queue = NULL;
  priv->action_idle_handler = 0;
  priv->action_list = NULL;
  priv->children = NULL;
}

static void
my_atk_action_finalize (GObject *object)
{
}

static void
my_atk_action_class_init (MyAtkActionClass *my_class)
{
  AtkObjectClass *atk_class = ATK_OBJECT_CLASS (my_class);
  GObjectClass *gobject_class = G_OBJECT_CLASS (my_class);

  gobject_class->finalize = my_atk_action_finalize;

  atk_class->initialize = my_atk_action_initialize;
}

static MyAtkActionInfo *
_my_atk_action_get_action_info (MyAtkAction *action, gint i)
{
  MyAtkActionPrivate *priv = NULL;
  MyAtkActionInfo *node_data = NULL;

  g_return_val_if_fail (MY_IS_ATK_ACTION (action), NULL);

  priv = action->priv;

  if (priv->action_list == NULL)
    return NULL;

  node_data = g_list_nth_data (priv->action_list, i);

  g_return_val_if_fail (node_data, NULL);

  return node_data;
}

static const gchar *
my_atk_action_description_get (AtkAction *action, gint i)
{
  MyAtkAction *my_action = NULL;
  MyAtkActionInfo *info = NULL;

  g_return_val_if_fail (MY_IS_ATK_ACTION (action), NULL);
  my_action = MY_ATK_ACTION (action);

  info = _my_atk_action_get_action_info (my_action, i);

  if (info == NULL)
    return NULL;

  return strdup (info->description);
}

static gboolean
my_atk_action_description_set (AtkAction *action, gint i, const char *des)
{
  MyAtkAction *my_action = NULL;
  MyAtkActionInfo *info = NULL;

  g_return_val_if_fail (MY_IS_ATK_ACTION (action), FALSE);
  my_action = MY_ATK_ACTION (action);

  info = _my_atk_action_get_action_info (my_action, i);

  if (info == NULL)
    return FALSE;

  g_free (info->description);
  info->description = g_strdup (des);

  return TRUE;
}

static const gchar *
my_atk_action_name_get (AtkAction *action, gint i)
{
  MyAtkAction *my_action = NULL;
  MyAtkActionInfo *info = NULL;

  g_return_val_if_fail (MY_IS_ATK_ACTION (action), NULL);
  my_action = MY_ATK_ACTION (action);

  info = _my_atk_action_get_action_info (my_action, i);

  if (info == NULL)
    return NULL;

  return strdup (info->name);
}

static const gchar *
my_atk_action_localized_name_get (AtkAction *action, gint i)
{
  MyAtkAction *my_action = NULL;
  MyAtkActionInfo *info = NULL;

  g_return_val_if_fail (MY_IS_ATK_ACTION (action), NULL);
  my_action = MY_ATK_ACTION (action);

  info = _my_atk_action_get_action_info (my_action, i);

  if (info == NULL)
    return NULL;

  return strdup (info->name);
}

static gint
my_atk_action_get_n_actions (AtkAction *action)
{
  MyAtkAction *action_obj = NULL;
  MyAtkActionPrivate *priv = NULL;

  action_obj = MY_ATK_ACTION (action);
  priv = action_obj->priv;

  return g_list_length (priv->action_list);
}

static const gchar *
my_atk_action_get_keybinding (AtkAction *action, gint i)
{
  MyAtkAction *my_action = NULL;
  MyAtkActionInfo *info = NULL;

  g_return_val_if_fail (MY_IS_ATK_ACTION (action), NULL);
  my_action = MY_ATK_ACTION (action);

  info = _my_atk_action_get_action_info (my_action, i);

  if (info == NULL)
    return NULL;

  return strdup (info->keybinding);
}

void
perform_action (AtkObject *obj)
{
  AtkStateSet *state_set1 = atk_object_ref_state_set (obj);
  atk_state_set_add_state (state_set1, ATK_STATE_ACTIVE);
}

static gboolean
my_atk_action_do_action (AtkAction *action, gint i)
{
  g_return_val_if_fail (MY_IS_ATK_ACTION (action), FALSE);

  perform_action (ATK_OBJECT (action));

  return FALSE;
}

guint
my_atk_action_add_action (MyAtkAction *action,
                          const gchar *action_name,
                          const gchar *action_description,
                          const gchar *action_keybinding)
{
  MyAtkActionInfo *info = NULL;
  MyAtkActionPrivate *priv = NULL;

  g_return_val_if_fail (MY_IS_ATK_ACTION (action), -1);

  priv = action->priv;

  info = g_slice_new (MyAtkActionInfo);
  info->name = g_strdup (action_name);
  info->description = g_strdup (action_description);
  info->keybinding = g_strdup (action_keybinding);

  priv->action_list = g_list_append (priv->action_list, info);

  return g_list_length (priv->action_list);
}

static void
atk_action_interface_init (AtkActionIface *iface)
{
  g_return_if_fail (iface);

  iface->do_action = my_atk_action_do_action;

  iface->get_n_actions = my_atk_action_get_n_actions;
  iface->get_description = my_atk_action_description_get;
  iface->get_keybinding = my_atk_action_get_keybinding;
  iface->get_name = my_atk_action_name_get;
  iface->set_description = my_atk_action_description_set;
  iface->get_localized_name = my_atk_action_localized_name_get;
}
