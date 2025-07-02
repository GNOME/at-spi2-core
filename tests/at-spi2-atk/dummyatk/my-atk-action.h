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

#pragma once

#include <atk/atk.h>
#include <glib-object.h>
#include <glib.h>

#include "my-atk-object.h"

#define MY_TYPE_ATK_ACTION (my_atk_action_get_type ())
#define MY_ATK_ACTION(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ATK_ACTION, MyAtkAction))
#define MY_ATK_ACTION_CLASS(vtable) (G_TYPE_CHECK_CLASS_CAST ((vtable), MY_TYPE_ATK_ACTION, MyAtkActionClass))
#define MY_IS_ATK_ACTION(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ATK_ACTION))
#define MY_IS_ATK_ACTION_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), MY_TYPE_ATK_ACTION))
#define MY_ATK_ACTION_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS ((inst), MY_TYPE_ATK_ACTION, MyAtkActionClass))

typedef struct _MyAtkAction MyAtkAction;
typedef struct _MyAtkActionPrivate MyAtkActionPrivate;
typedef struct _MyAtkActionClass MyAtkActionClass;

typedef void (*MyAtkActionFunc) (MyAtkAction *action);

struct _MyAtkAction
{
  MyAtkObject parent;
  MyAtkActionPrivate *priv;
  gint last_performed_action; // this field is changed when action is performed
};

struct _MyAtkActionClass
{
  MyAtkObjectClass parent;
};

GType my_atk_action_get_type (void);

guint my_atk_action_add_action (MyAtkAction *action,
                                const gchar *action_name,
                                const gchar *action_description,
                                const gchar *action_keybinding);
