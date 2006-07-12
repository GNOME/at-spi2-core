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

/* table.c : implements the Table interface */

#include <config.h>
#include <stdio.h>
#include <bonobo/bonobo-exception.h>
#include <atk/atktable.h>
#include <libspi/accessible.h>
#include <libspi/table.h>


SpiTable *
spi_table_interface_new (AtkObject *obj)
{
  SpiTable *new_table = g_object_new (SPI_TABLE_TYPE, NULL);

  spi_base_construct (SPI_BASE (new_table), G_OBJECT(obj));

  return new_table;
}


static AtkTable *
get_table_from_servant (PortableServer_Servant servant)
{
  SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));

  g_return_val_if_fail (object, NULL);
  g_return_val_if_fail (ATK_IS_OBJECT(object->gobj), NULL);
  return ATK_TABLE (object->gobj);
}


static Accessibility_Accessible
impl__get_caption (PortableServer_Servant servant,
		   CORBA_Environment     *ev)
{
  AtkObject *atk_object;
  AtkTable  *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, CORBA_OBJECT_NIL);

  atk_object = atk_table_get_caption (table);

  return spi_accessible_new_return (atk_object, FALSE, ev);
}


static Accessibility_Accessible
impl__get_summary (PortableServer_Servant servant,
		   CORBA_Environment     *ev)
{
  AtkObject *atk_object;
  AtkTable  *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, CORBA_OBJECT_NIL);

  atk_object = atk_table_get_summary (table);

  return spi_accessible_new_return (atk_object, FALSE, ev);
}


static CORBA_long
impl__get_nRows (PortableServer_Servant servant,
		 CORBA_Environment     *ev)
{
  AtkTable *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, 0);

  return atk_table_get_n_rows (table);
}


static CORBA_long
impl__get_nColumns (PortableServer_Servant servant,
		    CORBA_Environment     *ev)
{
  AtkTable *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, 0);

  return atk_table_get_n_columns (table);
}


static Accessibility_Accessible
impl_getAccessibleAt (PortableServer_Servant servant,
		      const CORBA_long       row,
		      const CORBA_long       column,
		      CORBA_Environment     *ev)
{
  AtkObject *atk_object;
  AtkTable  *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, CORBA_OBJECT_NIL);

  atk_object = atk_table_ref_at (table, row, column);

  return spi_accessible_new_return (atk_object, TRUE, ev);
}


static CORBA_long
impl_getIndexAt (PortableServer_Servant servant,
		 const CORBA_long       row,
		 const CORBA_long       column,
		 CORBA_Environment     *ev)
{
  AtkTable *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, 0);

  return atk_table_get_index_at (table, row, column);
}


static CORBA_long
impl_getRowAtIndex (PortableServer_Servant servant,
		    const CORBA_long       index,
		    CORBA_Environment     *ev)
{
  AtkTable *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, 0);

  return atk_table_get_row_at_index (table, index);
}


static CORBA_long
impl_getColumnAtIndex (PortableServer_Servant servant,
		       const CORBA_long       index,
		       CORBA_Environment     *ev)
{
  AtkTable *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, 0);

  return atk_table_get_column_at_index (table, index);
}


static CORBA_string
impl_getRowDescription (PortableServer_Servant servant,
			const CORBA_long       row,
			CORBA_Environment     *ev)
{
  const char *rv;
  AtkTable   *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, NULL);
  
  rv = atk_table_get_row_description (table, row);

  if (rv)
    {
      return CORBA_string_dup (rv);
    }
  else
    {
      return CORBA_string_dup ("");
    }
}


static CORBA_string
impl_getColumnDescription (PortableServer_Servant servant,
			   const CORBA_long       column,
			   CORBA_Environment     *ev)
{
  const char *rv;
  AtkTable   *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, CORBA_string_dup (""));
  
  rv = atk_table_get_column_description (table, column);

  if (rv)
    {
      return CORBA_string_dup (rv);
    }
  else
    {
      return CORBA_string_dup ("");
    }
}


