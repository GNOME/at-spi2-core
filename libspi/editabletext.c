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
 * This pulls the definition of the SpiEditableText bonobo object
 */
#include "editabletext.h"

/*
 * Static function declarations
 */

static void
spi_editable_text_class_init (SpiEditableTextClass *klass);
static void
spi_editable_text_init (SpiEditableText *editable);
static void
spi_editable_text_finalize (GObject *obj);
static CORBA_boolean
impl_setAttributes (PortableServer_Servant _servant,
		       const CORBA_char * attributes,
		       const CORBA_long startPos,
		       const CORBA_long endPos,
		       CORBA_Environment * ev);
static void
impl_setTextContents (PortableServer_Servant _servant,
		      const CORBA_char * newContents,
		      CORBA_Environment * ev);
static void 
impl_insertText (PortableServer_Servant _servant,
		 const CORBA_long position,
		 const CORBA_char * text,
		 const CORBA_long length,
		 CORBA_Environment * ev);
static void 
impl_copyText (PortableServer_Servant _servant,
	       const CORBA_long startPos, const CORBA_long endPos,
	       CORBA_Environment * ev);
static void 
impl_cutText (PortableServer_Servant _servant,
	      const CORBA_long startPos, const CORBA_long endPos,
	      CORBA_Environment * ev);
static void 
impl_deleteText (PortableServer_Servant _servant,
		 const CORBA_long startPos, const CORBA_long endPos,
		 CORBA_Environment * ev);
static void
impl_pasteText (PortableServer_Servant _servant,
		const CORBA_long position, CORBA_Environment * ev);

static GObjectClass *parent_class;


BONOBO_TYPE_FUNC_FULL (SpiEditableText,
		       Accessibility_EditableText,
		       SPI_TEXT_TYPE,
		       spi_editable_text);

static void
spi_editable_text_class_init (SpiEditableTextClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_EditableText__epv *epv = &klass->epv;
  parent_class = g_type_interface_peek_parent (klass);

  object_class->finalize = spi_editable_text_finalize;
  
  /* Initialize epv table */

  epv->setAttributes = impl_setAttributes;
  epv->setTextContents = impl_setTextContents;
  epv->insertText = impl_insertText;
  epv->copyText = impl_copyText;
  epv->cutText = impl_cutText;
  epv->deleteText = impl_deleteText;
  epv->pasteText = impl_pasteText;
}

static void
spi_editable_text_init (SpiEditableText *editable)
{
}

static void
spi_editable_text_finalize (GObject *obj)
{
  parent_class->finalize (obj);
}

SpiEditableText *
spi_editable_text_interface_new (AtkObject *obj)
{
  SpiEditableText *new_editable =
    SPI_EDITABLE_TEXT(g_object_new (SPI_EDITABLE_TEXT_TYPE, NULL));
  (SPI_TEXT (new_editable))->atko = obj;
  g_object_ref (obj);
  return new_editable;
}

static CORBA_boolean
impl_setAttributes (PortableServer_Servant _servant,
		       const CORBA_char * attributes,
		       const CORBA_long startPos,
		       const CORBA_long endPos,
					 CORBA_Environment * ev)
{
  SpiEditableText *editable;
  BonoboObject *obj;
  obj = (bonobo_object_from_servant (_servant));
  g_return_val_if_fail (IS_SPI_EDITABLE_TEXT (obj), FALSE);
  editable = SPI_EDITABLE_TEXT(bonobo_object_from_servant (_servant));
  g_return_val_if_fail (ATK_IS_EDITABLE_TEXT ( (SPI_TEXT (obj))->atko), FALSE);

  g_print ("setRunAttributes not implemented.\n");

  return FALSE;
}

static void
impl_setTextContents (PortableServer_Servant _servant,
		      const CORBA_char * newContents,
		      CORBA_Environment * ev)
{
  SpiEditableText *editable;
  BonoboObject *obj;
  obj = (bonobo_object_from_servant (_servant));
  g_return_if_fail (IS_SPI_EDITABLE_TEXT (obj));
  editable = SPI_EDITABLE_TEXT(bonobo_object_from_servant (_servant));
  g_return_if_fail (ATK_IS_EDITABLE_TEXT ( (SPI_TEXT (obj))->atko));
  
  atk_editable_text_set_text_contents (ATK_EDITABLE_TEXT( SPI_TEXT (editable)->atko),
				       (gchar *) newContents);
}



