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

/* selection.c : implements the Selection interface */

#include <config.h>
#include <stdio.h>
#include <libspi/accessible.h>
#include <libspi/selection.h>

/* Static function declarations */

static void
spi_selection_class_init (SpiSelectionClass *klass);
static void
spi_selection_init (SpiSelection *selection);
static void
spi_selection_finalize (GObject *obj);
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
spi_selection_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo = {
      sizeof (SpiSelectionClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) spi_selection_class_init,
      (GClassFinalizeFunc) NULL,
      NULL, /* class data */
      sizeof (SpiSelection),
      0, /* n preallocs */
      (GInstanceInitFunc) spi_selection_init,
                        NULL /* value table */
    };

    /*
     * Bonobo_type_unique auto-generates a load of
     * CORBA structures for us. All derived types must
     * use bonobo_type_unique.
     */
    type = bonobo_type_unique (
			       BONOBO_TYPE_OBJECT,
			       POA_Accessibility_Selection__init,
			       NULL,
			       G_STRUCT_OFFSET (SpiSelectionClass, epv),
			       &tinfo,
			       "SpiAccessibleSelection");
  }

  return type;
}

static void
spi_selection_class_init (SpiSelectionClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_Selection__epv *epv = &klass->epv;
  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = spi_selection_finalize;


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
spi_selection_init (SpiSelection *selection)
{
}

static void
spi_selection_finalize (GObject *obj)
{
  SpiSelection *selection = SPI_SELECTION (obj);
  g_object_unref (selection->atko);
  selection->atko = NULL;
  parent_class->finalize (obj);
}

SpiSelection *
spi_selection_interface_new (AtkObject *obj)
{
  SpiSelection *new_selection = 
    SPI_SELECTION(g_object_new (SPI_SELECTION_TYPE, NULL));
  new_selection->atko = obj;
  g_object_ref (obj);
  return new_selection;
}



static CORBA_long
impl__get_nSelectedChildren (PortableServer_Servant _servant,
			     CORBA_Environment * ev)
{
  BonoboObject *obj = bonobo_object_from_servant (_servant);
  SpiSelection *selection;
#ifdef SPI_DEBUG
  fprintf (stderr, "calling impl__get_nSelectedChildren\n");
#endif
  g_return_val_if_fail (IS_SPI_SELECTION (obj), 0);
  selection = SPI_SELECTION (obj);
  g_return_val_if_fail (ATK_IS_SELECTION (selection->atko), 0);
  return (CORBA_long)
    atk_selection_get_selection_count (ATK_SELECTION(selection->atko));
}


static Accessibility_Accessible
impl_getSelectedChild (PortableServer_Servant _servant,
		       const CORBA_long selectedChildIndex,
		       CORBA_Environment * ev)
{
  BonoboObject *obj = bonobo_object_from_servant (_servant);
  SpiSelection
	  *selection;
  AtkObject *atk_object;
  Accessibility_Accessible rv;
#ifdef SPI_DEBUG
  fprintf (stderr, "calling impl_getSelectedChild\n");
#endif
  g_return_val_if_fail (IS_SPI_SELECTION (obj), 0);
  selection = SPI_SELECTION (obj);
  g_return_val_if_fail (ATK_IS_SELECTION (selection->atko), 0);

  atk_object = atk_selection_ref_selection (ATK_SELECTION (selection->atko),
					    (gint) selectedChildIndex);
  g_return_val_if_fail (ATK_IS_OBJECT (atk_object), NULL);
#ifdef SPI_DEBUG
  fprintf (stderr, "child type is %s\n", g_type_name (G_OBJECT_TYPE (atk_object)));
#endif
  rv = bonobo_object_corba_objref (bonobo_object (spi_accessible_new (atk_object)));
  g_object_unref (atk_object);
  return CORBA_Object_duplicate (rv, ev);
}



static CORBA_boolean
impl_selectChild (PortableServer_Servant _servant,
		  const CORBA_long childIndex,
		  CORBA_Environment * ev)
{
  SpiSelection *selection = SPI_SELECTION (bonobo_object_from_servant (_servant));
  return (CORBA_boolean)
    atk_selection_add_selection (ATK_SELECTION(selection->atko), (gint) childIndex);
}




static CORBA_boolean
impl_deselectSelectedChild (PortableServer_Servant _servant,
			    const CORBA_long selectedChildIndex,
			    CORBA_Environment * ev)
{
  SpiSelection *selection = SPI_SELECTION (bonobo_object_from_servant (_servant));
  return (CORBA_boolean)
    atk_selection_remove_selection (ATK_SELECTION(selection->atko), (gint) selectedChildIndex);
}



static CORBA_boolean
impl_isChildSelected (PortableServer_Servant _servant,
		      const CORBA_long childIndex,
		      CORBA_Environment * ev)
{
  SpiSelection *selection = SPI_SELECTION (bonobo_object_from_servant (_servant));
  return (CORBA_boolean)
    atk_selection_is_child_selected (ATK_SELECTION(selection->atko), (gint) childIndex);
}



static void 
impl_selectAll (PortableServer_Servant _servant,
		CORBA_Environment * ev)
{
  SpiSelection *selection = SPI_SELECTION (bonobo_object_from_servant (_servant));
  atk_selection_select_all_selection (ATK_SELECTION(selection->atko));
}



static void 
impl_clearSelection (PortableServer_Servant _servant,
		     CORBA_Environment * ev)
{
  SpiSelection *selection = SPI_SELECTION (bonobo_object_from_servant (_servant));
  atk_selection_clear_selection (ATK_SELECTION(selection->atko));
}

