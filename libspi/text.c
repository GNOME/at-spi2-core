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

/*
 * component.c : bonobo wrapper for accessible component implementation
 *
 */
#include <config.h>
#include <bonobo/Bonobo.h>

#include <stdio.h>

/*
 * This pulls the CORBA definitions for the "Accessibility::Accessible" server
 */
#include <libspi/Accessibility.h>

/*
 * This pulls the definition of the Text bonobo object
 */
#include "text.h"

/*
 * Static function declarations
 */

static void
text_class_init (TextClass *klass);
static void
text_init (Text *text);
static void
text_finalize (GObject *obj);
static CORBA_string
impl_getText (PortableServer_Servant _servant,
	      const CORBA_long startOffset,
	      const CORBA_long endOffset,
	      CORBA_Environment * ev);
CORBA_string
impl_getTextAfterOffset (PortableServer_Servant _servant,
			 const CORBA_long offset,
			 const
			 Accessibility_TEXT_BOUNDARY_TYPE
			 type, CORBA_long * startOffset,
			 CORBA_long * endOffset,
			 CORBA_Environment * ev);
static CORBA_string
impl_getTextAtOffset (PortableServer_Servant _servant,
		      const CORBA_long offset,
		      const Accessibility_TEXT_BOUNDARY_TYPE
		      type, CORBA_long * startOffset,
		      CORBA_long * endOffset,
		      CORBA_Environment * ev);
static CORBA_wchar
impl_getCharacterAtOffset (PortableServer_Servant _servant,
			   const CORBA_long offset,
			   CORBA_Environment * ev);
static CORBA_string
impl_getTextBeforeOffset (PortableServer_Servant _servant,
			  const CORBA_long offset,
			  const
			  Accessibility_TEXT_BOUNDARY_TYPE
			  type, CORBA_long * startOffset,
			  CORBA_long * endOffset,
			  CORBA_Environment * ev);
static CORBA_long
impl__get_caretOffset (PortableServer_Servant _servant,
		     CORBA_Environment * ev);
static CORBA_string
impl_getAttributes (PortableServer_Servant _servant,
		       const CORBA_long offset,
		       CORBA_long * startOffset,
		       CORBA_long * endOffset,
		       CORBA_Environment * ev);
static void 
impl_getCharacterExtents (PortableServer_Servant _servant,
			  const CORBA_long offset, CORBA_long * x,
			  CORBA_long * y, CORBA_long * width,
			  CORBA_long * height,
			  const CORBA_short coordType,
			  CORBA_Environment * ev);
static CORBA_long
impl__get_characterCount (PortableServer_Servant _servant,
			CORBA_Environment * ev);
static CORBA_long
impl_getOffsetAtPoint (PortableServer_Servant _servant,
		       const CORBA_long x, const CORBA_long y,
		       const CORBA_short coordType,
		       CORBA_Environment * ev);
static CORBA_long
impl_getNSelections (PortableServer_Servant _servant,
		     CORBA_Environment * ev);
static void 
impl_getSelection (PortableServer_Servant _servant,
		   const CORBA_long selectionNum,
		   CORBA_long * startOffset, CORBA_long * endOffset,
		   CORBA_Environment * ev);
static CORBA_boolean
impl_addSelection (PortableServer_Servant _servant,
		   const CORBA_long startOffset,
		   const CORBA_long endOffset,
		   CORBA_Environment * ev);
static CORBA_boolean
impl_removeSelection (PortableServer_Servant _servant,
		      const CORBA_long selectionNum,
		      CORBA_Environment * ev);
static CORBA_boolean
impl_setSelection (PortableServer_Servant _servant,
		   const CORBA_long selectionNum,
		   const CORBA_long startOffset,
		   const CORBA_long endOffset,
		   CORBA_Environment * ev);
static CORBA_boolean
impl_setCaretOffset (PortableServer_Servant _servant,
		     const CORBA_long value,
		     CORBA_Environment * ev);
  
 

static GObjectClass *parent_class;

GType
text_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo = {
      sizeof (TextClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) text_class_init,
      (GClassFinalizeFunc) NULL,
      NULL, /* class data */
      sizeof (Text),
      0, /* n preallocs */
      (GInstanceInitFunc) text_init,
                        NULL /* value table */
    };

    /*
     * Bonobo_type_unique auto-generates a load of
     * CORBA structures for us. All derived types must
     * use bonobo_type_unique.
     */
    type = bonobo_type_unique (
			       BONOBO_OBJECT_TYPE,
			       POA_Accessibility_Text__init,
			       NULL,
			       G_STRUCT_OFFSET (TextClass, epv),
			       &tinfo,
			       "AccessibleText");
  }

  return type;
}

