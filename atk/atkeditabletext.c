/* ATK - The Accessibility Toolkit for GTK+
 * Copyright 2001 Sun Microsystems Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "atkeditabletext.h"

enum {
  TEXT_CHANGED,
  CARET_MOVED,
  LAST_SIGNAL
};

struct _AtkEditableTextIfaceClass
{
  GObjectClass parent;
};

typedef struct _AtkEditableTextIfaceClass AtkEditableTextIfaceClass;

#if 0
static void atk_editable_text_interface_init (AtkEditableTextIfaceClass *klass);

static gpointer parent_class = NULL;
#endif


GType
atk_editable_text_get_type ()
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo =
    {
      sizeof (AtkEditableTextIface),
      NULL,
      NULL,

    };

    type = g_type_register_static (G_TYPE_INTERFACE, "AtkEditableText", &tinfo, 0);
  }

  return type;
}

/*
 * Additional GObject properties exported by AtkText:
 *    "accessible_text" (accessible text has changed)
 *    "accessible_caret" (accessible text cursor position changed:
 *                         editable text only)
 */

#if 0
static void
atk_editable_text_interface_init (AtkEditableTextIfaceClass *klass)
{
  parent_class = g_type_class_ref (ATK_TYPE_EDITABLE_TEXT);

  /* Note that text_changed signal supports details "insert", "delete", possibly "replace". */

  atk_signals[TEXT_CHANGED] =
    g_signal_newc ("text_changed",
                   G_TYPE_FROM_CLASS (klass),
                   G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                   G_STRUCT_OFFSET (AtkEditableTextClass, text_changed), /* still need to declare and define this func */
                   NULL,
                   g_cclosure_marshal_VOID__UINT_POINTER,
                   G_TYPE_NONE,
                   2, G_TYPE_UINT, ATK_TYPE_OBJECT);

  atk_signals[CARET_MOVED] =
    g_signal_newc ("text_caret_moved",
                   G_TYPE_FROM_CLASS (klass),
                   G_SIGNAL_RUN_LAST,
                   G_STRUCT_OFFSET (AtkTextClass, caret_changed), /* still need to declare and define this func */
                   NULL,
                   g_cclosure_marshal_VOID__UINT_POINTER,
                   G_TYPE_NONE,
                   2, G_TYPE_UINT, ATK_TYPE_OBJECT);
}
#endif

void 
atk_editable_text_select_text (AtkEditableText  *text,
                               gint             start_pos,
                               gint             end_pos)
{
  AtkEditableTextIface *iface;

  g_return_if_fail (text != NULL);
  g_return_if_fail (ATK_IS_EDITABLE_TEXT (text));

  iface = ATK_EDITABLE_TEXT_GET_IFACE (text);

  if (iface->select_text)
    (*(iface->select_text)) (text, start_pos, end_pos);
}

void 
atk_editable_text_set_attributes (AtkEditableText  *text,
                                  gint             start_pos,
                                  gint             end_pos,
                                  PangoAttrList    *attributes)
{
  AtkEditableTextIface *iface;

  g_return_if_fail (text != NULL);
  g_return_if_fail (ATK_IS_EDITABLE_TEXT (text));

  iface = ATK_EDITABLE_TEXT_GET_IFACE (text);

  if (iface->set_attributes)
    (*(iface->set_attributes)) (text, start_pos, end_pos, attributes);
}

void 
atk_editable_text_set_text_contents (AtkEditableText  *text,
                                     const gchar      *string)
{
  AtkEditableTextIface *iface;

  g_return_if_fail (text != NULL);
  g_return_if_fail (ATK_IS_EDITABLE_TEXT (text));

  iface = ATK_EDITABLE_TEXT_GET_IFACE (text);

  if (iface->set_text_contents)
    (*(iface->set_text_contents)) (text, string);
}

void 
atk_editable_text_insert_text (AtkEditableText  *text,
                               const gchar      *string,
                               gint             length,
                               gint             *position)
{
  AtkEditableTextIface *iface;

  g_return_if_fail (text != NULL);
  g_return_if_fail (ATK_IS_EDITABLE_TEXT (text));

  iface = ATK_EDITABLE_TEXT_GET_IFACE (text);

  if (iface->insert_text)
    (*(iface->insert_text)) (text, string, length, position);
}

void 
atk_editable_text_copy_text (AtkEditableText  *text,
                             gint             start_pos,
                             gint             end_pos)
{
  AtkEditableTextIface *iface;

  g_return_if_fail (text != NULL);
  g_return_if_fail (ATK_IS_EDITABLE_TEXT (text));

  iface = ATK_EDITABLE_TEXT_GET_IFACE (text);

  if (iface->copy_text)
    (*(iface->copy_text)) (text, start_pos, end_pos);
}

void 
atk_editable_text_cut_text  (AtkEditableText  *text,
                             gint             start_pos,
                             gint             end_pos)
{
  AtkEditableTextIface *iface;

  g_return_if_fail (text != NULL);
  g_return_if_fail (ATK_IS_EDITABLE_TEXT (text));

  iface = ATK_EDITABLE_TEXT_GET_IFACE (text);

  if (iface->cut_text)
    (*(iface->cut_text)) (text, start_pos, end_pos);
}

void 
atk_editable_text_delete_text (AtkEditableText  *text,
                               gint             start_pos,
                               gint             end_pos)
{
  AtkEditableTextIface *iface;

  g_return_if_fail (text != NULL);
  g_return_if_fail (ATK_IS_EDITABLE_TEXT (text));

  iface = ATK_EDITABLE_TEXT_GET_IFACE (text);

  if (iface->delete_text)
    (*(iface->delete_text)) (text, start_pos, end_pos);
}

void 
atk_editable_text_paste_text (AtkEditableText  *text,
                              gint             position)
{
  AtkEditableTextIface *iface;

  g_return_if_fail (text != NULL);
  g_return_if_fail (ATK_IS_EDITABLE_TEXT (text));

  iface = ATK_EDITABLE_TEXT_GET_IFACE (text);

  if (iface->paste_text)
    (*(iface->paste_text)) (text, position);
}
