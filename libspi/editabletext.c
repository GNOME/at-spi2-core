/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

/* editabletext.c : implements the EditableText interface */

#include <config.h>
#include <stdio.h>
#include <atk/atkeditabletext.h>
#include <libspi/editabletext.h>

/* Static function declarations */

static void
spi_editable_text_class_init (SpiEditableTextClass *klass);
static void
spi_editable_text_init (SpiEditableText *editable);
static CORBA_boolean
impl_setAttributes (PortableServer_Servant servant,
		       const CORBA_char * attributes,
		       const CORBA_long startPos,
		       const CORBA_long endPos,
		       CORBA_Environment *ev);
static CORBA_boolean
impl_setTextContents (PortableServer_Servant servant,
		      const CORBA_char * newContents,
		      CORBA_Environment *ev);
static CORBA_boolean 
impl_insertText (PortableServer_Servant servant,
		 const CORBA_long position,
		 const CORBA_char * text,
		 const CORBA_long length,
		 CORBA_Environment *ev);
static void 
impl_copyText (PortableServer_Servant servant,
	       const CORBA_long startPos, const CORBA_long endPos,
	       CORBA_Environment *ev);
static CORBA_boolean
impl_cutText (PortableServer_Servant servant,
	      const CORBA_long startPos, const CORBA_long endPos,
	      CORBA_Environment *ev);
static CORBA_boolean
impl_deleteText (PortableServer_Servant servant,
		 const CORBA_long startPos, const CORBA_long endPos,
		 CORBA_Environment *ev);
static CORBA_boolean
impl_pasteText (PortableServer_Servant servant,
		const CORBA_long position, CORBA_Environment *ev);

BONOBO_TYPE_FUNC_FULL (SpiEditableText,
		       Accessibility_EditableText,
		       SPI_TEXT_TYPE,
		       spi_editable_text)

static void
spi_editable_text_class_init (SpiEditableTextClass *klass)
{
  POA_Accessibility_EditableText__epv *epv = &klass->epv;

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


SpiEditableText *
spi_editable_text_interface_new (AtkObject *obj)
{
  SpiEditableText *new_editable = g_object_new (
	  SPI_EDITABLE_TEXT_TYPE, NULL);

  spi_text_construct (SPI_TEXT (new_editable), obj);

  return new_editable;
}


static AtkEditableText *
get_editable_text_from_servant (PortableServer_Servant servant)
{
  SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));

  g_return_val_if_fail (object, NULL);
  g_return_val_if_fail (ATK_IS_OBJECT(object->gobj), NULL);
  return ATK_EDITABLE_TEXT (object->gobj);
}


static CORBA_boolean
impl_setAttributes (PortableServer_Servant servant,
		    const CORBA_char * attributes,
		    const CORBA_long startPos,
		    const CORBA_long endPos,
		    CORBA_Environment *ev)
{
  AtkEditableText *editable = get_editable_text_from_servant (servant);

  g_return_val_if_fail (editable != NULL, FALSE);

  g_print ("setRunAttributes not implemented.\n");

  return FALSE;
}


static CORBA_boolean
impl_setTextContents (PortableServer_Servant servant,
		      const CORBA_char     *newContents,
		      CORBA_Environment    *ev)
{
  AtkEditableText *editable = get_editable_text_from_servant (servant);

  g_return_val_if_fail (editable != NULL, FALSE);
  
  atk_editable_text_set_text_contents (editable, newContents);

  return TRUE;
}


static CORBA_boolean
impl_insertText (PortableServer_Servant servant,
		 const CORBA_long      position,
		 const CORBA_char     *text,
		 const CORBA_long      length,
		 CORBA_Environment    *ev)
{
  AtkEditableText *editable = get_editable_text_from_servant (servant);
  gint ip;

  g_return_val_if_fail (editable != NULL, FALSE);

  ip = position;
  atk_editable_text_insert_text (editable,
				 text,
				 length,
				 &ip);
  return TRUE;
}


static void 
impl_copyText (PortableServer_Servant servant,
	       const CORBA_long startPos, const CORBA_long endPos,
	       CORBA_Environment *ev)
{
  AtkEditableText *editable = get_editable_text_from_servant (servant);

  g_return_if_fail (editable != NULL);

  atk_editable_text_copy_text (editable, startPos, endPos);
  
}


static CORBA_boolean
impl_cutText (PortableServer_Servant servant,
	      const CORBA_long startPos, const CORBA_long endPos,
	      CORBA_Environment *ev)
{
  AtkEditableText *editable = get_editable_text_from_servant (servant);

  g_return_val_if_fail (editable != NULL, FALSE);

  atk_editable_text_cut_text (editable, startPos, endPos);

  return TRUE;
}


static CORBA_boolean
impl_deleteText (PortableServer_Servant servant,
		 const CORBA_long startPos, const CORBA_long endPos,
		 CORBA_Environment *ev)
{
  AtkEditableText *editable = get_editable_text_from_servant (servant);

  g_return_val_if_fail (editable != NULL, FALSE);

  atk_editable_text_delete_text (editable, startPos, endPos);

  return TRUE;
}


static CORBA_boolean
impl_pasteText (PortableServer_Servant servant,
		const CORBA_long position, CORBA_Environment *ev)
{
  AtkEditableText *editable = get_editable_text_from_servant (servant);

  g_return_val_if_fail (editable != NULL, FALSE);

  atk_editable_text_paste_text (editable, position);

  return TRUE;
}
