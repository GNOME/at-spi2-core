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
 * This pulls the CORBA definitions for the "Accessibility::SpiAccessible" server
 */
#include <libspi/Accessibility.h>

/*
 * This pulls the definition of the SpiTable bonobo object
 */
#include "table.h"

/*
 * Static function declarations
 */

static void
spi_table_class_init (SpiTableClass *klass);
static void
spi_table_init (SpiTable *table);
static void
spi_table_finalize (GObject *obj);
static Accessibility_SpiAccessible
impl__get_caption (PortableServer_Servant _servant,
		   CORBA_Environment * ev);
static Accessibility_SpiAccessible
impl__get_summary (PortableServer_Servant _servant,
		   CORBA_Environment * ev);
static CORBA_long
impl__get_nRows (PortableServer_Servant _servant,
		 CORBA_Environment * ev);
static CORBA_long
impl__get_nRows (PortableServer_Servant _servant,
		 CORBA_Environment * ev);
static CORBA_long
impl__get_nColumns (PortableServer_Servant _servant,
		    CORBA_Environment * ev);
static Accessibility_SpiAccessible
impl_getAccessibleAt (PortableServer_Servant _servant,
		      const CORBA_long row,
		      const CORBA_long column,
		      CORBA_Environment * ev);
static CORBA_long
impl_getIndexAt (PortableServer_Servant _servant,
		 const CORBA_long row, const CORBA_long column,
		 CORBA_Environment * ev);
static CORBA_long
impl_getRowAtIndex (PortableServer_Servant _servant,
		    const CORBA_long index,
		    CORBA_Environment * ev);
static CORBA_long
impl_getColumnAtIndex (PortableServer_Servant _servant,
		       const CORBA_long index,
		       CORBA_Environment * ev);
static CORBA_string
impl_getRowDescription (PortableServer_Servant _servant,
			const CORBA_long row,
			CORBA_Environment * ev);
static CORBA_string
impl_getColumnDescription (PortableServer_Servant _servant,
			   const CORBA_long column,
			   CORBA_Environment * ev);
static CORBA_long
impl_getRowExtentAt (PortableServer_Servant _servant,
		     const CORBA_long row,
		     const CORBA_long column,
		     CORBA_Environment * ev);
static CORBA_long
impl_getColumnExtentAt (PortableServer_Servant _servant,
			const CORBA_long row,
			const CORBA_long column,
			CORBA_Environment * ev);
static Accessibility_SpiTable
impl_getRowHeader (PortableServer_Servant _servant,
		   const CORBA_long row,
		   CORBA_Environment * ev);
static        Accessibility_SpiTable
impl_getColumnHeader (PortableServer_Servant _servant,
		      const CORBA_long column,
		      CORBA_Environment * ev);
static Accessibility_LongSeq *
impl_getSelectedRows (PortableServer_Servant _servant,
		      CORBA_Environment * ev);
static Accessibility_LongSeq *
impl_getSelectedColumns (PortableServer_Servant _servant,
			 CORBA_Environment * ev);
static CORBA_boolean
impl_isRowSelected (PortableServer_Servant _servant,
		    const CORBA_long row,
		    CORBA_Environment * ev);
static CORBA_boolean
impl_isColumnSelected (PortableServer_Servant _servant,
		       const CORBA_long column,
		       CORBA_Environment * ev);
static CORBA_boolean
impl_isSelected (PortableServer_Servant _servant,
		 const CORBA_long row,
		 const CORBA_long column,
		 CORBA_Environment * ev);


static GObjectClass *parent_class;

GType
spi_table_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo = {
      sizeof (SpiTableClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) spi_table_class_init,
      (GClassFinalizeFunc) NULL,
      NULL, /* class data */
      sizeof (SpiTable),
      0, /* n preallocs */
      (GInstanceInitFunc) spi_table_init,
                        NULL /* value table */
    };

    /*
     * Bonobo_type_unique auto-generates a load of
     * CORBA structures for us. All derived types must
     * use bonobo_type_unique.
     */
    type = bonobo_type_unique (
			       BONOBO_OBJECT_TYPE,
			       POA_Accessibility_SpiTable__init,
			       NULL,
			       G_STRUCT_OFFSET (SpiTableClass, epv),
			       &tinfo,
			       "SpiAccessibleTable");
  }

  return type;
}

