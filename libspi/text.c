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
#include <libspi/text.h>

/* Our parent Gtk object type */
#define PARENT_TYPE BONOBO_TYPE_OBJECT

/* A pointer to our parent object class */
static GObjectClass *spi_text_parent_class;

static CORBA_string
impl_getText (PortableServer_Servant _servant,
	      const CORBA_long startOffset,
	      const CORBA_long endOffset,
	      CORBA_Environment * ev)
{
  SpiText *text;
  gchar *txt;
  CORBA_string rv;
  BonoboObject *obj;
  
  obj = (bonobo_object_from_servant (_servant));
  g_return_val_if_fail (IS_TEXT (obj), (CORBA_char *)"");
  text = SPI_TEXT (obj);
  g_return_val_if_fail (ATK_IS_TEXT (text->atko), (CORBA_char *)"");
  
  txt = atk_text_get_text (ATK_TEXT(text->atko),
		       (gint) startOffset, (gint) endOffset);
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
impl_getTextAfterOffset (PortableServer_Servant _servant,
			 const CORBA_long offset,
			 const
			 Accessibility_TEXT_BOUNDARY_TYPE
			 type, CORBA_long * startOffset,
			 CORBA_long * endOffset,
			 CORBA_Environment * ev)
{
  SpiText *text;
  gchar *txt;
  CORBA_char *rv;
  gint intStartOffset, intEndOffset;
  BonoboObject *obj;

  obj = (bonobo_object_from_servant (_servant));
  g_return_val_if_fail (IS_TEXT (obj), (CORBA_char *)"");
  text = SPI_TEXT (obj);
  g_return_val_if_fail (ATK_IS_TEXT (text->atko), (CORBA_char *)"");
  
  txt = atk_text_get_text_after_offset (ATK_TEXT(text->atko),
				    (gint) offset, (AtkTextBoundary) type,
				    &intStartOffset, &intEndOffset);
  *startOffset = (CORBA_long) intStartOffset;
  *endOffset = (CORBA_long) intEndOffset;

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
impl_getTextAtOffset (PortableServer_Servant _servant,
		      const CORBA_long offset,
		      const Accessibility_TEXT_BOUNDARY_TYPE type,
		      CORBA_long * startOffset,
		      CORBA_long * endOffset,
		      CORBA_Environment * ev)
{
  SpiText *text;
  CORBA_char *txt;
  CORBA_char *rv;
  gint intStartOffset, intEndOffset;
  BonoboObject *obj;

  obj = (bonobo_object_from_servant (_servant));
  g_return_val_if_fail (IS_TEXT (obj), (CORBA_char *)"");
  text = SPI_TEXT (obj);
  g_return_val_if_fail (ATK_IS_TEXT (text->atko), (CORBA_char *)"");

  txt = (CORBA_char *) atk_text_get_text_at_offset (ATK_TEXT (text->atko),
				    (gint) offset, (AtkTextBoundary) type,
				    &intStartOffset, &intEndOffset);
  *startOffset = (CORBA_long) intStartOffset;
  *endOffset = (CORBA_long) intEndOffset;

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
impl_getCharacterAtOffset (PortableServer_Servant _servant,
			   const CORBA_long offset,
			   CORBA_Environment * ev)
{
  SpiText *text;
  BonoboObject *obj;
  obj = (bonobo_object_from_servant (_servant));
  
  g_return_val_if_fail (IS_TEXT (obj), (CORBA_unsigned_long)0);
  text = SPI_TEXT (obj);
  g_return_val_if_fail (ATK_IS_TEXT (text->atko), (CORBA_unsigned_long)0);

  return (CORBA_unsigned_long)
    atk_text_get_character_at_offset (ATK_TEXT(text->atko), (gint) offset);
}


static CORBA_string
impl_getTextBeforeOffset (PortableServer_Servant _servant,
			  const CORBA_long offset,
			  const
			  Accessibility_TEXT_BOUNDARY_TYPE
			  type, CORBA_long * startOffset,
			  CORBA_long * endOffset,
			  CORBA_Environment * ev)
{
  SpiText *text;
  gchar *txt;
  CORBA_char *rv;
  gint intStartOffset, intEndOffset;
  BonoboObject *obj;

  obj = (bonobo_object_from_servant (_servant));
  g_return_val_if_fail (IS_TEXT (obj), (CORBA_char *)"");
  text = SPI_TEXT (obj);
  g_return_val_if_fail (ATK_IS_TEXT (text->atko), (CORBA_char *)"");

  txt = atk_text_get_text_before_offset (ATK_TEXT(text->atko),
				    (gint) offset, (AtkTextBoundary) type,
				    &intStartOffset, &intEndOffset);
  *startOffset = (CORBA_long) intStartOffset;
  *endOffset = (CORBA_long) intEndOffset;

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
impl__get_caretOffset (PortableServer_Servant _servant,
		     CORBA_Environment * ev)
{
  SpiText *text;
  BonoboObject *obj;

  obj = (bonobo_object_from_servant (_servant));
  g_return_val_if_fail (IS_TEXT (obj), (CORBA_long)-1);
  text = SPI_TEXT (obj);
  g_return_val_if_fail (ATK_IS_TEXT (text->atko), (CORBA_long)-1);
  
  return (CORBA_long)
    atk_text_get_caret_offset (ATK_TEXT(text->atko));
}



static CORBA_string
impl_getAttributes (PortableServer_Servant _servant,
		       const CORBA_long offset,
		       CORBA_long * startOffset,
		       CORBA_long * endOffset,
		       CORBA_Environment * ev)
{
  SpiText *text;
  BonoboObject *obj;

  obj = (bonobo_object_from_servant (_servant));
  g_return_val_if_fail (IS_TEXT (obj), (CORBA_char *)"");
  text = SPI_TEXT (obj);
  g_return_val_if_fail (ATK_IS_TEXT (text->atko), (CORBA_char *)"");

  g_print ("getAttributes not yet implemented.\n");

  return CORBA_string_dup ("");
}

static void 
impl_getCharacterExtents (PortableServer_Servant _servant,
			  const CORBA_long offset, CORBA_long * x,
			  CORBA_long * y, CORBA_long * width,
			  CORBA_long * height,
			  const CORBA_short coordType,
			  CORBA_Environment * ev)
{
  SpiText *text;
  BonoboObject *obj;

  obj = (bonobo_object_from_servant (_servant));
  g_return_if_fail (IS_TEXT (obj));
  text = SPI_TEXT (obj);
  g_return_if_fail (ATK_IS_TEXT (text->atko));

  atk_text_get_character_extents (ATK_TEXT(text->atko), (gint) offset,
				  (gint *) x, (gint *) y, (gint *) width, (gint *) height,
				  (AtkCoordType) coordType);
}



static CORBA_long
impl__get_characterCount (PortableServer_Servant _servant,
			CORBA_Environment * ev)
{
  SpiText *text;
  BonoboObject *obj;
  CORBA_long retval;

  obj = (bonobo_object_from_servant (_servant));
  g_return_val_if_fail (IS_TEXT (obj), (CORBA_long)0);
  text = SPI_TEXT (obj);

  retval = (CORBA_long)
    atk_text_get_character_count (ATK_TEXT(text->atko));

  return retval;
}



static CORBA_long
impl_getOffsetAtPoint (PortableServer_Servant _servant,
		       const CORBA_long x, const CORBA_long y,
		       const CORBA_short coordType,
		       CORBA_Environment * ev)
{
  SpiText *text;
  BonoboObject *obj;

  obj = (bonobo_object_from_servant (_servant));
  g_return_val_if_fail (IS_TEXT (obj), (CORBA_long)-1);
  text = SPI_TEXT (obj);

  return (CORBA_long)
    atk_text_get_offset_at_point (ATK_TEXT(text->atko),
				  (gint) x, (gint) y, (AtkCoordType) coordType);
}



static CORBA_long
impl_getNSelections (PortableServer_Servant _servant,
		     CORBA_Environment * ev)
{
  SpiText *text;
  BonoboObject *obj;

  obj = (bonobo_object_from_servant (_servant));
  g_return_val_if_fail (IS_TEXT (obj), (CORBA_long)0);
  text = SPI_TEXT (obj);

  return (CORBA_long)
    atk_text_get_n_selections (ATK_TEXT(text->atko));
}



static void 
impl_getSelection (PortableServer_Servant _servant,
		   const CORBA_long selectionNum,
		   CORBA_long * startOffset, CORBA_long * endOffset,
		   CORBA_Environment * ev)
{
  SpiText *text;
  BonoboObject *obj;

  obj = (bonobo_object_from_servant (_servant));
  g_return_if_fail (IS_TEXT (obj));
  text = SPI_TEXT (obj);

  atk_text_get_selection (ATK_TEXT(text->atko), (gint) selectionNum,
			  (gint *) startOffset, (gint *) endOffset);
}



static CORBA_boolean
impl_addSelection (PortableServer_Servant _servant,
		   const CORBA_long startOffset,
		   const CORBA_long endOffset,
		   CORBA_Environment * ev)
{
  SpiText *text;
  BonoboObject *obj;

  obj = (bonobo_object_from_servant (_servant));
  g_return_val_if_fail (IS_TEXT (obj), (CORBA_boolean)FALSE);
  text = SPI_TEXT (obj);

  return (CORBA_boolean)
    atk_text_add_selection (ATK_TEXT(text->atko),
			    (gint) startOffset, (gint) endOffset);
}



static CORBA_boolean
impl_removeSelection (PortableServer_Servant _servant,
		      const CORBA_long selectionNum,
		      CORBA_Environment * ev)
{
  SpiText *text;
  BonoboObject *obj;

  obj = (bonobo_object_from_servant (_servant));
  g_return_val_if_fail (IS_TEXT (obj), (CORBA_boolean)FALSE);
  text = SPI_TEXT (obj);

  return (CORBA_boolean)
    atk_text_remove_selection (ATK_TEXT(text->atko), (gint) selectionNum);
}



static CORBA_boolean
impl_setSelection (PortableServer_Servant _servant,
		   const CORBA_long selectionNum,
		   const CORBA_long startOffset,
		   const CORBA_long endOffset,
		   CORBA_Environment * ev)
{
  SpiText *text;
  BonoboObject *obj;

  obj = (bonobo_object_from_servant (_servant));
  g_return_val_if_fail (IS_TEXT (obj), (CORBA_boolean)FALSE);
  text = SPI_TEXT (obj);

  return (CORBA_boolean)
    atk_text_set_selection (ATK_TEXT(text->atko),
			    (gint) selectionNum, (gint) startOffset, (gint) endOffset);
}



static CORBA_boolean
impl_setCaretOffset (PortableServer_Servant _servant,
		     const CORBA_long value,
		     CORBA_Environment * ev)
{
  SpiText *text;
  BonoboObject *obj;

  obj = (bonobo_object_from_servant (_servant));
  g_return_val_if_fail (IS_TEXT (obj), (CORBA_boolean)FALSE);
  text = SPI_TEXT (obj);

  return (CORBA_boolean)
    atk_text_set_caret_offset (ATK_TEXT(text->atko), (gint) value);
}



static void 
impl_getRowColAtOffset (PortableServer_Servant _servant,
			const CORBA_long offset, CORBA_long * row,
			CORBA_long * column, CORBA_Environment * ev)
{
  SpiText *text;
  BonoboObject *obj;

  obj = (bonobo_object_from_servant (_servant));
  g_return_if_fail (IS_TEXT (obj));
  text = SPI_TEXT (obj);

  g_print ("getRowColAtOffset not yet implemented\n");
}

static void
spi_text_object_finalize (GObject *obj)
{
  SpiText *text = SPI_TEXT (obj);
  g_object_unref (text->atko);
  text->atko = NULL;
  spi_text_parent_class->finalize (obj);
}

static void
spi_text_class_init (SpiTextClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_Text__epv *epv = &klass->epv;
  spi_text_parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = spi_text_object_finalize;

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

SpiText *
spi_text_interface_new (AtkObject *obj)
{
  SpiText *new_text = g_object_new (SPI_TEXT_TYPE, NULL);
  new_text->atko = obj;
  g_object_ref (obj);
  return new_text;
}
