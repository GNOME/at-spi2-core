/* This file contains both declaration and definition of the TestSimpleTable,
 * a GObject that pretends to implement the AtkTableIface interface (it 
 * registers appropriate interface), but provides no implementation for any of the
 * methods of this interface (NULL-filled vftbl).
 */

#include "test-simple-table.h"

#include <stdio.h>
#include <string.h>

GType 
test_simple_table_get_type (void);

///////////////////////////////////////////////////////////////////////////
// Helper functions and data
///////////////////////////////////////////////////////////////////////////

// row/column selection schemes
gboolean ss_rows_none[NROWS] = {FALSE, FALSE, FALSE, FALSE};
gboolean ss_rows_02[NROWS]   = {TRUE,  FALSE, TRUE,  FALSE};

gboolean ss_cols_none[NCOLS] = {FALSE, FALSE, FALSE, FALSE, FALSE};
gboolean ss_cols_24[NCOLS]   = {FALSE, FALSE, TRUE,  FALSE, TRUE};

// Select table rows or columns according to the specified selection scheme.
// TRUE means 'selected', FALSE - 'not selected'.
void
test_simple_table_select_rows (TestSimpleTable* table, gboolean sel_scheme[])
{
    int i;
    for (i = 0; i < NROWS; ++i)
    {
        table->row[i].selected = sel_scheme[i];
    }    
}

void
test_simple_table_select_columns (TestSimpleTable* table, gboolean sel_scheme[])
{
    int i;
    for (i = 0; i < NCOLS; ++i)
    {
        table->col[i].selected = sel_scheme[i];
    }    
}

// Reset row and column headers to their default values
void
test_simple_table_reset_headers (TestSimpleTable* table)
{
    int i, j;
    
    // initialize row and column descriptors
    for (i = 0; i < NROWS; ++i)
    {
        sprintf (TEST_SIMPLE_TEXT (table->row[i].hdr)->text, DEF_ROW_DESCR_TPL, i);
    }
    
    for (j = 0; j < NCOLS; ++j)
    {
        sprintf (TEST_SIMPLE_TEXT (table->col[j].hdr)->text, DEF_COL_DESCR_TPL, j);
    }
}

///////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////
static GObjectClass *parent_class_simple_table = NULL;

static AtkObject* 
test_simple_table_ref_at (AtkTable *table, gint row, gint column)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return NULL;
    }
    
    if ((row >=0) && (row < self->nrows) && 
        (column >= 0) && (column < self->ncols))
    {
        return ATK_OBJECT (self->tbl[row][column].elem);
    }
    else
    { 
        return NULL;
    }
}

static gint 
test_simple_table_get_index_at (AtkTable *table, gint row, gint column)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return -1;
    }

    if ((row >=0) && (row < self->nrows) && 
        (column >= 0) && (column < self->ncols))
    {
        // non-child objects have ID=-1 by default.
        return (self->tbl[row][column].elem->id);
    }
    else
    { 
        return -1;
    }
}

static gint 
test_simple_table_get_column_at_index (AtkTable *table, gint index_)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return -1;
    }

    int i, j;
    for (i = 0; i < self->nrows; ++i)
    {
        for (j = 0; j < self->ncols; ++j)
        {
            if (self->tbl[i][j].elem->id == index_)
            {
                return j;
            }
        }
    }
    
    return -1;
}

static gint 
test_simple_table_get_row_at_index (AtkTable *table, gint index_)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return -1;
    }
    
    if (index_ < 0)
    {
        return -1;
    }
    
    int i, j;
    for (i = 0; i < self->nrows; ++i)
    {
        for (j = 0; j < self->ncols; ++j)
        {
            if (self->tbl[i][j].elem->id == index_)
            {
                return i;
            }
        }
    }
    return -1;
}

static gint 
test_simple_table_get_n_columns (AtkTable *table)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return 0;
    }

    return self->ncols;
}

static gint 
test_simple_table_get_n_rows (AtkTable *table)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return 0;
    }

    return self->nrows;
}

static gint 
test_simple_table_get_column_extent_at (AtkTable *table, gint row, gint column)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return 0;
    }

    if ((row >=0) && (row < self->nrows) && 
        (column >= 0) && (column < self->ncols))
    {
        return (self->tbl[row][column].ext_col);
    }
    else
    { 
        return 0;
    }
}

