/* ATK -  Accessibility Toolkit
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

#ifndef __ATK_TABLE_H__
#define __ATK_TABLE_H__

#include <atk/atkobject.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * AtkTable describes a user-interface component that presents data in
 * two-dimensional table format.
 */


#define ATK_TYPE_TABLE                    (atk_table_get_type ())
#define ATK_IS_TABLE(obj)                 G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATK_TYPE_TABLE)
#define ATK_TABLE(obj)                    G_TYPE_CHECK_INSTANCE_CAST ((obj), ATK_TYPE_TABLE, AtkTable)
#define ATK_TABLE_GET_IFACE(obj)          (G_TYPE_INSTANCE_GET_INTERFACE ((obj), ATK_TYPE_TABLE, AtkTableIface))

#ifndef _TYPEDEF_ATK_TABLE_
#define _TYPEDEF_ATK_TABLE_
typedef struct _AtkTable AtkTable;
#endif

struct _AtkTableIface
{
  GTypeInterface parent;

  /*
   * Returns a reference to the accessible object at a specified row 
   * and column in the table.
   */
  AtkObject*        (* ref_at)                   (AtkTable      *table,
                                                  gint          row,
                                                  gint          column);
  gint              (* get_index_at)             (AtkTable      *table,
                                                  gint          row,
                                                  gint          column);
  gint              (* get_row_at_index)         (AtkTable      *table,
                                                  gint          index);
  gint              (* get_column_at_index)      (AtkTable      *table,
                                                  gint          index);
  /*
   * Returns the caption for the table.
   */
  AtkObject*        (* get_caption)              (AtkTable      *table);
  /*
   * Returns the number of columns in the table.
   */
  gint              (* get_n_columns)           (AtkTable      *table);
  /*
   * Returns the description text of the specified column in the table
   */
  AtkObject*        (* get_column_description)   (AtkTable      *table,
                                                  gint          column);
  /*
   * Returns the number of columns occupied by the accessible object
   * at a specified row and column in the table.
   */
  gint              (* get_column_extent_at)     (AtkTable      *table,
                                                  gint          row,
                                                  gint          column);
  /*
   * Returns the column headers of an accessible table.
   */
  AtkTable*         (* get_column_header)        (AtkTable      *table);
  /*
   * Returns the number of rows in the table.
   */
  gint              (* get_n_rows)               (AtkTable      *table);
  /*
   * Returns the description text of the specified row in the table
   */
  AtkObject*        (* get_row_description)      (AtkTable      *table,
                                                  gint          row);
  /*
   * Returns the number of rows occupied by the accessible object
   * at a specified row and column in the table.
   */
  gint              (* get_row_extent_at)        (AtkTable      *table,
                                                  gint          row,
                                                  gint          column);
  /*
   * Returns the row headers of an accessible table.
   */
  AtkTable*         (* get_row_header)           (AtkTable      *table);
  /*
   * Returns the summary description of the table.
   */
  AtkObject*        (* get_summary)              (AtkTable      *table);
  /*
   * Returns the selected columns of the table.
   */
  gint*             (* get_selected_columns)     (AtkTable      *table);
  /*
   * Returns the selected rows of the table.
   */
  gint*             (* get_selected_rows)        (AtkTable      *table);
  /*
   * Returns a boolean value indicating whether the specified column
   * is selected
   */
  gboolean          (* is_column_selected)       (AtkTable      *table,
                                                  gint          column);
  /*
   * Returns a boolean value indicating whether the specified row
   * is selected
   */
  gboolean          (* is_row_selected)          (AtkTable      *table,
                                                  gint          row);
  /*
   * Returns a boolean value indicating whether the acessible object
   * at the specified row and column is selected
   */
  gboolean          (* is_selected)              (AtkTable      *table,
                                                  gint          row,
                                                  gint          column);
  /*
   * Sets the caption for the table.
   */
  void              (* set_caption)              (AtkTable      *table,
                                                  AtkObject     *accessible);
  /*
   * Sets the description text for the specified column of the table.
   */
  void              (* set_column_description)   (AtkTable      *table,
                                                  gint          column,
                                                  AtkObject     *accessible);
  /*
   * Sets the column headers
   */
  void              (* set_column_header)        (AtkTable      *table,
                                                  gint          column,
                                                  AtkTable      *header);
  /*
   * Sets the description text for the specified row of the table.
   */
  void              (* set_row_description)      (AtkTable      *table,
                                                  gint          row,
                                                  AtkObject     *accessible);
  /*
   * Sets the row headers
   */
  void              (* set_row_header)           (AtkTable      *table,
                                                  gint          row,
                                                  AtkTable      *header);
  /*
   * Sets the summary description of the table
   */
  void              (* set_summary)              (AtkTable      *table,
                                                  AtkObject     *accessible);
};

GType atk_table_get_type ();

AtkObject*        atk_table_ref_at               (AtkTable         *table,
                                                  gint             row,
                                                  gint             column);
gint              atk_table_get_index_at         (AtkTable         *table,
                                                  gint             row,
                                                  gint             column);
gint              atk_table_get_row_at_index     (AtkTable         *table,
                                                  gint             index);
gint              atk_table_get_column_at_index  (AtkTable         *table,
                                                  gint             index);
AtkObject*        atk_table_get_caption          (AtkTable         *table);
gint              atk_table_get_n_columns        (AtkTable         *table);
AtkObject*        atk_table_get_column_description (AtkTable         *table,
                                                    gint             column);
gint              atk_table_get_column_extent_at (AtkTable         *table,
                                                  gint             row,
                                                  gint             column);
AtkTable*         atk_table_get_column_header    (AtkTable         *table);
gint              atk_table_get_n_rows           (AtkTable         *table);
AtkObject*        atk_table_get_row_description  (AtkTable         *table,
                                                  gint             r);
gint              atk_table_get_row_extent_at    (AtkTable         *table,
                                                  gint             row,
                                                  gint             column);
AtkTable*         atk_table_get_row_header       (AtkTable         *table);
AtkObject*        atk_table_get_summary          (AtkTable         *table);
gint*             atk_table_get_selected_columns (AtkTable         *table);
gint*             atk_table_get_selected_rows    (AtkTable         *table);
gboolean          atk_table_is_column_selected   (AtkTable         *table,
                                                  gint             column);
gboolean          atk_table_is_row_selected      (AtkTable         *table,
                                                  gint             row);
gboolean          atk_table_is_selected          (AtkTable         *table,
                                                  gint             row,
                                                  gint             column);
void              atk_table_set_caption          (AtkTable         *table,
                                                  AtkObject        *accessible);
void              atk_table_set_column_description (AtkTable       *table,
                                                    gint           column,
                                                    AtkObject      *accessible);
void              atk_table_set_column_header    (AtkTable         *table,
                                                  gint             column,
                                                  AtkTable         *header);
void              atk_table_set_row_description  (AtkTable         *table,
                                                  gint             row,
                                                  AtkObject        *accessible);
void              atk_table_set_row_header       (AtkTable         *table,
                                                  gint             row,
                                                  AtkTable         *header);
void              atk_table_set_summary          (AtkTable         *table,
                                                  AtkObject        *accessible);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __ATK_TABLE_H__ */