static void
text_class_init (TextClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_Text__epv *epv = &klass->epv;
  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = text_finalize;


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
text_init (Text *text)
{
}

static void
text_finalize (GObject *obj)
{
  Text *text = TEXT (obj);
  g_object_unref (text->atko);
  text->atko = NULL;
  parent_class->finalize (obj);
}

Text *
text_interface_new (AtkObject *obj)
{
  Text *new_text = 
    TEXT(g_object_new (TEXT_TYPE, NULL));
  new_text->atko = obj;
  g_object_ref (obj);
  return new_text;
}



static CORBA_string
impl_getText (PortableServer_Servant _servant,
	      const CORBA_long startOffset,
	      const CORBA_long endOffset,
	      CORBA_Environment * ev)
{
  Text *text = TEXT (bonobo_object_from_servant (_servant));
  gchar *txt;
  CORBA_char *rv;

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
  Text *text = TEXT (bonobo_object_from_servant (_servant));
  gchar *txt;
  CORBA_char *rv;


  txt = atk_text_get_text_after_offset (ATK_TEXT(text->atko),
				    (gint) offset, (AtkTextBoundary) type,
				    (gint *) startOffset, (gint *) endOffset);
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
		      const Accessibility_TEXT_BOUNDARY_TYPE
		      type, CORBA_long * startOffset,
		      CORBA_long * endOffset,
		      CORBA_Environment * ev)
{
  Text *text = TEXT (bonobo_object_from_servant (_servant));
  gchar *txt;
  CORBA_char *rv;

  txt = atk_text_get_text_at_offset (ATK_TEXT(text->atko),
				    (gint) offset, (AtkTextBoundary) type,
				    (gint *) startOffset, (gint *) endOffset);
  if (txt)
    {
      rv = CORBA_string_dup (txt);
      g_free (txt);
    }
  else
    rv = CORBA_string_dup ("");
  return rv;
}



static CORBA_wchar
impl_getCharacterAtOffset (PortableServer_Servant _servant,
			   const CORBA_long offset,
			   CORBA_Environment * ev)
{
  Text *text = TEXT (bonobo_object_from_servant (_servant));
  return (CORBA_wchar)
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
  Text *text = TEXT (bonobo_object_from_servant (_servant));
  gchar *txt;
  CORBA_char *rv;

  txt = atk_text_get_text_before_offset (ATK_TEXT(text->atko),
				    (gint) offset, (AtkTextBoundary) type,
				    (gint *) startOffset, (gint *) endOffset);
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
  Text *text = TEXT (bonobo_object_from_servant (_servant));
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
  Text *text = TEXT (bonobo_object_from_servant (_servant));
  g_print ("getAttributes not implemented.\n");
}



static void 
impl_getCharacterExtents (PortableServer_Servant _servant,
			  const CORBA_long offset, CORBA_long * x,
			  CORBA_long * y, CORBA_long * width,
			  CORBA_long * height,
			  const CORBA_short coordType,
			  CORBA_Environment * ev)
{
  Text *text = TEXT (bonobo_object_from_servant (_servant));
  atk_text_get_character_extents (ATK_TEXT(text->atko), (gint) offset,
				  (gint *) x, (gint *) y, (gint *) width, (gint *) height,
				  (AtkCoordType) coordType);
}



static CORBA_long
impl__get_characterCount (PortableServer_Servant _servant,
			CORBA_Environment * ev)
{
  Text *text = TEXT (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_text_get_character_count (ATK_TEXT(text->atko));
}



static CORBA_long
impl_getOffsetAtPoint (PortableServer_Servant _servant,
		       const CORBA_long x, const CORBA_long y,
		       const CORBA_short coordType,
		       CORBA_Environment * ev)
{
  Text *text = TEXT (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_text_get_offset_at_point (ATK_TEXT(text->atko),
				  (gint) x, (gint) y, (AtkCoordType) coordType);
}



static CORBA_long
impl_getNSelections (PortableServer_Servant _servant,
		     CORBA_Environment * ev)
{
  Text *text = TEXT (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_text_get_n_selections (ATK_TEXT(text->atko));
}



static void 
impl_getSelection (PortableServer_Servant _servant,
		   const CORBA_long selectionNum,
		   CORBA_long * startOffset, CORBA_long * endOffset,
		   CORBA_Environment * ev)
{
  Text *text = TEXT (bonobo_object_from_servant (_servant));
  atk_text_get_selection (ATK_TEXT(text->atko), (gint) selectionNum,
			  (gint *) startOffset, (gint *) endOffset);
}



static CORBA_boolean
impl_addSelection (PortableServer_Servant _servant,
		   const CORBA_long startOffset,
		   const CORBA_long endOffset,
		   CORBA_Environment * ev)
{
  Text *text = TEXT (bonobo_object_from_servant (_servant));
  return (CORBA_boolean)
    atk_text_add_selection (ATK_TEXT(text->atko),
			    (gint) startOffset, (gint) endOffset);
}



static CORBA_boolean
impl_removeSelection (PortableServer_Servant _servant,
		      const CORBA_long selectionNum,
		      CORBA_Environment * ev)
{
  Text *text = TEXT (bonobo_object_from_servant (_servant));
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
  Text *text = TEXT (bonobo_object_from_servant (_servant));
  return (CORBA_boolean)
    atk_text_set_selection (ATK_TEXT(text->atko),
			    (gint) selectionNum, (gint) startOffset, (gint) endOffset);
}



static CORBA_boolean
impl_setCaretOffset (PortableServer_Servant _servant,
		     const CORBA_long value,
		     CORBA_Environment * ev)
{
  Text *text = TEXT (bonobo_object_from_servant (_servant));
  return (CORBA_boolean)
    atk_text_set_caret_offset (ATK_TEXT(text->atko), (gint) value);
}



static void 
impl_getRowColAtOffset (PortableServer_Servant _servant,
			const CORBA_long offset, CORBA_long * row,
			CORBA_long * column, CORBA_Environment * ev)
{
  Text *text = TEXT (bonobo_object_from_servant (_servant));
}

