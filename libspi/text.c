/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
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

/* text.c : implements the Text interface */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <atk/atktext.h>
#include <libspi/text.h>

/* Our parent Gtk object type */
#define PARENT_TYPE SPI_TYPE_BASE

static AtkText *
get_text_from_servant (PortableServer_Servant servant)
{
  SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));

  g_return_val_if_fail (object, NULL);
  g_return_val_if_fail (ATK_IS_OBJECT(object->gobj), NULL);
  return ATK_TEXT (object->gobj);
}

static CORBA_string
impl_getText (PortableServer_Servant servant,
	      const CORBA_long       startOffset,
	      const CORBA_long       endOffset,
	      CORBA_Environment     *ev)
{
  gchar *txt;
  CORBA_string rv;
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, CORBA_string_dup (""));
  
  txt = atk_text_get_text (text, startOffset, endOffset);
  if (txt)
    {
      rv = CORBA_string_dup (txt);
      g_free (txt);
    }
  else
    rv = CORBA_string_dup ("");

  return rv;
}


CORBA_string
impl_getTextAfterOffset (PortableServer_Servant servant,
			 const CORBA_long offset,
			 const
			 Accessibility_TEXT_BOUNDARY_TYPE
			 type, CORBA_long * startOffset,
			 CORBA_long * endOffset,
			 CORBA_Environment *ev)
{
  gchar *txt;
  CORBA_char *rv;
  gint intStartOffset, intEndOffset;
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, CORBA_string_dup (""));

  txt = atk_text_get_text_after_offset (text,
					offset, (AtkTextBoundary) type,
					&intStartOffset, &intEndOffset);
  *startOffset = intStartOffset;
  *endOffset = intEndOffset;

  if (txt)
    {
      rv = CORBA_string_dup (txt);
      g_free (txt);
    }
  else
    rv = CORBA_string_dup ("");

  return rv;
}


static CORBA_string
impl_getTextAtOffset (PortableServer_Servant servant,
		      const CORBA_long offset,
		      const Accessibility_TEXT_BOUNDARY_TYPE type,
		      CORBA_long * startOffset,
		      CORBA_long * endOffset,
		      CORBA_Environment *ev)
{
  gchar *txt;
  CORBA_char *rv;
  gint intStartOffset, intEndOffset;
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, CORBA_string_dup (""));

  txt = atk_text_get_text_at_offset (
	  text,
	  offset, (AtkTextBoundary) type,
	  &intStartOffset, &intEndOffset);

  *startOffset = intStartOffset;
  *endOffset = intEndOffset;

  if (txt)
    {
      rv = CORBA_string_dup (txt);
      g_free (txt);
    }
  else
    rv = CORBA_string_dup ("");

  return rv;
}


static CORBA_unsigned_long
impl_getCharacterAtOffset (PortableServer_Servant servant,
			   const CORBA_long offset,
			   CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, 0);

  return atk_text_get_character_at_offset (text, offset);
}


static CORBA_string
impl_getTextBeforeOffset (PortableServer_Servant servant,
			  const CORBA_long offset,
			  const
			  Accessibility_TEXT_BOUNDARY_TYPE
			  type, CORBA_long * startOffset,
			  CORBA_long * endOffset,
			  CORBA_Environment *ev)
{
  gchar *txt;
  CORBA_char *rv;
  gint intStartOffset, intEndOffset;
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, CORBA_string_dup (""));

  txt = atk_text_get_text_before_offset (text,
					 offset, (AtkTextBoundary) type,
					 &intStartOffset, &intEndOffset);

  *startOffset = intStartOffset;
  *endOffset = intEndOffset;

  if (txt)
    {
      rv = CORBA_string_dup (txt);
      g_free (txt);
    }
  else
    rv = CORBA_string_dup ("");

  return rv;
}


static CORBA_long
impl__get_caretOffset (PortableServer_Servant servant,
		     CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, -1);

  return atk_text_get_caret_offset (text);
}


static CORBA_char *
_string_from_attribute_set (AtkAttributeSet *set)
{
  gchar *attributes, *tmp, *tmp2;
  CORBA_char *rv;
  GSList *cur_attr;
  AtkAttribute *at;
  
  attributes = g_strdup ("");
  cur_attr = (GSList *) set;
  while (cur_attr)
    {
      at = (AtkAttribute *) cur_attr->data;
      tmp = g_strdup_printf ("%s%s:%s%s",
			     ((GSList *)(set) == cur_attr) ? "" : " ",
			     at->name, at->value,
			     (cur_attr->next) ? ", " : "");
      tmp2 = g_strconcat (attributes, tmp, NULL);
      g_free (tmp);
      g_free (attributes);
      attributes = tmp2;
      cur_attr = cur_attr->next;
    }
  rv = CORBA_string_dup (attributes);
  g_free (attributes);
  return rv;
}