static void
spi_table_class_init (SpiTableClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_SpiTable__epv *epv = &klass->epv;
  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = spi_table_finalize;


  /* Initialize epv table */

  epv->_get_caption = impl__get_caption;
  epv->_get_summary = impl__get_summary;
  epv->_get_nRows = impl__get_nRows;
  epv->_get_nColumns = impl__get_nColumns;
  epv->getAccessibleAt = impl_getAccessibleAt;
  epv->getIndexAt = impl_getIndexAt;
  epv->getRowAtIndex = impl_getRowAtIndex;
  epv->getColumnAtIndex = impl_getColumnAtIndex;
  epv->getRowDescription = impl_getRowDescription;
  epv->getColumnDescription = impl_getColumnDescription;
  epv->getRowExtentAt = impl_getRowExtentAt;
  epv->getColumnExtentAt = impl_getColumnExtentAt;
  epv->getRowHeader = impl_getRowHeader;
  epv->getColumnHeader = impl_getColumnHeader;
  epv->getSelectedRows = impl_getSelectedRows;
  epv->getSelectedColumns = impl_getSelectedColumns;
  epv->isRowSelected = impl_isRowSelected;
  epv->isColumnSelected = impl_isColumnSelected;
  epv->isSelected = impl_isSelected;
}

static void
spi_table_init (SpiTable *table)
{
}

static void
spi_table_finalize (GObject *obj)
{
  SpiTable *table = SPI_TABLE (obj);
  g_object_unref (table->atko);
  table->atko = NULL;  
  parent_class->finalize (obj);
}

SpiTable *
spi_table_interface_new (AtkObject *obj)
{
  SpiTable *new_table =
    SPI_TABLE(g_object_new (SPI_TABLE_TYPE, NULL));
  new_table->atko = obj;
  g_object_ref (obj);
  return new_table;
}



static Accessibility_SpiAccessible
impl__get_caption (PortableServer_Servant _servant,
		   CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  AtkObject *atk_object;
  Accessibility_SpiAccessible rv;

  atk_object = atk_table_get_caption (ATK_TABLE(table-> atko));
  rv = bonobo_object_corba_objref (BONOBO_OBJECT(spi_accessible_new(atk_object)));
  return CORBA_Object_duplicate (rv, ev);
}



static Accessibility_SpiAccessible
impl__get_summary (PortableServer_Servant _servant,
		   CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  AtkObject *atk_object;
  Accessibility_SpiAccessible rv;

  atk_object = atk_table_get_summary (ATK_TABLE(table->atko));
  rv = bonobo_object_corba_objref (BONOBO_OBJECT(spi_accessible_new(atk_object)));
  return CORBA_Object_duplicate (rv, ev);
}



static CORBA_long
impl__get_nRows (PortableServer_Servant _servant,
		 CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_table_get_n_rows (ATK_TABLE(table->atko) );
}



static CORBA_long
impl__get_nColumns (PortableServer_Servant _servant,
		    CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_table_get_n_columns (ATK_TABLE(table->atko));
}



static Accessibility_SpiAccessible
impl_getAccessibleAt (PortableServer_Servant _servant,
		      const CORBA_long row,
		      const CORBA_long column,
		      CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  AtkObject *atk_object;
  Accessibility_SpiAccessible rv;

  atk_object = atk_table_ref_at (ATK_TABLE(table->atko),
					     (gint) row, (gint) column);
  rv = bonobo_object_corba_objref (BONOBO_OBJECT(spi_accessible_new(atk_object)));
  return CORBA_Object_duplicate (rv, ev);
}



static CORBA_long
impl_getIndexAt (PortableServer_Servant _servant,
		 const CORBA_long row, const CORBA_long column,
		 CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_table_get_index_at (ATK_TABLE(table->atko),
			    (gint) row, (gint) column);
}



static CORBA_long
impl_getRowAtIndex (PortableServer_Servant _servant,
		    const CORBA_long index,
		    CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_table_get_row_at_index (ATK_TABLE(table->atko), (gint) index);
}



static CORBA_long
impl_getColumnAtIndex (PortableServer_Servant _servant,
		       const CORBA_long index,
		       CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_table_get_column_at_index (ATK_TABLE(table->atko), (gint) index);
}