static void 
impl_insertText (PortableServer_Servant _servant,
		 const CORBA_long position,
		 const CORBA_char * text,
		 const CORBA_long length,
		 CORBA_Environment * ev)
{
  SpiEditableText *editable;
  BonoboObject *obj;
  obj = (bonobo_object_from_servant (_servant));
  g_return_if_fail (IS_SPI_EDITABLE_TEXT (obj));
  editable = SPI_EDITABLE_TEXT(bonobo_object_from_servant (_servant));
  g_return_if_fail (ATK_IS_EDITABLE_TEXT ( (SPI_TEXT (obj))->atko));

  atk_editable_text_insert_text (ATK_EDITABLE_TEXT( SPI_TEXT (editable)->atko),
				 (gchar *) text,
				 (gint) length,
				 (gint *) &position);
}


static void 
impl_copyText (PortableServer_Servant _servant,
	       const CORBA_long startPos, const CORBA_long endPos,
	       CORBA_Environment * ev)
{
  SpiEditableText *editable;
  BonoboObject *obj;
  obj = (bonobo_object_from_servant (_servant));
  g_return_if_fail (IS_SPI_EDITABLE_TEXT (obj));
  editable = SPI_EDITABLE_TEXT(bonobo_object_from_servant (_servant));
  g_return_if_fail (ATK_IS_EDITABLE_TEXT ( (SPI_TEXT (obj))->atko));

  atk_editable_text_copy_text (ATK_EDITABLE_TEXT( SPI_TEXT(editable)->atko),
			       (gint) startPos, (gint) endPos);
}



static void 
impl_cutText (PortableServer_Servant _servant,
	      const CORBA_long startPos, const CORBA_long endPos,
	      CORBA_Environment * ev)
{
  SpiEditableText *editable;
  BonoboObject *obj;
  obj = (bonobo_object_from_servant (_servant));
  g_return_if_fail (IS_SPI_EDITABLE_TEXT (obj));
  editable = SPI_EDITABLE_TEXT(bonobo_object_from_servant (_servant));
  g_return_if_fail (ATK_IS_EDITABLE_TEXT ( (SPI_TEXT (obj))->atko));

  atk_editable_text_cut_text (ATK_EDITABLE_TEXT(SPI_TEXT (editable)->atko),
				 (gint) startPos, (gint) endPos);
}




static void 
impl_deleteText (PortableServer_Servant _servant,
		 const CORBA_long startPos, const CORBA_long endPos,
		 CORBA_Environment * ev)
{
  SpiEditableText *editable;
  BonoboObject *obj;
  obj = (bonobo_object_from_servant (_servant));
  g_return_if_fail (IS_SPI_EDITABLE_TEXT (obj));
  editable = SPI_EDITABLE_TEXT(bonobo_object_from_servant (_servant));
  g_return_if_fail (ATK_IS_EDITABLE_TEXT ( (SPI_TEXT (obj))->atko));

  atk_editable_text_delete_text (ATK_EDITABLE_TEXT( SPI_TEXT(editable)->atko),
				 (gint) startPos, (gint) endPos);
}


static void
impl_pasteText (PortableServer_Servant _servant,
		const CORBA_long position, CORBA_Environment * ev)
{
  SpiEditableText *editable;
  BonoboObject *obj;
  obj = (bonobo_object_from_servant (_servant));
  g_return_if_fail (IS_SPI_EDITABLE_TEXT (obj));
  editable = SPI_EDITABLE_TEXT(bonobo_object_from_servant (_servant));
  g_return_if_fail (ATK_IS_EDITABLE_TEXT ( (SPI_TEXT (obj))->atko));

  atk_editable_text_paste_text (ATK_EDITABLE_TEXT( SPI_TEXT(editable)->atko), position);
}