static gint 
test_simple_table_get_row_extent_at (AtkTable *table, gint row, gint column)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return 0;
    }

    if ((row >=0) && (row < self->nrows) && 
        (column >= 0) && (column < self->ncols))
    {
        return (self->tbl[row][column].ext_row);
    }
    else
    { 
        return 0;
    }

    return 0;
}

static AtkObject* 
test_simple_table_get_caption (AtkTable *table)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return NULL;
    }

    return (self->caption);
}

static const gchar* 
test_simple_table_get_column_description (AtkTable *table, gint column)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return NULL;
    }
    
    if ((column < 0) || (column >= self->ncols))
    {
        return NULL;
    }
    
    return TEST_SIMPLE_TEXT (self->col[column].hdr)->text;
}

static AtkObject* 
test_simple_table_get_column_header (AtkTable *table, gint column)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return NULL;
    }

    if ((column < 0) || (column >= self->ncols))
    {
        return NULL;
    }
    
    return (self->col[column].hdr);
}

static const gchar* 
test_simple_table_get_row_description (AtkTable *table, gint row)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return NULL;
    }

    if ((row < 0) || (row >= self->nrows))
    {
        return NULL;
    }
    
    return TEST_SIMPLE_TEXT (self->row[row].hdr)->text;
}

static AtkObject* 
test_simple_table_get_row_header (AtkTable *table, gint row)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return NULL;
    }

    if ((row < 0) || (row >= self->nrows))
    {
        return NULL;
    }
    
    return (self->row[row].hdr);
}

static AtkObject* 
test_simple_table_get_summary (AtkTable *table)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return NULL;
    }

    return (self->summary);
}

static void 
test_simple_table_set_caption (AtkTable *table, AtkObject *caption)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed || !caption)
    {
        return;
    }

    // unref the old caption
    g_object_unref (G_OBJECT (self->caption));
    
    // and replace it with a new one
    self->caption = caption;
    g_object_ref (G_OBJECT (self->caption));
    return;
}

static void 
test_simple_table_set_column_description (AtkTable *table, gint column, const gchar *description)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return;
    }

    if ((column < 0) || (column >= self->ncols) || !description)
    {
        return;
    }
    
    strncpy (TEST_SIMPLE_TEXT (self->col[column].hdr)->text, description, MAX_DESCR_LENGTH);
    return;
}

static void 
test_simple_table_set_column_header (AtkTable *table, gint column, AtkObject *header)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return;
    }

    if ((column < 0) || (column >= self->ncols) || !header)
    {
        return;
    }
    
    // unref old header
    g_object_unref (G_OBJECT (self->col[column].hdr));
    
    // and replace it with a new one
    g_object_ref (G_OBJECT (header));
    self->col[column].hdr = header;
    return;
}

static void 
test_simple_table_set_row_description (AtkTable *table, gint row, const gchar *description)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return;
    }

    if ((row < 0) || (row >= self->nrows) || !description)
    {
        return;
    }
    
    strncpy (TEST_SIMPLE_TEXT (self->row[row].hdr)->text, description, MAX_DESCR_LENGTH);
    return;
}

static void 
test_simple_table_set_row_header (AtkTable *table, gint row, AtkObject *header)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return;
    }

    if ((row < 0) || (row >= self->nrows) || !header)
    {
        return;
    }
    
    // unref old header
    g_object_unref (G_OBJECT (self->row[row].hdr));
    
    // and replace it with a new one
    g_object_ref (G_OBJECT (header));
    self->row[row].hdr = header;
    
    return;
}

static void 
test_simple_table_set_summary (AtkTable *table, AtkObject *accessible)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed || !accessible)
    {
        return;
    }

    // unref the old summary
    g_object_unref (G_OBJECT (self->summary));
    
    // and replace it with a new one
    self->summary = accessible;
    g_object_ref (G_OBJECT (self->summary));
    return;
}

static gint 
test_simple_table_get_selected_columns (AtkTable *table, gint **selected)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed || !selected)
    {
        return 0;
    }

    // count the selected columns
    int i;
    int nsel = 0;
    for (i = 0; i < self->ncols; ++i)
    {
        if (self->col[i].selected)
        {
            ++nsel;
        }
    }
    
    if (nsel > 0)
    {
        // store the indexes of the selected columns 
        int pos = 0;
        *selected = g_new (gint, nsel);
        for (i = 0; i < self->ncols; ++i)
        {
            if (self->col[i].selected)
            {
                (*selected)[pos] = i;
                ++pos;
            }
        } // end for i
    }
    
    return nsel;
}

