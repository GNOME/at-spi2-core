/*
 * Copyright 2008 Codethink Ltd.
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
#include <glib.h>
#include <stdio.h>
#include <string.h>

#include "my-atk-hyperlink.h"
#include "my-atk-object.h"
#include "my-atk-text.h"

typedef struct _MyAtkHyperlinkInfo MyAtkHyperlinkInfo;

G_DEFINE_TYPE (MyAtkHyperlink, my_atk_hyperlink, ATK_TYPE_HYPERLINK)

gint
my_atk_set_hyperlink (AtkHyperlink *obj, const gchar *uri, gint start, gint end)
{
  MyAtkHyperlink *self = MY_ATK_HYPERLINK (obj);
  g_return_val_if_fail (MY_IS_ATK_HYPERLINK (obj), -1);

  self->uri = g_strdup (uri);
  self->start = start;
  self->end = end;
  self->state = FALSE;
  self->selected = FALSE;

  return 0;
}

MyAtkHyperlink *
new_MyAtkHyperlink (void)
{
  return g_object_new (MY_TYPE_ATK_HYPERLINK, NULL);
}

static gchar *
my_atk_hyperlink_get_uri (AtkHyperlink *obj, gint i)
{
  MyAtkHyperlink *self = MY_ATK_HYPERLINK (obj);
  g_return_val_if_fail (MY_IS_ATK_HYPERLINK (obj), NULL);

  return self->uri;
}

static AtkObject *
my_atk_hyperlink_get_object (AtkHyperlink *obj, gint i)
{
  MyAtkHyperlink *self = MY_ATK_HYPERLINK (obj);
  MyAtkText *text;
  g_return_val_if_fail (MY_IS_ATK_HYPERLINK (obj), NULL);

  text = my_atk_text_new ();
  my_atk_set_text (ATK_TEXT (text),
                   self->uri,
                   10,
                   50,
                   self->end - self->start,
                   15,
                   NULL);

  return ATK_OBJECT (text);
}

static gint
my_atk_hyperlink_get_end_index (AtkHyperlink *obj)
{
  MyAtkHyperlink *self = MY_ATK_HYPERLINK (obj);
  g_return_val_if_fail (MY_IS_ATK_HYPERLINK (obj), -1);

  return self->end;
}

static gint
my_atk_hyperlink_get_start_index (AtkHyperlink *obj)
{
  MyAtkHyperlink *self = MY_ATK_HYPERLINK (obj);
  g_return_val_if_fail (MY_IS_ATK_HYPERLINK (obj), -1);

  return self->start;
}

static gboolean
my_atk_hyperlink_is_valid (AtkHyperlink *obj)
{
  g_return_val_if_fail (MY_IS_ATK_HYPERLINK (obj), FALSE);

  return TRUE;
}

static gint
my_atk_hyperlink_get_n_anchors (AtkHyperlink *obj)
{
  g_return_val_if_fail (MY_IS_ATK_HYPERLINK (obj), -1);

  return 1;
}

static guint
my_atk_hyperlink_link_state (AtkHyperlink *obj)
{
  MyAtkHyperlink *self = MY_ATK_HYPERLINK (obj);
  g_return_val_if_fail (MY_IS_ATK_HYPERLINK (obj), -1);

  return self->state;
}

static gboolean
my_atk_hyperlink_is_selected_link (AtkHyperlink *obj)
{
  MyAtkHyperlink *self = MY_ATK_HYPERLINK (obj);
  g_return_val_if_fail (MY_IS_ATK_HYPERLINK (obj), FALSE);

  return self->selected;
}

static void
my_atk_hyperlink_link_activated (AtkHyperlink *obj)
{
  MyAtkHyperlink *self = MY_ATK_HYPERLINK (obj);
  g_return_if_fail (MY_IS_ATK_HYPERLINK (obj));

  self->selected = !self->selected;
}

static void
my_atk_hyperlink_init (MyAtkHyperlink *self)
{
  self->uri = NULL;
  self->start = -1;
  self->end = -1;
  self->state = FALSE;
  self->selected = FALSE;
}

static void
my_atk_hyperlink_set_property (GObject *obj,
                               guint id,
                               const GValue *value,
                               GParamSpec *spec)
{
  switch (id)
    {
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    default:
      // G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, id, pspec);
      break;
    }
}

static void
my_atk_hyperlink_get_property (GObject *obj,
                               guint id,
                               GValue *value,
                               GParamSpec *pspec)
{
}

static void
my_atk_hyperlink_class_finalize (GObject *obj)
{
}

static void
my_atk_hyperlink_class_init (MyAtkHyperlinkClass *my_class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (my_class);
  AtkHyperlinkClass *iface = ATK_HYPERLINK_CLASS (my_class);
  const gchar *inst_tab[] = { "accessible-name", "accessible-description", "accessible-role", NULL };

  gobject_class->finalize = my_atk_hyperlink_class_finalize;
  gobject_class->set_property = my_atk_hyperlink_set_property;
  gobject_class->get_property = my_atk_hyperlink_get_property;

  gint i;
  for (i = 0; inst_tab[i] != NULL; i++)
    g_object_class_install_property (gobject_class,
                                     i + 1,
                                     g_param_spec_string (inst_tab[i],
                                                          inst_tab[i],
                                                          inst_tab[i],
                                                          NULL,
                                                          G_PARAM_READWRITE));

  iface->get_uri = my_atk_hyperlink_get_uri;
  iface->get_object = my_atk_hyperlink_get_object;
  iface->get_end_index = my_atk_hyperlink_get_end_index;
  iface->get_start_index = my_atk_hyperlink_get_start_index;
  iface->is_valid = my_atk_hyperlink_is_valid;
  iface->get_n_anchors = my_atk_hyperlink_get_n_anchors;
  iface->link_state = my_atk_hyperlink_link_state;
  iface->is_selected_link = my_atk_hyperlink_is_selected_link;
  iface->link_activated = my_atk_hyperlink_link_activated;
}