static CORBA_long
impl_getRowExtentAt (PortableServer_Servant servant,
		     const CORBA_long       row,
		     const CORBA_long       column,
		     CORBA_Environment     *ev)
{
  AtkTable *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, -1);

  return atk_table_get_row_extent_at (table, row, column);
}


static CORBA_long
impl_getColumnExtentAt (PortableServer_Servant servant,
			const CORBA_long       row,
			const CORBA_long       column,
			CORBA_Environment     *ev)
{
  AtkTable *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, -1);

  return atk_table_get_column_extent_at (table, row, column);
}


static Accessibility_Table
impl_getRowHeader (PortableServer_Servant servant,
		   const CORBA_long       row,
		   CORBA_Environment     *ev)
{
  AtkObject *header;
  AtkTable  *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, CORBA_OBJECT_NIL);

  header = atk_table_get_row_header (table, row);

  return spi_accessible_new_return (header, FALSE, ev);
}


static Accessibility_Table
impl_getColumnHeader (PortableServer_Servant servant,
		      const CORBA_long       column,
		      CORBA_Environment     *ev)
{
  AtkObject *header;
  AtkTable  *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, CORBA_OBJECT_NIL);

  header = atk_table_get_column_header (table, column);

  return spi_accessible_new_return (header, FALSE, ev);
}

static CORBA_long
impl__get_nSelectedRows (PortableServer_Servant servant,
		         CORBA_Environment     *ev)
{
  gint *selectedRows = NULL;
  gint retval = 0;
  AtkTable *table = get_table_from_servant (servant);

  bonobo_return_val_if_fail (table != NULL, 0, ev);

  retval = atk_table_get_selected_rows (table, &selectedRows);
  if (selectedRows) g_free (selectedRows);
  return retval;
}


static CORBA_long
impl__get_nSelectedColumns (PortableServer_Servant servant,
			    CORBA_Environment     *ev)
{
  gint *selectedColumns = NULL;
  gint retval = 0;
  AtkTable *table = get_table_from_servant (servant);

  bonobo_return_val_if_fail (table != NULL, 0, ev);

  retval = atk_table_get_selected_columns (table, &selectedColumns);
  if (selectedColumns) g_free (selectedColumns);
  return retval;
}

static Accessibility_LongSeq *
impl_getSelectedRows (PortableServer_Servant servant,
		      CORBA_Environment     *ev)
{
  gint *selectedRows = NULL;
  gint length;
  Accessibility_LongSeq *retval;
  AtkTable *table = get_table_from_servant (servant);

  bonobo_return_val_if_fail (table != NULL, NULL, ev);

  length = atk_table_get_selected_rows (table, &selectedRows);

  bonobo_return_val_if_fail (length >= 0, NULL, ev);

  retval = Accessibility_LongSeq__alloc ();
  retval->_maximum = retval->_length = length;
  retval->_buffer = Accessibility_LongSeq_allocbuf (length);

  while (--length >= 0)
    {
      retval->_buffer[length] = selectedRows[length];
    }

  if (selectedRows) g_free (selectedRows);

  return retval;
}


static Accessibility_LongSeq *
impl_getSelectedColumns (PortableServer_Servant servant,
			 CORBA_Environment     *ev)
{
  gint *selectedColumns = NULL;
  gint length;
  Accessibility_LongSeq *retval;
  AtkTable *table = get_table_from_servant (servant);

  bonobo_return_val_if_fail (table != NULL, NULL, ev);

  length = atk_table_get_selected_columns (table, &selectedColumns);

  bonobo_return_val_if_fail (length >= 0, NULL, ev);

  retval = Accessibility_LongSeq__alloc ();
  retval->_maximum = retval->_length = length;
  retval->_buffer = Accessibility_LongSeq_allocbuf (length);

  while (--length >= 0)
    {
      retval->_buffer[length] = (CORBA_long) selectedColumns[length];
    }

  if (selectedColumns) g_free (selectedColumns);

  return retval;
}


static CORBA_boolean
impl_isRowSelected (PortableServer_Servant servant,
		    const CORBA_long       row,
		    CORBA_Environment     *ev)
{
  AtkTable *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, FALSE);

  return atk_table_is_row_selected (table, row);
}


