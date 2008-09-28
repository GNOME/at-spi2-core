#Copyright (C) 2008 Codethink Ltd

#This library is free software; you can redistribute it and/or
#modify it under the terms of the GNU Lesser General Public
#License version 2 as published by the Free Software Foundation.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#You should have received a copy of the GNU Lesser General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

import interfaces
from base import BaseProxy
from factory import add_accessible_class

__all__ = [
	   "Table",
	  ]

#------------------------------------------------------------------------------

class Table(BaseProxy):
    """
    An interface used by containers whose contained data is arranged
    in a "tabular" (i.e. row-column) fashion. Tables may resemble
    a two-dimensional grid, as in a spreadsheet, or may feature objects
    which span multiple rows and/or columns, but whose bounds are
    aligned on a row/column matrix. Thus, the Table interface may
    be used to represent "spreadsheets" as well as "frames".
    Objects within tables are children of the Table instance, and
    they may be referenced either via a child index or via a row/column
    pair. Their role may be ROLE_TABLE_CELL, but table 'cells' may
    have other roles as well. These 'cells' may implement other interfaces,
    such as Text, Action, Image, and Component, and should do so
    as appropriate to their onscreen representation and/or behavior.
    """
    
    def addColumnSelection(self, *args, **kwargs):
        """
        Select the specified column, adding it to the current column
        selection, if the table's selection model permits it.
        @param : column
        @return True if the specified column was successfully selected,
        False if not.
        """
        func = self.get_dbus_method("addColumnSelection")
        return func(*args, **kwargs)
    
    def addRowSelection(self, *args, **kwargs):
        """
        Select the specified row, adding it to the current row selection,
        if the table's selection model permits it.
        @param : row
        @return True if the specified row was successfully selected,
        False if not.
        """
        func = self.get_dbus_method("addRowSelection")
        return func(*args, **kwargs)
    
    def getAccessibleAt(self, *args, **kwargs):
        """
        Get the table cell at the specified row and column indices. 
        @param : row
        the specified table row, zero-indexed. 
        @param : column
        the specified table column, zero-indexed.
        @return an Accessible object representing the specified table
        cell.
        """
        func = self.get_dbus_method("getAccessibleAt")
        return func(*args, **kwargs)
    
    def getColumnAtIndex(self, *args, **kwargs):
        """
        Get the table column index occupied by the child at a particular
        1-D child index.
        @param : index
        the specified child index, zero-indexed.
        @return a long integer indicating the first column spanned by
        the child of a table, at the specified 1-D (zero-offset) index.
        """
        func = self.get_dbus_method("getColumnAtIndex")
        return func(*args, **kwargs)
    
    def getColumnDescription(self, *args, **kwargs):
        """
        Get a text description of a particular table column. This differs
        from AccessibleTable_getColumnHeader, which returns an Accessible.
        @param : column
        the specified table column, zero-indexed.
        @return a UTF-8 string describing the specified table column,
        if available.
        """
        func = self.get_dbus_method("getColumnDescription")
        return func(*args, **kwargs)
    
    def getColumnExtentAt(self, *args, **kwargs):
        """
        Get the number of columns spanned by the table cell at the specific
        row and column. (some tables can have cells which span multiple
        rows and/or columns).
        @param : row
        the specified table row, zero-indexed. 
        @param : column
        the specified table column, zero-indexed.
        @return a long integer indicating the number of columns spanned
        by the specified cell.
        """
        func = self.get_dbus_method("getColumnExtentAt")
        return func(*args, **kwargs)
    
    def getColumnHeader(self, *args, **kwargs):
        """
        Get the header associated with a table column, if available,
        as an instance of Accessible. This differs from getColumnDescription,
        which returns a string.
        @param : column
        the specified table column, zero-indexed.
        @return an Accessible representatin of the specified table column,
        if available.
        """
        func = self.get_dbus_method("getColumnHeader")
        return func(*args, **kwargs)
    
    def getIndexAt(self, *args, **kwargs):
        """
        Get the 1-D child index corresponding to the specified 2-D row
        and column indices. 
        @param : row
        the specified table row, zero-indexed. 
        @param : column
        the specified table column, zero-indexed.
        @return a long integer which serves as the index of a specified
        cell in the table, in a form usable by Accessible::getChildAtIndex.
        """
        func = self.get_dbus_method("getIndexAt")
        return func(*args, **kwargs)
    
    def getRowAtIndex(self, *args, **kwargs):
        """
        Get the table row index occupied by the child at a particular
        1-D child index.
        @param : index
        the specified child index, zero-indexed.
        @return a long integer indicating the first row spanned by the
        child of a table, at the specified 1-D (zero-offset) index.
        """
        func = self.get_dbus_method("getRowAtIndex")
        return func(*args, **kwargs)
    
    def getRowColumnExtentsAtIndex(self, *args, **kwargs):
        """
        Given a child index, determine the row and column indices and
        extents, and whether the cell is currently selected. If the child
        at index is not a cell (for instance, if it is a summary, caption,
        etc.), False is returned.
        @param : index
        the index of the Table child whose row/column extents are requested.
        @param : row
        back-filled with the first table row associated with the cell
        with child index index. 
        @param : col
        back-filled with the first table column associated with the cell
        with child index index. 
        @param : row_extents
        back-filled with the number of table rows across which child
        i extends. 
        @param : col_extents
        back-filled with the number of table columns across which child
        i extends. 
        @param : is_selected
        a boolean which is back-filled with True if the child at index
        i corresponds to a selected table cell, False otherwise.
        Example: If the Table child at index '6' extends across columns
        5 and 6 of row 2 of a Table instance, and is currently selected,
        then retval=table::getRowColumnExtentsAtIndex(6,row,col,
        row_extents,
        col_extents,
        is_selected);
         will return True, and after the call row, col, row_extents,
        col_extents, and is_selected will contain 2, 5, 1, 2, and True,
        respectively.
        @return True if the index is associated with a valid table cell,
        False if the index does not correspond to a cell. If False is
        returned, the values of the out parameters are undefined.
        """
        func = self.get_dbus_method("getRowColumnExtentsAtIndex")
        return func(*args, **kwargs)
    
    def getRowDescription(self, *args, **kwargs):
        """
        Get a text description of a particular table row. This differs
        from AccessibleTable_getRowHeader, which returns an Accessible.
        @param : row
        the specified table row, zero-indexed.
        @return a UTF-8 string describing the specified table row, if
        available.
        """
        func = self.get_dbus_method("getRowDescription")
        return func(*args, **kwargs)
    
    def getRowExtentAt(self, *args, **kwargs):
        """
        Get the number of rows spanned by the table cell at the specific
        row and column. (some tables can have cells which span multiple
        rows and/or columns).
        @param : row
        the specified table row, zero-indexed. 
        @param : column
        the specified table column, zero-indexed.
        @return a long integer indicating the number of rows spanned
        by the specified cell.
        """
        func = self.get_dbus_method("getRowExtentAt")
        return func(*args, **kwargs)
    
    def getRowHeader(self, *args, **kwargs):
        """
        Get the header associated with a table row, if available. This
        differs from getRowDescription, which returns a string.
        @param : row
        the specified table row, zero-indexed.
        @return an Accessible representatin of the specified table row,
        if available.
        """
        func = self.get_dbus_method("getRowHeader")
        return func(*args, **kwargs)
    
    def getSelectedColumns(self, *args, **kwargs):
        """
        Obtain the indices of all columns which are currently selected.
        @return a sequence of integers comprising the indices of columns
        currently selected.
        """
        func = self.get_dbus_method("getSelectedColumns")
        return func(*args, **kwargs)
    
    def getSelectedRows(self, *args, **kwargs):
        """
        Obtain the indices of all rows which are currently selected.
        @return a sequence of integers comprising the indices of rows
        currently selected.
        """
        func = self.get_dbus_method("getSelectedRows")
        return func(*args, **kwargs)
    
    def isColumnSelected(self, *args, **kwargs):
        """
        Determine whether a table column is selected. 
        @param : column
        the column being queried.
        @return True if the specified column is currently selected, False
        if not.
        """
        func = self.get_dbus_method("isColumnSelected")
        return func(*args, **kwargs)
    
    def isRowSelected(self, *args, **kwargs):
        """
        Determine whether a table row is selected. 
        @param : row
        the row being queried.
        @return True if the specified row is currently selected, False
        if not.
        """
        func = self.get_dbus_method("isRowSelected")
        return func(*args, **kwargs)
    
    def isSelected(self, *args, **kwargs):
        """
        Determine whether the cell at a specific row and column is selected.
        @param : row
        a row occupied by the cell whose state is being queried. 
        @param : column
        a column occupied by the cell whose state is being queried.
        @return True if the specified cell is currently selected, False
        if not.
        """
        func = self.get_dbus_method("isSelected")
        return func(*args, **kwargs)
    
    def removeColumnSelection(self, *args, **kwargs):
        """
        Remove the specified column from current column selection, if
        the table's selection model permits it.
        @param : column
        @return True if the specified column was successfully de-selected,
        False if not.
        """
        func = self.get_dbus_method("removeColumnSelection")
        return func(*args, **kwargs)
    
    def removeRowSelection(self, *args, **kwargs):
        """
        Remove the specified row from current row selection, if the table's
        selection model permits it.
        @param : row
        @return True if the specified row was successfully de-selected,
        False if not.
        """
        func = self.get_dbus_method("removeRowSelection")
        return func(*args, **kwargs)
    
    def get_caption(self):
        return self._pgetter(self._dbus_interface, "caption")
    def set_caption(self, value):
        self._psetter(self._dbus_interface, "caption", value)
    _captionDoc = \
        """
        An Accessible which represents of a caption for a Table.
        """
    caption = property(fget=get_caption, fset=set_caption, doc=_captionDoc)
    
    def get_nColumns(self):
        return self._pgetter(self._dbus_interface, "nColumns")
    def set_nColumns(self, value):
        self._psetter(self._dbus_interface, "nColumns", value)
    _nColumnsDoc = \
        """
        The total number of columns in this table (including empty columns),
        exclusive of columns which are programmatically hidden. Columns
        which are scrolled out of view or clipped by the current viewport
        are included.
        """
    nColumns = property(fget=get_nColumns, fset=set_nColumns, doc=_nColumnsDoc)
    
    def get_nRows(self):
        return self._pgetter(self._dbus_interface, "nRows")
    def set_nRows(self, value):
        self._psetter(self._dbus_interface, "nRows", value)
    _nRowsDoc = \
        """
        The total number of rows in this table (including empty rows),
        exclusive of any rows which are programmatically hidden. Rows
        which are merely scrolled out of view are included.
        """
    nRows = property(fget=get_nRows, fset=set_nRows, doc=_nRowsDoc)
    
    def get_nSelectedColumns(self):
        return self._pgetter(self._dbus_interface, "nSelectedColumns")
    def set_nSelectedColumns(self, value):
        self._psetter(self._dbus_interface, "nSelectedColumns", value)
    _nSelectedColumnsDoc = \
        """
        The number of columns currently selected. A selected column is
        one in which all included cells are selected.
        """
    nSelectedColumns = property(fget=get_nSelectedColumns, fset=set_nSelectedColumns, doc=_nSelectedColumnsDoc)
    
    def get_nSelectedRows(self):
        return self._pgetter(self._dbus_interface, "nSelectedRows")
    def set_nSelectedRows(self, value):
        self._psetter(self._dbus_interface, "nSelectedRows", value)
    _nSelectedRowsDoc = \
        """
        The number of rows currently selected. A selected row is one
        in which all included cells are selected.
        """
    nSelectedRows = property(fget=get_nSelectedRows, fset=set_nSelectedRows, doc=_nSelectedRowsDoc)
    
    def get_summary(self):
        return self._pgetter(self._dbus_interface, "summary")
    def set_summary(self, value):
        self._psetter(self._dbus_interface, "summary", value)
    _summaryDoc = \
        """
        An accessible object which summarizes the contents of a Table.
        This object is frequently itself a Table instance, albeit a simplified
        one.
        """
    summary = property(fget=get_summary, fset=set_summary, doc=_summaryDoc)

# ATTENTION - Register the Application class with the accessible factory.
add_accessible_class(interfaces.ATSPI_TABLE, Table)

#END----------------------------------------------------------------------------
