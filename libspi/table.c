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
 * This pulls the definition of the Table bonobo object
 */
#include "table.h"

/*
 * Static function declarations
 */

static void
table_class_init (TableClass *klass);
static void
table_init (Table *table);
static void
table_finalize (GObject *obj);
static Accessibility_Accessible
impl__get_caption (PortableServer_Servant _servant,
		   CORBA_Environment * ev);
static Accessibility_Accessible
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
static Accessibility_Accessible
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
static Accessibility_Table
impl_getRowHeader (PortableServer_Servant _servant,
		   const CORBA_long row,
		   CORBA_Environment * ev);
static        Accessibility_Table
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
table_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo = {
      sizeof (TableClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) table_class_init,
      (GClassFinalizeFunc) NULL,
      NULL, /* class data */
      sizeof (Table),
      0, /* n preallocs */
      (GInstanceInitFunc) table_init,
                        NULL /* value table */
    };

    /*
     * Bonobo_type_unique auto-generates a load of
     * CORBA structures for us. All derived types must
     * use bonobo_type_unique.
     */
    type = bonobo_type_unique (
			       BONOBO_OBJECT_TYPE,
			       POA_Accessibility_Table__init,
			       NULL,
			       G_STRUCT_OFFSET (TableClass, epv),
			       &tinfo,
			       "AccessibleTable");
  }

  return type;
}

static void
table_class_init (TableClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_Table__epv *epv = &klass->epv;
  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = table_finalize;


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
table_init (Table *table)
{
}

static void
table_finalize (GObject *obj)
{
  Table *table = TABLE (obj);
  table->atk_table = NULL;
  parent_class->finalize (obj);
}

Table *
table_new (AtkTable *table)
{
  Table *new_table =
    TABLE(g_object_new (TABLE_TYPE, NULL));
  new_table->atk_table = table;
  return new_table;
}



static Accessibility_Accessible
impl__get_caption (PortableServer_Servant _servant,
		   CORBA_Environment * ev)
{
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  AtkObject *atk_object;
  Accessibility_Accessible rv;

  atk_object = g_object_new (ATK_TYPE_OBJECT, NULL);
  atk_object_set_name (atk_object, atk_table_get_caption (table->atk_table));
  rv = bonobo_object_corba_objref (BONOBO_OBJECT(accessible_new(atk_object)));
  return rv;
}



static Accessibility_Accessible
impl__get_summary (PortableServer_Servant _servant,
		   CORBA_Environment * ev)
{
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  AtkObject *atk_object;
  Accessibility_Accessible rv;

  atk_object = atk_table_get_summary (table->atk_table);
  rv = bonobo_object_corba_objref (BONOBO_OBJECT(accessible_new(atk_object)));
  return rv;
}



static CORBA_long
impl__get_nRows (PortableServer_Servant _servant,
		 CORBA_Environment * ev)
{
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_table_get_n_rows (table->atk_table);
}



static CORBA_long
impl__get_nColumns (PortableServer_Servant _servant,
		    CORBA_Environment * ev)
{
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_table_get_n_columns (table->atk_table);
}



static Accessibility_Accessible
impl_getAccessibleAt (PortableServer_Servant _servant,
		      const CORBA_long row,
		      const CORBA_long column,
		      CORBA_Environment * ev)
{
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  AtkObject *atk_object;
  Accessibility_Accessible rv;

  atk_object = atk_table_ref_at (table->atk_table,
					     (gint) row, (gint) column);
  rv = bonobo_object_corba_objref (BONOBO_OBJECT(accessible_new(atk_object)));
  return rv;
}



static CORBA_long
impl_getIndexAt (PortableServer_Servant _servant,
		 const CORBA_long row, const CORBA_long column,
		 CORBA_Environment * ev)
{
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_table_get_index_at (table->atk_table,
			    (gint) row, (gint) column);
}



static CORBA_long
impl_getRowAtIndex (PortableServer_Servant _servant,
		    const CORBA_long index,
		    CORBA_Environment * ev)
{
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_table_get_row_at_index (table->atk_table, (gint) index);
}



static CORBA_long
impl_getColumnAtIndex (PortableServer_Servant _servant,
		       const CORBA_long index,
		       CORBA_Environment * ev)
{
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_table_get_column_at_index (table->atk_table, (gint) index);
}



static CORBA_string
impl_getRowDescription (PortableServer_Servant _servant,
			const CORBA_long row,
			CORBA_Environment * ev)
{
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  return CORBA_string_dup (
			   atk_table_get_row_description (table->atk_table, (gint) row));
}



static CORBA_string
impl_getColumnDescription (PortableServer_Servant _servant,
			   const CORBA_long column,
			   CORBA_Environment * ev)
{
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  return CORBA_string_dup (
			   atk_table_get_column_description (table->atk_table, (gint) column));
}



static CORBA_long
impl_getRowExtentAt (PortableServer_Servant _servant,
		     const CORBA_long row,
		     const CORBA_long column,
		     CORBA_Environment * ev)
{
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_table_get_row_extent_at (table->atk_table,
				 (gint) row, (gint) column);
}



static CORBA_long
impl_getColumnExtentAt (PortableServer_Servant _servant,
			const CORBA_long row,
			const CORBA_long column,
			CORBA_Environment * ev)
{
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_long)
    atk_table_get_column_extent_at (table->atk_table,
				 (gint) row, (gint) column);
}



static Accessibility_Table
impl_getRowHeader (PortableServer_Servant _servant,
		   const CORBA_long row,
		   CORBA_Environment * ev)
{
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  AtkObject *header;
  Accessibility_Table rv;

  header = atk_table_get_row_header (table->atk_table, (gint) row);
  rv = bonobo_object_corba_objref (BONOBO_OBJECT(accessible_new(header)));
  return rv;
}



static        Accessibility_Table
impl_getColumnHeader (PortableServer_Servant _servant,
		      const CORBA_long column,
		      CORBA_Environment * ev)
{
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  AtkObject *header;
  Accessibility_Table rv;

  header = atk_table_get_column_header (table->atk_table, (gint) column);
  rv = bonobo_object_corba_objref (BONOBO_OBJECT(accessible_new(header)));
  return rv;
}



static Accessibility_LongSeq *
impl_getSelectedRows (PortableServer_Servant _servant,
		      CORBA_Environment * ev)
{
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  gint *selectedRows;
  gint length;
  Accessibility_LongSeq *retval;

  length = atk_table_get_selected_rows (table->atk_table, &selectedRows);

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
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  gint *selectedColumns;
  gint length;
  Accessibility_LongSeq *retval;

  length = atk_table_get_selected_columns (table->atk_table, &selectedColumns);

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
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_boolean)
    atk_table_is_row_selected (table->atk_table, (gint) row);
}



static CORBA_boolean
impl_isColumnSelected (PortableServer_Servant _servant,
		       const CORBA_long column,
		       CORBA_Environment * ev)
{
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_boolean)
    atk_table_is_column_selected (table->atk_table, (gint) column);
}



static CORBA_boolean
impl_isSelected (PortableServer_Servant _servant,
		 const CORBA_long row,
		 const CORBA_long column,
		 CORBA_Environment * ev)
{
  Table *table = TABLE (bonobo_object_from_servant (_servant));
  return (CORBA_boolean)
    atk_table_is_selected (table->atk_table,
			   (gint) row, (gint) column);
}