static CORBA_string
impl_getRowDescription (PortableServer_Servant _servant,
			const CORBA_long row,
			CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  CORBA_char *rv;

  rv = atk_table_get_row_description (ATK_TABLE(table->atko), (gint) row);
  if (rv)
    return CORBA_string_dup (rv);
  else
    return CORBA_string_dup ("");
}



static CORBA_string
impl_getColumnDescription (PortableServer_Servant _servant,
			   const CORBA_long column,
			   CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  CORBA_char *rv;

  rv = atk_table_get_column_description (ATK_TABLE(table->atko), (gint) column);
  if (rv)
    return CORBA_string_dup (rv);
  else
    return CORBA_string_dup ("");
}



static CORBA_long
impl_getRowExtentAt (PortableServer_Servant _servant,
		     const CORBA_long row,
		     const CORBA_long column,
		     CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_table_get_row_extent_at (ATK_TABLE(table->atko),
				 (gint) row, (gint) column);
}



static CORBA_long
impl_getColumnExtentAt (PortableServer_Servant _servant,
			const CORBA_long row,
			const CORBA_long column,
			CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_table_get_column_extent_at (ATK_TABLE(table->atko),
				 (gint) row, (gint) column);
}



static Accessibility_SpiTable
impl_getRowHeader (PortableServer_Servant _servant,
		   const CORBA_long row,
		   CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  AtkObject *header;
  Accessibility_SpiTable rv;

  header = atk_table_get_row_header (ATK_TABLE(table->atko), (gint) row);
  rv = bonobo_object_corba_objref (BONOBO_OBJECT(spi_accessible_new(header)));
  return CORBA_Object_duplicate (rv, ev);
}



static        Accessibility_SpiTable
impl_getColumnHeader (PortableServer_Servant _servant,
		      const CORBA_long column,
		      CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  AtkObject *header;
  Accessibility_SpiTable rv;

  header = atk_table_get_column_header (ATK_TABLE(table->atko), (gint) column);
  rv = bonobo_object_corba_objref (BONOBO_OBJECT(spi_accessible_new(header)));
  return CORBA_Object_duplicate (rv, ev);
}



static Accessibility_LongSeq *
impl_getSelectedRows (PortableServer_Servant _servant,
		      CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  gint *selectedRows;
  gint length;
  Accessibility_LongSeq *retval;

  length = atk_table_get_selected_rows (ATK_TABLE(table->atko), &selectedRows);

  g_return_val_if_fail (length, NULL);
  retval = Accessibility_LongSeq__alloc ();
  retval->_maximum = retval->_length = (CORBA_long) length;
  retval->_buffer = Accessibility_LongSeq_allocbuf (length);

  while (--length)
    retval->_buffer[length] = (CORBA_long) selectedRows[length];
  g_free ((gpointer) selectedRows);
  return retval;
}



static Accessibility_LongSeq *
impl_getSelectedColumns (PortableServer_Servant _servant,
			 CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  gint *selectedColumns;
  gint length;
  Accessibility_LongSeq *retval;

  length = atk_table_get_selected_columns (ATK_TABLE(table->atko), &selectedColumns);

  g_return_val_if_fail (length, NULL);

  retval = Accessibility_LongSeq__alloc ();
  retval->_maximum = retval->_length = (CORBA_long) length;
  retval->_buffer = Accessibility_LongSeq_allocbuf (length);

  while (--length)
    retval->_buffer[length] = (CORBA_long) selectedColumns[length];
  g_free ((gpointer) selectedColumns);
  return retval;
}



static CORBA_boolean
impl_isRowSelected (PortableServer_Servant _servant,
		    const CORBA_long row,
		    CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_boolean)
    atk_table_is_row_selected (ATK_TABLE(table->atko), (gint) row);
}



static CORBA_boolean
impl_isColumnSelected (PortableServer_Servant _servant,
		       const CORBA_long column,
		       CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_boolean)
    atk_table_is_column_selected (ATK_TABLE(table->atko), (gint) column);
}



static CORBA_boolean
impl_isSelected (PortableServer_Servant _servant,
		 const CORBA_long row,
		 const CORBA_long column,
		 CORBA_Environment * ev)
{
  SpiTable *table = SPI_TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_boolean)
    atk_table_is_selected (ATK_TABLE(table->atko),
			   (gint) row, (gint) column);
}



