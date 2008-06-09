/* This file contains both declaration and definition of the MyAtkSelection,
 * a GObject that implements the AtkSelectionIface interface.
 */

#include <glib-object.h>
#include <atk/atk.h> 

#include "my-atk-object.h"
#include "my-atk-selection.h"

/******************************************************************/
/*                    Implementation                              */
/******************************************************************/
static GObjectClass *parent_class_simple_selection = NULL;

/* Implementation of the AtkSelectionIface interface. */
static gboolean
simple_selection_add_selection (AtkSelection *selection, gint i)
{
    MyAtkSelection* self = (MyAtkSelection*)selection;
    if ((!self) || self->disposed)
    {
        return FALSE;
    }
    
    if ((i >= 0) && (i < TEST_SELECTION_NCHILDREN))
    {
        /* If the child is not selected, select it and send the signal */
        if (!self->is_selected[i])
        {
            self->is_selected[i] = TRUE;
            g_signal_emit_by_name ((gpointer)self, "selection-changed");
        }
                
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
    
}

static gboolean 
simple_selection_clear_selection (AtkSelection *selection)
{
    MyAtkSelection* self = (MyAtkSelection*)selection;
    if ((!self) || self->disposed)
    {
        return FALSE;
    }
    
    /* clear selection */
    {
        gboolean changed = FALSE;
        int i;
        for (i = 0; i < TEST_SELECTION_NCHILDREN; ++i)
        {
            changed |= self->is_selected[i];
            self->is_selected[i] = FALSE;
        }
        
        if (changed)
        {
            g_signal_emit_by_name ((gpointer)self, "selection-changed");
        }
    }
    
    return TRUE;
}

static AtkObject* 
simple_selection_ref_selection (AtkSelection *selection, gint i)
{
    int pos;
    int nsel;
    MyAtkSelection* self = (MyAtkSelection*)selection;
    if ((!self) || self->disposed)
    {
        return NULL;
    }
    
    nsel = 0;
    for (pos = 0; pos < TEST_SELECTION_NCHILDREN; ++pos)
    {
        if (self->is_selected[pos])
        {
            if (i == nsel)
            {
                g_object_ref (G_OBJECT (self->child[pos]));
                return ATK_OBJECT(self->child[pos]);
            }
            ++nsel;
        }
    }
            
    return NULL;
}

static gint 
simple_selection_get_selection_count (AtkSelection *selection)
{
    MyAtkSelection* self = (MyAtkSelection*)selection;
    
    int cnt = 0;
    int i;
    
    if ((!self) || self->disposed)
    {
        return 0;
    }
    
    for (i = 0; i < TEST_SELECTION_NCHILDREN; ++i)
    {
        if (self->is_selected[i]) 
        {
            ++cnt;
        }
    }
    
     return cnt;
}

static gboolean 
simple_selection_is_child_selected (AtkSelection *selection, gint i)
{
    MyAtkSelection* self = (MyAtkSelection*)selection;
    if ((!self) || self->disposed)
    {
        return FALSE;
    }
    
    if ((i >= 0) && (i < TEST_SELECTION_NCHILDREN))
    {
        return (self->is_selected[i]);
    }
    else
    {
        return FALSE;
    }
}

static gboolean 
simple_selection_remove_selection (AtkSelection *selection, gint i)
{
    int pos;
    int nsel;
    
    MyAtkSelection* self = (MyAtkSelection*)selection;
    if ((!self) || self->disposed)
    {
        return FALSE;
    }
    
    nsel = 0;
    for (pos = 0; pos < TEST_SELECTION_NCHILDREN; ++pos)
    {
        if (self->is_selected[pos])
        {
            if (i == nsel)
            {
                self->is_selected[pos] = FALSE;
                g_signal_emit_by_name ((gpointer)self, "selection-changed");
                return TRUE;
            }
            ++nsel;
        }
    }
    
    return TRUE;
}

static gboolean 
simple_selection_select_all_selection (AtkSelection *selection)
{
    MyAtkSelection* self = (MyAtkSelection*)selection;
    if ((!self) || self->disposed)
    {
        return FALSE;
    }
    
    if (!self->multisel_supported)
    {
        return FALSE;
    }
    
    /* select all */
    {
        gboolean changed = FALSE;
        int i;
        for (i = 0; i < TEST_SELECTION_NCHILDREN; ++i)
        {
            changed |= !self->is_selected[i];
            self->is_selected[i] = TRUE;
        }
        
        if (changed)
        {
            g_signal_emit_by_name ((gpointer)self, "selection-changed");
        }
    }
    
    return TRUE;
}

/******************************************************************/
static void
simple_selection_interface_init (gpointer g_iface, gpointer iface_data)
{
    AtkSelectionIface *klass = (AtkSelectionIface *)g_iface;
    
    /* set up overrides here */
    klass->add_selection = 
        (gboolean (*) (AtkSelection *selection, gint i)) simple_selection_add_selection;
    
    klass->clear_selection = 
        (gboolean (*) (AtkSelection *selection)) simple_selection_clear_selection;
    
    klass->ref_selection = 
        (AtkObject* (*) (AtkSelection *selection, gint i)) simple_selection_ref_selection;
    
    klass->get_selection_count = 
        (gint (*) (AtkSelection *selection)) simple_selection_get_selection_count;
    
    klass->is_child_selected = 
        (gboolean (*) (AtkSelection *selection, gint i)) simple_selection_is_child_selected;
    
    klass->remove_selection = 
        (gboolean (*) (AtkSelection *selection, gint i)) simple_selection_remove_selection;
    
    klass->select_all_selection = 
        (gboolean (*) (AtkSelection *selection)) simple_selection_select_all_selection;
}

static void
simple_selection_instance_init (GTypeInstance *instance, gpointer g_class)
{
    MyAtkSelection *self = (MyAtkSelection *)instance;
    int i;
    
    self->disposed = FALSE;
    self->multisel_supported = TRUE;
    for (i = 0; i < TEST_SELECTION_NCHILDREN; ++i)
    {
        self->child[i] = MY_ATK_OBJECT (g_object_new (MY_TYPE_ATK_OBJECT, NULL));
        self->child[i]->id = i;
        self->is_selected[i] = FALSE; /* not selected by default */
    }
}

static void
my_atk_selection_dispose (GObject *obj)
{
    MyAtkSelection *self = (MyAtkSelection *)obj;
    int i;

    if (self->disposed) 
    {
        return;
    }
    
    /* Make sure dispose does not run twice. */
    self->disposed = TRUE;

    for (i = 0; i < TEST_SELECTION_NCHILDREN; ++i)
    {
        g_object_unref (G_OBJECT (self->child[i]));
    }

    /* Chain up to the parent class */
    G_OBJECT_CLASS (parent_class_simple_selection)->dispose (obj);
}

static void
my_atk_selection_finalize (GObject *obj)
{
    /*MyAtkSelection *self = (MyAtkSelection *)obj;
    if (self)
    {
    }*/
    
    /* Chain up to the parent class */
    G_OBJECT_CLASS (parent_class_simple_selection)->finalize (obj);
}

static void
my_atk_selection_class_init (gpointer g_class, gpointer g_class_data)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
    MyAtkSelectionClass *klass = MY_ATK_SELECTION_CLASS (g_class);

    gobject_class->dispose = my_atk_selection_dispose;
    gobject_class->finalize = my_atk_selection_finalize;

    parent_class_simple_selection = g_type_class_peek_parent (klass);
}

GType 
my_atk_selection_get_type (void)
{
    static GType type = 0;
    if (type == 0) 
    {
        static const GTypeInfo info = 
        {
            sizeof (MyAtkSelectionClass),
            NULL,   /* base_init */
            NULL,   /* base_finalize */
            my_atk_selection_class_init, /* class_init */
            NULL,   /* class_finalize */
            NULL,   /* class_data */
            sizeof (MyAtkSelection),
            0,      /* n_preallocs */
            simple_selection_instance_init    /* instance_init */
        };
                
        static const GInterfaceInfo iface_info = 
        {
            (GInterfaceInitFunc) simple_selection_interface_init,    /* interface_init */
            NULL,                                       /* interface_finalize */
            NULL                                        /* interface_data */
        };
        type = g_type_register_static (MY_TYPE_ATK_OBJECT,
                                       "MyAtkSelectionType",
                                       &info, 0);
        g_type_add_interface_static (type,
                                     ATK_TYPE_SELECTION,
                                     &iface_info);
    }
    return type;
}

