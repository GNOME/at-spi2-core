#ifndef MY_ATK_TABLE_H
#define MY_ATK_TABLE_H

#include <glib-object.h>
#include <atk/atk.h> 
#include <my-atk-object.h>
    
#define MY_TYPE_ATK_TABLE             (my_atk_table_get_type ())
#define MY_ATK_TABLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ATK_TABLE, MyAtkTable))
#define MY_ATK_TABLE_CLASS(vtable)    (G_TYPE_CHECK_CLASS_CAST ((vtable), MY_TYPE_ATK_TABLE, MyAtkTableClass))
#define MY_IS_ATK_TABLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ATK_TABLE))
#define MY_IS_ATK_TABLE_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), MY_TYPE_ATK_TABLE))
#define MY_ATK_TABLE_GET_CLASS(inst)  (G_TYPE_INSTANCE_GET_CLASS ((inst), MY_TYPE_ATK_TABLE, MyAtkTableClass))

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

/* row and column headers */
typedef struct
{
    AtkObject* hdr;
    gboolean selected;  /* TRUE if the row/column is selected, FALSE otherwise */
} TestSimpleHeaderStruct;

/* This struct represents a table cell */
typedef struct
{
    MyAtkObject* elem;   /* the element */
    guint ext_row;           /* its row extent */
    guint ext_col;           /* its column extent */
} TestSimpleCell;

typedef struct _MyAtkTable MyAtkTable;
typedef struct _MyAtkTableClass MyAtkTableClass;

struct _MyAtkTable 
{
    MyAtkObject parent;
        
    gboolean disposed;
    
    MyAtkObject* child[NCHILDREN];
    MyAtkObject* not_a_child;
    TestSimpleHeaderStruct row[NROWS];
    TestSimpleHeaderStruct col[NCOLS];
    guint nrows;
    guint ncols;
    AtkObject* caption;
    AtkObject* summary;
};

struct _MyAtkTableClass 
{
    MyAtkObjectClass parent;
};

GType 
my_atk_table_get_type (void);

#endif /*MY_ATK_TABLE_H*/