static gint 
test_simple_table_get_selected_rows (AtkTable *table, gint **selected)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed || !selected)
    {
        return 0;
    }

    // count the selected rows
    int i;
    int nsel = 0;
    for (i = 0; i < self->nrows; ++i)
    {
        if (self->row[i].selected)
        {
            ++nsel;
        }
    }
    
    if (nsel > 0)
    {
        // store the indexes of the selected rows 
        int pos = 0;
        *selected = g_new (gint, nsel);
        for (i = 0; i < self->nrows; ++i)
        {
            if (self->row[i].selected)
            {
                (*selected)[pos] = i;
                ++pos;
            }
        } // end for i
    }
    
    return nsel;
}

static gboolean 
test_simple_table_is_column_selected (AtkTable *table, gint column)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return FALSE;
    }

    if ((column < 0) || (column >= self->ncols))
    {
        return FALSE;
    }
    
    return self->col[column].selected;
}

static gboolean 
test_simple_table_is_row_selected (AtkTable *table, gint row)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return FALSE;
    }

    if ((row < 0) || (row >= self->nrows))
    {
        return FALSE;
    }
    
    return self->row[row].selected;
}

static gboolean 
test_simple_table_is_selected (AtkTable *table, gint row, gint column)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return FALSE;
    }

    if ((row < 0) || (row >= self->nrows) ||
        (column < 0) || (column >= self->ncols))
    {
        return FALSE;
    }
    
    // In this implementation a cell is selected if and only if the
    // corresponding row and column are both selected.
    
    return (self->row[row].selected && self->col[column].selected);
}

static gboolean 
test_simple_table_add_row_selection (AtkTable *table, gint row)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return FALSE;
    }

    if ((row < 0) || (row >= self->nrows))
    {
        return FALSE;
    }
    
    gboolean was_selected = self->row[row].selected;
    self->row[row].selected = TRUE;
    return !was_selected;
}

static gboolean 
test_simple_table_remove_row_selection (AtkTable *table, gint row)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return FALSE;
    }

    gboolean was_selected = self->row[row].selected;
    self->row[row].selected = FALSE;
    return was_selected;
}

static gboolean 
test_simple_table_add_column_selection (AtkTable *table, gint column)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return FALSE;
    }

    if ((column < 0) || (column >= self->ncols))
    {
        return FALSE;
    }
    
    gboolean was_selected = self->col[column].selected;
    self->col[column].selected = TRUE;
    return !was_selected;
}

static gboolean 
test_simple_table_remove_column_selection (AtkTable *table, gint column)
{
    TestSimpleTable* self = (TestSimpleTable*)table;
    if ((!self) || self->disposed)
    {
        return FALSE;
    }

    gboolean was_selected = self->col[column].selected;
    self->col[column].selected = FALSE;
    return was_selected;
}

// Signal emitters
// "column-deleted"
static void
emit_column_deleted (TestSimpleTable* table, gint arg1, gint arg2)
{
    g_signal_emit_by_name ((gpointer)table, "column-deleted", arg1, arg2);
    return;
}

// "column-inserted"
static void
emit_column_inserted (TestSimpleTable* table, gint arg1, gint arg2)
{
    g_signal_emit_by_name ((gpointer)table, "column-inserted", arg1, arg2);
    return;
}
    
// "column-reordered"
static void
emit_column_reordered (TestSimpleTable* table)
{
    g_signal_emit_by_name ((gpointer)table, "column-reordered");
    return;
}
    
// "model-changed"
static void
emit_model_changed (TestSimpleTable* table)
{
    g_signal_emit_by_name ((gpointer)table, "model-changed");
    return;
}
    
// "row-deleted"
static void
emit_row_deleted (TestSimpleTable* table, gint arg1, gint arg2)
{
    g_signal_emit_by_name ((gpointer)table, "row-deleted", arg1, arg2);
    return;
}
    
// "row-inserted"
static void
emit_row_inserted (TestSimpleTable* table, gint arg1, gint arg2)
{
    g_signal_emit_by_name ((gpointer)table, "row-inserted", arg1, arg2);
    return;
}
    
// "row-reordered"
static void
emit_row_reordered (TestSimpleTable* table)
{
    g_signal_emit_by_name ((gpointer)table, "row-reordered");
    return;
}


