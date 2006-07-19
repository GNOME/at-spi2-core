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

/* selection.c : implements the Selection interface */

#include <config.h>
#include <stdio.h>
#include <libspi/accessible.h>
#include <libspi/selection.h>


SpiSelection *
spi_selection_interface_new (AtkObject *obj)
{
  SpiSelection *new_selection = g_object_new (SPI_SELECTION_TYPE, NULL);

  spi_base_construct (SPI_BASE (new_selection), G_OBJECT(obj));

  return new_selection;
}


static AtkSelection *
get_selection_from_servant (PortableServer_Servant servant)
{
  SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));

  g_return_val_if_fail (object, NULL);
  g_return_val_if_fail (ATK_IS_OBJECT(object->gobj), NULL);
  return ATK_SELECTION (object->gobj);
}


static CORBA_long
impl__get_nSelectedChildren (PortableServer_Servant servant,
			     CORBA_Environment     *ev)
{
  AtkSelection *selection = get_selection_from_servant (servant);

  g_return_val_if_fail (selection != NULL, 0);

  return atk_selection_get_selection_count (selection);
}


static Accessibility_Accessible
impl_getSelectedChild (PortableServer_Servant servant,
		       const CORBA_long       selectedChildIndex,
		       CORBA_Environment     *ev)
{
  AtkObject    *atk_object;
  AtkSelection *selection = get_selection_from_servant (servant);

  g_return_val_if_fail (selection != NULL, CORBA_OBJECT_NIL);

#ifdef SPI_DEBUG
  fprintf (stderr, "calling impl_getSelectedChild\n");
#endif

  atk_object = atk_selection_ref_selection (selection,
					    selectedChildIndex);

  g_return_val_if_fail (ATK_IS_OBJECT (atk_object), CORBA_OBJECT_NIL);

#ifdef SPI_DEBUG
  fprintf (stderr, "child type is %s\n",
	   g_type_name (G_OBJECT_TYPE (atk_object)));
#endif

  return spi_accessible_new_return (atk_object, TRUE, ev);
}


static CORBA_boolean
impl_selectChild (PortableServer_Servant servant,
		  const CORBA_long       childIndex,
		  CORBA_Environment     *ev)
{
  AtkSelection *selection = get_selection_from_servant (servant);

  g_return_val_if_fail (selection != NULL, FALSE);

  return atk_selection_add_selection (selection, childIndex);
}


static CORBA_boolean
impl_deselectSelectedChild (PortableServer_Servant servant,
			    const CORBA_long       selectedChildIndex,
			    CORBA_Environment     *ev)
{
  AtkSelection *selection = get_selection_from_servant (servant);

  g_return_val_if_fail (selection != NULL, FALSE);

  return atk_selection_remove_selection (selection, selectedChildIndex);
}



static CORBA_boolean
impl_deselectChild (PortableServer_Servant servant,
		    const CORBA_long       selectedChildIndex,
		    CORBA_Environment     *ev)
{
  AtkSelection *selection = get_selection_from_servant (servant);
  gint i, nselected;

  g_return_val_if_fail (selection != NULL, FALSE);
  nselected = atk_selection_get_selection_count (selection);
  for (i=0; i<nselected; ++i)
  {
      AtkObject *selected_obj = atk_selection_ref_selection (selection, i);
      if (atk_object_get_index_in_parent (selected_obj) == selectedChildIndex)
      {
	  g_object_unref (G_OBJECT (selected_obj));
	  return atk_selection_remove_selection (selection, i);
      }
      g_object_unref (G_OBJECT (selected_obj));
  }
  return FALSE;
}


static CORBA_boolean
impl_isChildSelected (PortableServer_Servant servant,
		      const CORBA_long       childIndex,
		      CORBA_Environment     *ev)
{
  AtkSelection *selection = get_selection_from_servant (servant);

  g_return_val_if_fail (selection != NULL, FALSE);

  return atk_selection_is_child_selected (selection, childIndex);
}


static CORBA_boolean 
impl_selectAll (PortableServer_Servant servant,
		CORBA_Environment     *ev)
{
  AtkSelection *selection = get_selection_from_servant (servant);

  g_return_val_if_fail (selection != NULL, FALSE);

  return atk_selection_select_all_selection (selection);

}


static CORBA_boolean
impl_clearSelection (PortableServer_Servant servant,
		     CORBA_Environment     *ev)
{
  AtkSelection *selection = get_selection_from_servant (servant);

  g_return_val_if_fail (selection != NULL, FALSE);

  return atk_selection_clear_selection (selection);
}


static void
spi_selection_class_init (SpiSelectionClass *klass)
{
  POA_Accessibility_Selection__epv *epv = &klass->epv;

  /* Initialize epv table */

  epv->_get_nSelectedChildren = impl__get_nSelectedChildren;
  epv->getSelectedChild       = impl_getSelectedChild;
  epv->selectChild            = impl_selectChild;
  epv->deselectSelectedChild  = impl_deselectSelectedChild;
  epv->deselectChild          = impl_deselectChild;
  epv->isChildSelected        = impl_isChildSelected;
  epv->selectAll              = impl_selectAll;
  epv->clearSelection         = impl_clearSelection;
}


static void
spi_selection_init (SpiSelection *selection)
{
}


BONOBO_TYPE_FUNC_FULL (SpiSelection,
		       Accessibility_Selection,
		       SPI_TYPE_BASE,
		       spi_selection)
