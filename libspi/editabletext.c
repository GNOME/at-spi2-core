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
 * This pulls the definition of the EditableText bonobo object
 */
#include "editabletext.h"

/*
 * Static function declarations
 */

static void
editable_text_class_init (EditableTextClass *klass);
static void
editable_text_init (EditableText *editable);
static void
editable_text_finalize (GObject *obj);
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

GType
editable_text_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo = {
      sizeof (EditableTextClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) editable_text_class_init,
      (GClassFinalizeFunc) NULL,
      NULL, /* class data */
      sizeof (EditableText),
      0, /* n preallocs */
      (GInstanceInitFunc) editable_text_init,
                        NULL /* value table */
    };

    /*
     * Bonobo_type_unique auto-generates a load of
     * CORBA structures for us. All derived types must
     * use bonobo_type_unique.
     */
    type = bonobo_type_unique (
			       BONOBO_OBJECT_TYPE,
			       POA_Accessibility_EditableText__init,
			       NULL,
			       G_STRUCT_OFFSET (EditableTextClass, epv),
			       &tinfo,
			       "AccessibleEditableText");
  }

  return type;
}

static void
editable_text_class_init (EditableTextClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_EditableText__epv *epv = &klass->epv;
  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = editable_text_finalize;


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
editable_text_init (EditableText *editable)
{
}

static void
editable_text_finalize (GObject *obj)
{
  EditableText *editable = EDITABLE_TEXT(obj);
  editable->atk_editable_text = NULL;
  parent_class->finalize (obj);
}

EditableText *
editable_text_new (AtkEditableText *editable)
{
  EditableText *new_editable =
    EDITABLE_TEXT(g_object_new (EDITABLE_TEXT_TYPE, NULL));
  new_editable->atk_editable_text = editable;
  return new_editable;
}



static CORBA_boolean
impl_setAttributes (PortableServer_Servant _servant,
		       const CORBA_char * attributes,
		       const CORBA_long startPos,
		       const CORBA_long endPos,
					 CORBA_Environment * ev)
{
  g_print ("setRunAttributes not implemented.\n");
}



static void
impl_setTextContents (PortableServer_Servant _servant,
		      const CORBA_char * newContents,
		      CORBA_Environment * ev)
{
  EditableText *editable = EDITABLE_TEXT(bonobo_object_from_servant (_servant));
  atk_editable_text_set_text_contents (editable->atk_editable_text,
				       (gchar *) newContents);
}



static void 
impl_insertText (PortableServer_Servant _servant,
		 const CORBA_long position,
		 const CORBA_char * text,
		 const CORBA_long length,
		 CORBA_Environment * ev)
{
  EditableText *editable = EDITABLE_TEXT (bonobo_object_from_servant(_servant));
  atk_editable_text_insert_text (editable->atk_editable_text,
				 (gchar *) text,
				 (gint) length,
				 (gint *) &position);
}


static void 
impl_copyText (PortableServer_Servant _servant,
	       const CORBA_long startPos, const CORBA_long endPos,
	       CORBA_Environment * ev)
{
  EditableText *editable = EDITABLE_TEXT (bonobo_object_from_servant(_servant));
  atk_editable_text_copy_text (editable->atk_editable_text,
			       (gint) startPos, (gint) endPos);
}



static void 
impl_cutText (PortableServer_Servant _servant,
	      const CORBA_long startPos, const CORBA_long endPos,
	      CORBA_Environment * ev)
{
  EditableText *editable = EDITABLE_TEXT (bonobo_object_from_servant(_servant));
  atk_editable_text_cut_text (editable->atk_editable_text,
				 (gint) startPos, (gint) endPos);
}




static void 
impl_deleteText (PortableServer_Servant _servant,
		 const CORBA_long startPos, const CORBA_long endPos,
		 CORBA_Environment * ev)
{
  EditableText *editable = EDITABLE_TEXT (bonobo_object_from_servant(_servant));
  atk_editable_text_delete_text (editable->atk_editable_text,
				 (gint) startPos, (gint) endPos);
}


static void
impl_pasteText (PortableServer_Servant _servant,
		const CORBA_long position, CORBA_Environment * ev)
{
  EditableText *editable = EDITABLE_TEXT (bonobo_object_from_servant(_servant));
  atk_editable_text_paste_text (editable->atk_editable_text, position);
}

