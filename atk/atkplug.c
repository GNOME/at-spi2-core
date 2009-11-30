/* ATK -  Accessibility Toolkit
 * Copyright (C) 2009 Novell, Inc.
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

#include "atk.h"
#include "atkplug.h"

G_DEFINE_TYPE (AtkPlug, atk_plug, ATK_TYPE_OBJECT);

static void atk_plug_class_init (AtkPlugClass *klass);

static void
atk_plug_init (AtkPlug* obj)
{
}

static void
atk_plug_class_init (AtkPlugClass* klass)
{
  klass->get_object_id = NULL;
}

AtkObject*
atk_plug_new (void)
{
  AtkObject* accessible;
  
  accessible = g_object_new (ATK_TYPE_PLUG, NULL);
  g_return_val_if_fail (accessible != NULL, NULL);

  accessible->role = ATK_ROLE_FILLER;
  accessible->layer = ATK_LAYER_WIDGET;
  
  return accessible;
}

/**
 * atk_plug_get_plug_id:
 * @obj: an #AtkPlug
 *
 * Gets the unique ID of an #AtkPlug object, which can be used to embed inside
 * of an #AtkSocket using atk_socket_embed().
 *
 * Returns: the unique ID for the plug
 **/
gchar*
atk_plug_get_id (AtkPlug* obj)
{
  AtkPlugClass *klass;

  g_return_val_if_fail (ATK_IS_PLUG (obj), NULL);

  klass = g_type_class_peek (ATK_TYPE_PLUG);

  if (klass && klass->get_object_id)
    return (klass->get_object_id) (obj);
  else
    return NULL;
}
