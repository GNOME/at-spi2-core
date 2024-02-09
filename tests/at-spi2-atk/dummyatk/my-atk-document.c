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
#include <glib-object.h>

#include "my-atk-document.h"
#include "my-atk-object.h"

static void atk_document_interface_init (AtkDocumentIface *iface);

G_DEFINE_TYPE_WITH_CODE (MyAtkDocument,
                         my_atk_document,
                         MY_TYPE_ATK_OBJECT,
                         G_IMPLEMENT_INTERFACE (ATK_TYPE_DOCUMENT,
                                                atk_document_interface_init));

void
my_atk_set_document (AtkDocument *obj, gint page, gint page_num)
{
  g_return_if_fail (MY_IS_ATK_DOCUMENT (obj));
  MyAtkDocument *self = MY_ATK_DOCUMENT (obj);

  self->pages = page;
  self->current_page = page_num;

  AtkAttribute *attr1, *attr2;
  attr1 = g_malloc (sizeof (AtkAttribute));
  attr1->name = g_strdup ("atspi1");
  attr1->value = g_strdup ("test1");

  attr2 = g_malloc (sizeof (AtkAttribute));
  attr2->name = g_strdup ("atspi2");
  attr2->value = g_strdup ("test2");

  self->attributes = g_slist_append (NULL, attr1);
  self->attributes = g_slist_append (self->attributes, attr2);
}

static void
my_atk_document_init (MyAtkDocument *obj)
{
  obj->disposed = FALSE;
  obj->locale = NULL;
  obj->document_type = NULL;
  obj->pages = 0;
  obj->current_page = 0;
}

AtkAttributeSet *
my_atk_document_get_document_attributes (AtkDocument *document)
{
  MyAtkDocument *self = MY_ATK_DOCUMENT (document);

  return self->attributes;
}

const gchar *
my_atk_document_get_document_attribute_value (AtkDocument *document, const gchar *value)
{
  AtkAttributeSet *attr = my_atk_document_get_document_attributes (document);
  GSList *cur_attr = (GSList *) attr;
  AtkAttribute *at;

  while (cur_attr)
    {
      at = (AtkAttribute *) cur_attr->data;
      if (!g_strcmp0 (at->name, value))
        {
          return at->value;
        }
      cur_attr = cur_attr->next;
    }
  return NULL;
}

gboolean
my_atk_document_set_document_attribute (AtkDocument *document, const gchar *attribute_name, const gchar *attribute_value)
{
  return FALSE;
}

gint
my_atk_document_get_current_page_number (AtkDocument *document)
{
  return 0;
}

gint
my_atk_document_get_page_count (AtkDocument *document)
{
  return 0;
}

const gchar *
my_atk_document_get_document_locale (AtkDocument *document)
{

  return g_strdup ("document_locale");
}

static GArray *
my_atk_document_get_text_selections (AtkDocument *document)
{
  g_return_val_if_fail (MY_IS_ATK_DOCUMENT (document), NULL);
  MyAtkDocument *self = MY_ATK_DOCUMENT (document);

  return self->text_selections;
}

static gboolean
my_atk_document_set_text_selections (AtkDocument *document, GArray *selections)
{
  g_return_val_if_fail (MY_IS_ATK_DOCUMENT (document), FALSE);
  MyAtkDocument *self = MY_ATK_DOCUMENT (document);

  if (self->text_selections)
    g_array_free (self->text_selections, TRUE);
  self->text_selections = g_array_copy (selections);
  return TRUE;
}

static void
atk_document_interface_init (AtkDocumentIface *iface)
{
  if (!iface)
    return;

  iface->get_document_locale = my_atk_document_get_document_locale;
  iface->get_document_attributes = my_atk_document_get_document_attributes;
  iface->get_document_attribute_value = my_atk_document_get_document_attribute_value;
  iface->set_document_attribute = my_atk_document_set_document_attribute;
  iface->get_current_page_number = my_atk_document_get_current_page_number;
  iface->get_page_count = my_atk_document_get_page_count;
  iface->get_text_selections = my_atk_document_get_text_selections;
  iface->set_text_selections = my_atk_document_set_text_selections;
}

static void
my_atk_document_initialize (AtkObject *obj, gpointer data)
{
}

static void
my_atk_document_finalize (GObject *object)
{
}

static void
my_atk_document_class_init (MyAtkDocumentClass *my_class)
{
  AtkObjectClass *atk_class = ATK_OBJECT_CLASS (my_class);
  GObjectClass *gobject_class = G_OBJECT_CLASS (my_class);

  gobject_class->finalize = my_atk_document_finalize;

  atk_class->initialize = my_atk_document_initialize;
}
