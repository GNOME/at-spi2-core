/* This file contains both declaration and definition of the MyAtkTable,
 * a GObject that pretends to implement the AtkTableIface interface (it 
 * registers appropriate interface), but provides no implementation for any of the
 * methods of this interface (NULL-filled vftbl).
 */

#include <glib-object.h>
#include <atk/atk.h> 

#include "my-atk-object.h"
#include "my-atk-table.h"
    
///////////////////////////////////////////////////////////////////////////
// Helper functions and data
///////////////////////////////////////////////////////////////////////////
void
my_atk_table_select_rows (MyAtkTable* table, gboolean sel_scheme[])
{
    // the function does nothing  
}

void
my_atk_table_select_columns (MyAtkTable* table, gboolean sel_scheme[])
{
    // the function does nothing
}

///////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////
static GObjectClass *parent_class_table = NULL;

/******************************************************************/
static void
table_interface_init (gpointer g_iface, gpointer iface_data)
{
    AtkTableIface *klass = (AtkTableIface *)g_iface;
    
    /* set up overrides here */
    klass-> ref_at =
        (AtkObject* (*) (AtkTable *table, gint row, gint column)) NULL;
    klass-> get_index_at =
        (gint (*) (AtkTable *table, gint row, gint column)) NULL;
    klass-> get_column_at_index =
        (gint (*) (AtkTable *table, gint index_)) NULL;
    klass-> get_row_at_index =
        (gint (*) (AtkTable *table, gint index_)) NULL;
    klass-> get_n_columns =
        (gint (*) (AtkTable *table)) NULL;
    klass-> get_n_rows =
        (gint (*) (AtkTable *table)) NULL;
    klass-> get_column_extent_at =
        (gint (*) (AtkTable *table, gint row, gint column)) NULL;
    klass-> get_row_extent_at =
        (gint (*) (AtkTable *table, gint row, gint column)) NULL;
    klass-> get_caption =
        (AtkObject* (*) (AtkTable *table)) NULL;
    klass-> get_column_description =
        (const gchar* (*) (AtkTable *table, gint column)) NULL;
    klass-> get_column_header =
        (AtkObject* (*) (AtkTable *table, gint column)) NULL;
    klass-> get_row_description =
        (const gchar* (*) (AtkTable *table, gint row)) NULL;
    klass-> get_row_header =
        (AtkObject* (*) (AtkTable *table, gint row)) NULL;
    klass-> get_summary =
        (AtkObject* (*) (AtkTable *table)) NULL;
    klass-> set_caption =
        (void (*) (AtkTable *table, AtkObject *caption)) NULL;
    klass-> set_column_description =
        (void (*) (AtkTable *table, gint column, const gchar *description)) NULL;
    klass-> set_column_header =
        (void (*) (AtkTable *table, gint column, AtkObject *header)) NULL;
    klass-> set_row_description =
        (void (*) (AtkTable *table, gint row, const gchar *description)) NULL;
    klass-> set_row_header =
        (void (*) (AtkTable *table, gint row, AtkObject *header)) NULL;
    klass-> set_summary =
        (void (*) (AtkTable *table, AtkObject *accessible)) NULL;
    klass-> get_selected_columns =
        (gint (*) (AtkTable *table, gint **selected)) NULL;
    klass-> get_selected_rows =
        (gint (*) (AtkTable *table, gint **selected)) NULL;
    klass-> is_column_selected =
        (gboolean (*) (AtkTable *table, gint column)) NULL;
    klass-> is_row_selected =
        (gboolean (*) (AtkTable *table, gint row)) NULL;
    klass-> is_selected =
        (gboolean (*) (AtkTable *table, gint row, gint column)) NULL;
    klass-> add_row_selection =
        (gboolean (*) (AtkTable *table, gint row)) NULL;
    klass-> remove_row_selection =
        (gboolean (*) (AtkTable *table, gint row)) NULL;
    klass-> add_column_selection =
        (gboolean (*) (AtkTable *table, gint column)) NULL;
    klass-> remove_column_selection =
        (gboolean (*) (AtkTable *table, gint column)) NULL;
}

static void
table_instance_init (GTypeInstance *instance, gpointer g_class)
{
    MyAtkTable *self = (MyAtkTable *)instance;
    
    self->disposed = FALSE;
}

static void
my_atk_table_dispose (GObject *obj)
{
    MyAtkTable *self = (MyAtkTable *)obj;

    if (self->disposed) 
    {
        return;
    }
    
    /* Make sure dispose does not run twice. */
    self->disposed = TRUE;

    /* Chain up to the parent class */
    G_OBJECT_CLASS (parent_class_table)->dispose (obj);
}

static void
my_atk_table_finalize (GObject *obj)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (parent_class_table)->finalize (obj);
}

static void
my_atk_table_class_init (gpointer g_class, gpointer g_class_data)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
    MyAtkTableClass *klass = MY_ATK_TABLE_CLASS (g_class);

    gobject_class->dispose = my_atk_table_dispose;
    gobject_class->finalize = my_atk_table_finalize;

    parent_class_table = g_type_class_peek_parent (klass);
}

GType 
my_atk_table_get_type (void)
{
    static GType type = 0;
    if (type == 0) 
    {
        static const GTypeInfo info = 
        {
            sizeof (MyAtkTableClass),
            NULL,   /* base_init */
            NULL,   /* base_finalize */
            my_atk_table_class_init, /* class_init */
            NULL,   /* class_finalize */
            NULL,   /* class_data */
            sizeof (MyAtkTable),
            0,      /* n_preallocs */
            table_instance_init    /* instance_init */
        };
                
        static const GInterfaceInfo iface_info = 
        {
            (GInterfaceInitFunc) table_interface_init,    /* interface_init */
            NULL,                                       /* interface_finalize */
            NULL                                        /* interface_data */
        };
        type = g_type_register_static (MY_TYPE_ATK_OBJECT,
                                       "MyAtkTableType",
                                       &info, 0);
        g_type_add_interface_static (type,
                                     ATK_TYPE_TABLE,
                                     &iface_info);
    }
    return type;
}
