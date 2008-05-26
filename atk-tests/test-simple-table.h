/* This file contains both declaration and definition of the TestSimpleTable,
 * a GObject that pretends to implement the AtkTableIface interface (it 
 * registers appropriate interface), but provides no implementation for any of the
 * methods of this interface (NULL-filled vftbl).
 */

#ifndef TESTSIMPLETABLE_H_
#define TESTSIMPLETABLE_H_

#include <glib-object.h>
#include <atk/atk.h> 

#include <string.h>

#include "test-simple-child.h"
#include "test-simple-text.h"

#define TEST_TYPE_SIMPLE_TABLE             (test_simple_table_get_type ())
#define TEST_SIMPLE_TABLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEST_TYPE_SIMPLE_TABLE, TestSimpleTable))
#define TEST_SIMPLE_TABLE_CLASS(vtable)    (G_TYPE_CHECK_CLASS_CAST ((vtable), TEST_TYPE_SIMPLE_TABLE, TestSimpleTableClass))
#define TEST_IS_SIMPLE_TABLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TEST_TYPE_SIMPLE_TABLE))
#define TEST_IS_SIMPLE_TABLE_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), TEST_TYPE_SIMPLE_TABLE))
#define TEST_SIMPLE_TABLE_GET_CLASS(inst)  (G_TYPE_INSTANCE_GET_CLASS ((inst), TEST_TYPE_SIMPLE_TABLE, TestSimpleTableClass))

#define NROWS 4     /* row count */
#define NCOLS 5     /* column count */

static gint ids[NROWS][NCOLS] = 
    { {0,  1,  2,  2,  3},
      {4,  5,  6,  7,  8},
      {9,  9, 10, 11, 12},
      {9,  9, 13, 14, -1} };
      
static gint row_ext[NROWS][NCOLS] = 
    { {1,  1,  1,  1,  1},
      {1,  1,  1,  1,  1},
      {2,  2,  1,  1,  1},
      {2,  2,  1,  1,  1} };

static gint col_ext[NROWS][NCOLS] = 
    { {1,  1,  2,  2,  1},
      {1,  1,  1,  1,  1},
      {2,  2,  1,  1,  1},
      {2,  2,  1,  1,  1} };

#define NCHILDREN 16    /* child object count */

// default string values
#define DEF_CAPTION_TEXT    "Default table caption"
#define DEF_SUMMARY_TEXT    "Default table summary"
#define DEF_ROW_DESCR_TPL   "Row No%d"
#define DEF_COL_DESCR_TPL   "Column No%d"

// convenience typedefs
typedef TestSimpleText TestSimpleHeader;
typedef TestSimpleText TestSimpleSummary;
//typedef TestSimpleText TestSimpleDescription;
typedef TestSimpleText TestSimpleCaption;

/* row and column headers */
typedef struct
{
    AtkObject* hdr;
    gboolean selected;  /* TRUE if the row/column is selected, FALSE otherwise */
} TestSimpleHeaderStruct;

/* This struct represents a table cell */
typedef struct
{
    TestSimpleChild* elem;   /* the element */
    guint ext_row;           /* its row extent */
    guint ext_col;           /* its column extent */
} TestSimpleCell;

/* TestSimpleTable class */
typedef struct _TestSimpleTable TestSimpleTable;
typedef struct _TestSimpleTableClass TestSimpleTableClass;

struct _TestSimpleTable 
{
    AtkObject parent;
    gboolean disposed;
    
    /* Children of this object. */
    TestSimpleChild* child[NCHILDREN];
    
    /* This object is not a child of the table object. */
    TestSimpleChild* not_a_child;
    
    /* The table itself. */
    TestSimpleCell tbl[NROWS][NCOLS];
    
    /* Row and column descriptors */
    TestSimpleHeaderStruct row[NROWS];
    TestSimpleHeaderStruct col[NCOLS];
        
    /* current number of rows and columns */
    guint nrows;
    guint ncols;
    
    AtkObject* caption;
    
    AtkObject* summary;
};

struct _TestSimpleTableClass 
{
    AtkObjectClass parent;
};

#endif /*TESTSIMPLETABLE_H_*/
