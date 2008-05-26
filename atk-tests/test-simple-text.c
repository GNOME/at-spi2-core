/* This file contains both declaration and definition of the TestSimpleText class,  
 * a simple wrapper for a text string not longer than MAX_DESCR_LENGTH symbols. 
 */

#include "test-simple-text.h"

GType 
test_simple_text_get_type (void);

static GObjectClass *parent_class_simple_text = NULL;

/******************************************************************/
/*                    Implementation                              */
/******************************************************************/
static void
simple_text_instance_init (GTypeInstance *instance, gpointer g_class)
{
    TestSimpleText *self = (TestSimpleText *)instance;
    
    self->disposed = FALSE;
    self->text = g_new (char, MAX_DESCR_LENGTH + 1);
    self->text[0] = '\0';
    
    //g_print ("\tCreate string\n");
}

static void
test_simple_text_dispose (GObject *obj)
{
    TestSimpleText *self = (TestSimpleText *)obj;

    if (self->disposed) 
    {
            return;
    }
    
    /* Make sure dispose does not run twice. */
    self->disposed = TRUE;
    
    g_free (self->text);
    self->text = NULL;
    //g_print ("\tDelete string\n");

    /* Chain up to the parent class */
    G_OBJECT_CLASS (parent_class_simple_text)->dispose (obj);
}

static void
test_simple_text_finalize (GObject *obj)
{
    TestSimpleText *self = (TestSimpleText *)obj;
    if (self)
    {
        /*free (self->pdata->descr);
        g_free (self->pdata);*/
    }
    
    /* Chain up to the parent class */
    G_OBJECT_CLASS (parent_class_simple_text)->finalize (obj);
}

static void
test_simple_text_class_init (gpointer g_class, gpointer g_class_data)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
    TestSimpleTextClass *klass = TEST_SIMPLE_TEXT_CLASS (g_class);

    gobject_class->dispose = test_simple_text_dispose;
    gobject_class->finalize = test_simple_text_finalize;

    parent_class_simple_text = g_type_class_peek_parent (klass);
}

GType 
test_simple_text_get_type (void)
{
    static GType type = 0;
    if (type == 0) 
    {
            static const GTypeInfo info = 
            {
                    sizeof (TestSimpleTextClass),
                    NULL,   /* base_init */
                    NULL,   /* base_finalize */
                    test_simple_text_class_init, /* class_init */
                    NULL,   /* class_finalize */
                    NULL,   /* class_data */
                    sizeof (TestSimpleText),
                    0,      /* n_preallocs */
                    simple_text_instance_init    /* instance_init */
            };

            type = g_type_register_static (ATK_TYPE_OBJECT,
                                           "TestSimpleTextType",
                                           &info, 0);
    }
    return type;
}