static CORBA_string
impl_getAttributes (PortableServer_Servant servant,
		    const CORBA_long offset,
		    CORBA_long * startOffset,
		    CORBA_long * endOffset,
		    CORBA_Environment *ev)
{
  AtkAttributeSet *set;
  gint intstart_offset, intend_offset;
  CORBA_char *rv;
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, CORBA_string_dup (""));

  set = atk_text_get_run_attributes (text, offset,
				     &intstart_offset, &intend_offset);
  *startOffset = intstart_offset;
  *endOffset = intend_offset;
  rv = _string_from_attribute_set (set);
  atk_attribute_set_free (set);
  return rv;  
}


static void 
impl_getCharacterExtents (PortableServer_Servant servant,
			  const CORBA_long offset, CORBA_long * x,
			  CORBA_long * y, CORBA_long * width,
			  CORBA_long * height,
			  const CORBA_short coordType,
			  CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);
  gint ix, iy, iw, ih;

  g_return_if_fail (text != NULL);

  atk_text_get_character_extents (
	  text, offset,
	  &ix, &iy, &iw, &ih,
	  (AtkCoordType) coordType);
  *x = ix;
  *y = iy;
  *width = iw;
  *height = ih;
}


static CORBA_long
impl__get_characterCount (PortableServer_Servant servant,
			  CORBA_Environment    *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, 0);

  return atk_text_get_character_count (text);
}


static CORBA_long
impl_getOffsetAtPoint (PortableServer_Servant servant,
		       const CORBA_long x, const CORBA_long y,
		       const CORBA_short coordType,
		       CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, -1);

  return atk_text_get_offset_at_point (text,
				  x, y,
				  (AtkCoordType) coordType);
}


static CORBA_long
impl_getNSelections (PortableServer_Servant servant,
		     CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, 0);

  return atk_text_get_n_selections (text);
}


static void 
impl_getSelection (PortableServer_Servant servant,
		   const CORBA_long selectionNum,
		   CORBA_long * startOffset, CORBA_long * endOffset,
		   CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);
  gint intStartOffset, intEndOffset;
  
  g_return_if_fail (text != NULL);

  /* atk_text_get_selection returns gchar* which we discard */
  g_free (atk_text_get_selection (text, selectionNum,
				  &intStartOffset, &intEndOffset));
  
  *startOffset = intStartOffset;
  *endOffset = intEndOffset;
}


static CORBA_boolean
impl_addSelection (PortableServer_Servant servant,
		   const CORBA_long startOffset,
		   const CORBA_long endOffset,
		   CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, FALSE);

  return atk_text_add_selection (text,
			    startOffset, endOffset);
}


static CORBA_boolean
impl_removeSelection (PortableServer_Servant servant,
		      const CORBA_long selectionNum,
		      CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, FALSE);

  return atk_text_remove_selection (text, selectionNum);
}


static CORBA_boolean
impl_setSelection (PortableServer_Servant servant,
		   const CORBA_long selectionNum,
		   const CORBA_long startOffset,
		   const CORBA_long endOffset,
		   CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, FALSE);

  return atk_text_set_selection (text,
			    selectionNum, startOffset, endOffset);
}


static CORBA_boolean
impl_setCaretOffset (PortableServer_Servant servant,
		     const CORBA_long value,
		     CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, FALSE);

  return atk_text_set_caret_offset (text, value);
}

static void
spi_text_class_init (SpiTextClass *klass)
{
  POA_Accessibility_Text__epv *epv = &klass->epv;

  /* Initialize epv table */

  epv->getText = impl_getText;
  epv->getTextAfterOffset = impl_getTextAfterOffset;
  epv->getCharacterAtOffset = impl_getCharacterAtOffset;
  epv->getTextAtOffset = impl_getTextAtOffset;
  epv->getTextBeforeOffset = impl_getTextBeforeOffset;
  epv->_get_caretOffset = impl__get_caretOffset;
  epv->getAttributes = impl_getAttributes;
  epv->getCharacterExtents = impl_getCharacterExtents;
  epv->_get_characterCount = impl__get_characterCount;
  epv->getOffsetAtPoint = impl_getOffsetAtPoint;
  epv->getNSelections = impl_getNSelections;
  epv->getSelection = impl_getSelection;
  epv->addSelection = impl_addSelection;
  epv->removeSelection = impl_removeSelection;
  epv->setSelection = impl_setSelection;
  epv->setCaretOffset = impl_setCaretOffset;
}

static void
spi_text_init (SpiText *text)
{
}

BONOBO_TYPE_FUNC_FULL (SpiText,
		       Accessibility_Text,
		       PARENT_TYPE,
		       spi_text);

void
spi_text_construct (SpiText *text, AtkObject *obj)
{
  spi_base_construct (SPI_BASE (text), G_OBJECT(obj));
}


SpiText *
spi_text_interface_new (AtkObject *obj)
{
  SpiText *retval;

  g_return_val_if_fail (ATK_IS_TEXT (obj), NULL);

  retval = g_object_new (SPI_TEXT_TYPE, NULL);

  spi_text_construct (retval, obj);

  return retval;
}