static CORBA_boolean
impl_isColumnSelected (PortableServer_Servant servant,
		       const CORBA_long       column,
		       CORBA_Environment     *ev)
{
  AtkTable *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, FALSE);

  return atk_table_is_column_selected (table, column);
}

static CORBA_boolean
impl_addRowSelection (PortableServer_Servant servant,
		      const CORBA_long       row,
		      CORBA_Environment     *ev)
{
  AtkTable *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, FALSE);

  return atk_table_add_row_selection (table, row);
}


static CORBA_boolean
impl_addColumnSelection (PortableServer_Servant servant,
			 const CORBA_long       column,
			 CORBA_Environment     *ev)
{
  AtkTable *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, FALSE);

  return atk_table_add_column_selection (table, column);
}


static CORBA_boolean
impl_removeRowSelection (PortableServer_Servant servant,
			 const CORBA_long       row,
			 CORBA_Environment     *ev)
{
  AtkTable *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, FALSE);

  return atk_table_remove_row_selection (table, row);
}


static CORBA_boolean
impl_removeColumnSelection (PortableServer_Servant servant,
			    const CORBA_long       column,
			    CORBA_Environment     *ev)
{
  AtkTable *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, FALSE);

  return atk_table_remove_column_selection (table, column);
}


static CORBA_boolean
impl_getRowColumnExtentsAtIndex (PortableServer_Servant servant,
				 const CORBA_long index,
				 CORBA_long *row,
				 CORBA_long *column,
				 CORBA_long *row_extents,
				 CORBA_long *col_extents,
				 CORBA_boolean *is_selected,
				 CORBA_Environment *ev)
{

  AtkObject *cell;
  AtkRole role;
  AtkTable *table = get_table_from_servant (servant);
  gint intColumn, intRow, intRow_extents, intCol_extents;
  gboolean boolIs_selected;

  g_return_val_if_fail (table != NULL, FALSE);

  intColumn = atk_table_get_column_at_index (table, index);
  intRow = atk_table_get_row_at_index (table, index);
  intRow_extents = atk_table_get_row_extent_at (table, intRow, intColumn);
  intCol_extents = atk_table_get_column_extent_at (table, intRow, intColumn);
  boolIs_selected = atk_table_is_selected (table, intRow, intColumn);

  *column = intColumn;
  *row = intRow;
  *row_extents = intRow_extents;
  *col_extents = intCol_extents;
  *is_selected = boolIs_selected;

  cell = atk_table_ref_at (table, intRow, intColumn);
  role = atk_object_get_role (cell);

  if (role == ATK_ROLE_TABLE_CELL)
    return TRUE;
  
  return FALSE;

}

static CORBA_boolean
impl_isSelected (PortableServer_Servant servant,
		 const CORBA_long       row,
		 const CORBA_long       column,
		 CORBA_Environment     *ev)
{
  AtkTable *table = get_table_from_servant (servant);

  g_return_val_if_fail (table != NULL, FALSE);

  return atk_table_is_selected (table,
				row, column);
}


static void
spi_table_class_init (SpiTableClass *klass)
{
  POA_Accessibility_Table__epv *epv = &klass->epv;

  /* Initialize epv table */

  epv->_get_caption = impl__get_caption;
  epv->_get_summary = impl__get_summary;
  epv->_get_nRows = impl__get_nRows;
  epv->_get_nColumns = impl__get_nColumns;
  epv->_get_nSelectedRows = impl__get_nSelectedRows;
  epv->_get_nSelectedColumns = impl__get_nSelectedColumns;
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
  epv->addRowSelection = impl_addRowSelection;
  epv->addColumnSelection = impl_addColumnSelection;
  epv->removeRowSelection = impl_removeRowSelection;
  epv->removeColumnSelection = impl_removeColumnSelection;
  epv->isSelected = impl_isSelected;
  epv->getRowColumnExtentsAtIndex = impl_getRowColumnExtentsAtIndex;
}

static void
spi_table_init (SpiTable *table)
{
}

BONOBO_TYPE_FUNC_FULL (SpiTable,
		       Accessibility_Table,
		       SPI_TYPE_BASE,
		       spi_table)