/******************************************************************/
static void
simple_table_interface_init (gpointer g_iface, gpointer iface_data)
{
    AtkTableIface *klass = (AtkTableIface *)g_iface;
    
    /* set up overrides here */
    klass-> ref_at =
        (AtkObject* (*) (AtkTable *table, gint row, gint column)) test_simple_table_ref_at;
    klass-> get_index_at =
        (gint (*) (AtkTable *table, gint row, gint column)) test_simple_table_get_index_at;
    klass-> get_column_at_index =
        (gint (*) (AtkTable *table, gint index_)) test_simple_table_get_column_at_index;
    klass-> get_row_at_index =
        (gint (*) (AtkTable *table, gint index_)) test_simple_table_get_row_at_index;
    klass-> get_n_columns =
        (gint (*) (AtkTable *table)) test_simple_table_get_n_columns;
    klass-> get_n_rows =
        (gint (*) (AtkTable *table)) test_simple_table_get_n_rows;
    klass-> get_column_extent_at =
        (gint (*) (AtkTable *table, gint row, gint column)) test_simple_table_get_column_extent_at;
    klass-> get_row_extent_at =
        (gint (*) (AtkTable *table, gint row, gint column)) test_simple_table_get_row_extent_at;
    klass-> get_caption =
        (AtkObject* (*) (AtkTable *table)) test_simple_table_get_caption;
    klass-> get_column_description =
        (const gchar* (*) (AtkTable *table, gint column)) test_simple_table_get_column_description;
    klass-> get_column_header =
        (AtkObject* (*) (AtkTable *table, gint column)) test_simple_table_get_column_header;
    klass-> get_row_description =
        (const gchar* (*) (AtkTable *table, gint row)) test_simple_table_get_row_description;
    klass-> get_row_header =
        (AtkObject* (*) (AtkTable *table, gint row)) test_simple_table_get_row_header;
    klass-> get_summary =
        (AtkObject* (*) (AtkTable *table)) test_simple_table_get_summary;
    klass-> set_caption =
        (void (*) (AtkTable *table, AtkObject *caption)) test_simple_table_set_caption;
    klass-> set_column_description =
        (void (*) (AtkTable *table, gint column, const gchar *description)) test_simple_table_set_column_description;
    klass-> set_column_header =
        (void (*) (AtkTable *table, gint column, AtkObject *header)) test_simple_table_set_column_header;
    klass-> set_row_description =
        (void (*) (AtkTable *table, gint row, const gchar *description)) test_simple_table_set_row_description;
    klass-> set_row_header =
        (void (*) (AtkTable *table, gint row, AtkObject *header)) test_simple_table_set_row_header ;
    klass-> set_summary =
        (void (*) (AtkTable *table, AtkObject *accessible)) test_simple_table_set_summary ;
    klass-> get_selected_columns =
        (gint (*) (AtkTable *table, gint **selected)) test_simple_table_get_selected_columns;
    klass-> get_selected_rows =
        (gint (*) (AtkTable *table, gint **selected)) test_simple_table_get_selected_rows;
    klass-> is_column_selected =
        (gboolean (*) (AtkTable *table, gint column)) test_simple_table_is_column_selected;
    klass-> is_row_selected =
        (gboolean (*) (AtkTable *table, gint row)) test_simple_table_is_row_selected;
    klass-> is_selected =
        (gboolean (*) (AtkTable *table, gint row, gint column)) test_simple_table_is_selected;
    klass-> add_row_selection =
        (gboolean (*) (AtkTable *table, gint row)) test_simple_table_add_row_selection;
    klass-> remove_row_selection =
        (gboolean (*) (AtkTable *table, gint row)) test_simple_table_remove_row_selection;
    klass-> add_column_selection =
        (gboolean (*) (AtkTable *table, gint column)) test_simple_table_add_column_selection;
    klass-> remove_column_selection =
        (gboolean (*) (AtkTable *table, gint column)) test_simple_table_remove_column_selection;
}

