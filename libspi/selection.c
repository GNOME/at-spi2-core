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
 * This pulls the definition of the selection bonobo object
 */
#include "selection.h"

/*
 * Static function declarations
 */

static void
selection_class_init (SelectionClass *klass);
static void
selection_init (Selection *selection);
static void
selection_finalize (GObject *obj);
static CORBA_long
impl__get_nSelectedChildren (PortableServer_Servant _servant,
			     CORBA_Environment * ev);
static Accessibility_Accessible
impl_getSelectedChild (PortableServer_Servant _servant,
		       const CORBA_long selectedChildIndex,
		       CORBA_Environment * ev);
static CORBA_boolean
impl_selectChild (PortableServer_Servant _servant,
		  const CORBA_long childIndex,
		  CORBA_Environment * ev);
static CORBA_boolean
impl_deselectSelectedChild (PortableServer_Servant _servant,
			    const CORBA_long selectedChildIndex,
			    CORBA_Environment * ev);
static CORBA_boolean
impl_isChildSelected (PortableServer_Servant _servant,
		      const CORBA_long childIndex,
		      CORBA_Environment * ev);
static void 
impl_selectAll (PortableServer_Servant _servant,
		CORBA_Environment * ev);
static void 
impl_clearSelection (PortableServer_Servant _servant,
		     CORBA_Environment * ev);


static GObjectClass *parent_class;

GType
selection_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo = {
      sizeof (SelectionClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) selection_class_init,
      (GClassFinalizeFunc) NULL,
      NULL, /* class data */
      sizeof (Selection),
      0, /* n preallocs */
      (GInstanceInitFunc) selection_init,
                        NULL /* value table */
    };

    /*
     * Bonobo_type_unique auto-generates a load of
     * CORBA structures for us. All derived types must
     * use bonobo_type_unique.
     */
    type = bonobo_type_unique (
			       BONOBO_OBJECT_TYPE,
			       POA_Accessibility_Selection__init,
			       NULL,
			       G_STRUCT_OFFSET (SelectionClass, epv),
			       &tinfo,
			       "AccessibleSelection");
  }

  return type;
}

static void
selection_class_init (SelectionClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_Selection__epv *epv = &klass->epv;
  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = selection_finalize;


  /* Initialize epv table */

  epv->_get_nSelectedChildren = impl__get_nSelectedChildren;
  epv->getSelectedChild = impl_getSelectedChild;
  epv->selectChild = impl_selectChild;
  epv->deselectSelectedChild = impl_deselectSelectedChild;
  epv->isChildSelected = impl_isChildSelected;
  epv->selectAll = impl_selectAll;
  epv->clearSelection = impl_clearSelection;
}

static void
selection_init (Selection *selection)
{
}

static void
selection_finalize (GObject *obj)
{
  Selection *selection = SELECTION (obj);
  g_object_unref (selection->atko);
  selection->atko = NULL;
  parent_class->finalize (obj);
}

Selection *
selection_interface_new (AtkObject *obj)
{
  Selection *new_selection = 
    SELECTION(g_object_new (SELECTION_TYPE, NULL));
  new_selection->atko = obj;
  g_object_ref (obj);
  return new_selection;
}



static CORBA_long
impl__get_nSelectedChildren (PortableServer_Servant _servant,
			     CORBA_Environment * ev)
{
  Selection *selection = SELECTION (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_selection_get_selection_count (ATK_SELECTION(selection->atko));
} 




static Accessibility_Accessible
impl_getSelectedChild (PortableServer_Servant _servant,
		       const CORBA_long selectedChildIndex,
		       CORBA_Environment * ev)
{
  Selection *selection = SELECTION (bonobo_object_from_servant (_servant));
  AtkObject *atk_object;
  Accessibility_Accessible rv;

  atk_object = atk_selection_ref_selection (ATK_SELECTION(selection->atko), (gint) selectedChildIndex);
  rv = bonobo_object_corba_objref (BONOBO_OBJECT(accessible_new(atk_object)));
  return rv;
}



static CORBA_boolean
impl_selectChild (PortableServer_Servant _servant,
		  const CORBA_long childIndex,
		  CORBA_Environment * ev)
{
  Selection *selection = SELECTION (bonobo_object_from_servant (_servant));
  return (CORBA_boolean)
    atk_selection_add_selection (ATK_SELECTION(selection->atko), (gint) childIndex);
}




static CORBA_boolean
impl_deselectSelectedChild (PortableServer_Servant _servant,
			    const CORBA_long selectedChildIndex,
			    CORBA_Environment * ev)
{
  Selection *selection = SELECTION (bonobo_object_from_servant (_servant));
  return (CORBA_boolean)
    atk_selection_remove_selection (ATK_SELECTION(selection->atko), (gint) selectedChildIndex);
}



static CORBA_boolean
impl_isChildSelected (PortableServer_Servant _servant,
		      const CORBA_long childIndex,
		      CORBA_Environment * ev)
{
  Selection *selection = SELECTION (bonobo_object_from_servant (_servant));
  return (CORBA_boolean)
    atk_selection_is_child_selected (ATK_SELECTION(selection->atko), (gint) childIndex);
}



static void 
impl_selectAll (PortableServer_Servant _servant,
		CORBA_Environment * ev)
{
  Selection *selection = SELECTION (bonobo_object_from_servant (_servant));
  atk_selection_select_all_selection (ATK_SELECTION(selection->atko));
}



static void 
impl_clearSelection (PortableServer_Servant _servant,
		     CORBA_Environment * ev)
{
  Selection *selection = SELECTION (bonobo_object_from_servant (_servant));
  atk_selection_clear_selection (ATK_SELECTION(selection->atko));
}