static void
simple_table_instance_init (GTypeInstance *instance, gpointer g_class)
{
    TestSimpleTable *self = (TestSimpleTable *)instance;
    
    self->disposed = FALSE;
    
    self->nrows = NROWS;
    self->ncols = NCOLS;
    
    /* create & initialize child objects */
    int i, j;
    for (i = 0; i < NCHILDREN; ++i)
    {
        self->child[i] = TEST_SIMPLE_CHILD (g_object_new (TEST_TYPE_SIMPLE_CHILD, NULL));
        self->child[i]->id = i;
    }
    
    self->not_a_child = TEST_SIMPLE_CHILD (g_object_new (TEST_TYPE_SIMPLE_CHILD, NULL));
    
    // initialize table cells
    for (i = 0; i < NROWS; ++i)
    {
        for (j = 0; j < NCOLS; ++j)
        {
            if (ids[i][j] >= 0)
            {
                self->tbl[i][j].elem = self->child[ids[i][j]];
            }
            else
            {
                self->tbl[i][j].elem = self->not_a_child;
            }
            
            self->tbl[i][j].ext_row = row_ext[i][j];
            self->tbl[i][j].ext_col = col_ext[i][j];
        } // end for j
    } // end for i
    
    self->caption = ATK_OBJECT (g_object_new (TEST_TYPE_SIMPLE_TEXT, NULL));
    strncpy (TEST_SIMPLE_TEXT (self->caption)->text, DEF_CAPTION_TEXT, MAX_DESCR_LENGTH);
    
    self->summary = ATK_OBJECT (g_object_new (TEST_TYPE_SIMPLE_TEXT, NULL));
    strncpy (TEST_SIMPLE_TEXT (self->summary)->text, DEF_SUMMARY_TEXT, MAX_DESCR_LENGTH);
    
    // initialize row and column descriptors
    for (i = 0; i < NROWS; ++i)
    {
        self->row[i].hdr = ATK_OBJECT (g_object_new (TEST_TYPE_SIMPLE_TEXT, NULL));
        self->row[i].selected = FALSE;
    }
    
    for (j = 0; j < NCOLS; ++j)
    {
        self->col[j].hdr = ATK_OBJECT (g_object_new (TEST_TYPE_SIMPLE_TEXT, NULL));
        self->col[j].selected = FALSE;
    }
    
    test_simple_table_reset_headers(self);
}

static void
test_simple_table_dispose (GObject *obj)
{
    TestSimpleTable *self = (TestSimpleTable *)obj;

    if (self->disposed) 
    {
        return;
    }
    
    /* Make sure dispose does not run twice. */
    self->disposed = TRUE;

    int i;
    for (i = 0; i < NCHILDREN; ++i)
    {
        g_object_unref (G_OBJECT (self->child[i]));
    }
    g_object_unref (G_OBJECT (self->not_a_child));

    for (i = 0; i < NROWS; ++i)
    {
        g_object_unref (G_OBJECT (self->row[i].hdr));
    }
    
    for (i = 0; i < NCOLS; ++i)
    {
        g_object_unref (G_OBJECT (self->col[i].hdr));
    }
    
    g_object_unref (G_OBJECT (self->caption));
    g_object_unref (G_OBJECT (self->summary));

    /* Chain up to the parent class */
    G_OBJECT_CLASS (parent_class_simple_table)->dispose (obj);
}

static void
test_simple_table_finalize (GObject *obj)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (parent_class_simple_table)->finalize (obj);
}

static void
test_simple_table_class_init (gpointer g_class, gpointer g_class_data)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
    TestSimpleTableClass *klass = TEST_SIMPLE_TABLE_CLASS (g_class);

    gobject_class->dispose = test_simple_table_dispose;
    gobject_class->finalize = test_simple_table_finalize;

    parent_class_simple_table = g_type_class_peek_parent (klass);
}

GType 
test_simple_table_get_type (void)
{
    static GType type = 0;
    if (type == 0) 
    {
        static const GTypeInfo info = 
        {
            sizeof (TestSimpleTableClass),
            NULL,   /* base_init */
            NULL,   /* base_finalize */
            test_simple_table_class_init, /* class_init */
            NULL,   /* class_finalize */
            NULL,   /* class_data */
            sizeof (TestSimpleTable),
            0,      /* n_preallocs */
            simple_table_instance_init    /* instance_init */
        };
                
        static const GInterfaceInfo iface_info = 
        {
            (GInterfaceInitFunc) simple_table_interface_init,    /* interface_init */
            NULL,                                       /* interface_finalize */
            NULL                                        /* interface_data */
        };
        type = g_type_register_static (ATK_TYPE_OBJECT,
                                       "TestSimpleTableType",
                                       &info, 0);
        g_type_add_interface_static (type,
                                     ATK_TYPE_TABLE,
                                     &iface_info);
    }
    return type;
}
