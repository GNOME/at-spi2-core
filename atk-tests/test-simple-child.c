/* This file contains both declaration and definition of the TestSimpleChild class 
 * derived from AtkObject. The TestSimpleChild instances can be used as children by 
 * the TestSimpleSelection and TestSimpleTable instances.
 */

#include "test-simple-child.h"

GType 
test_simple_child_get_type (void);

static GObjectClass *parent_class_simple_child = NULL;

/******************************************************************/
/*                    Implementation                              */
/******************************************************************/
static void
simple_child_instance_init (GTypeInstance *instance, gpointer g_class)
{
	TestSimpleChild *self = (TestSimpleChild *)instance;
	
	self->disposed = FALSE;
	self->id = -1;	/* invalid ID value by default */
}

static void
test_simple_child_dispose (GObject *obj)
{
	TestSimpleChild *self = (TestSimpleChild *)obj;

	if (self->disposed) 
	{
			return;
	}
	
	/* Make sure dispose does not run twice. */
	self->disposed = TRUE;

	/* Chain up to the parent class */
	G_OBJECT_CLASS (parent_class_simple_child)->dispose (obj);
}

static void
test_simple_child_finalize (GObject *obj)
{
	TestSimpleChild *self = (TestSimpleChild *)obj;
	if (self)
	{
		/*free (self->pdata->descr);
		g_free (self->pdata);*/
	}
	
	/* Chain up to the parent class */
	G_OBJECT_CLASS (parent_class_simple_child)->finalize (obj);
}

static void
test_simple_child_class_init (gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	TestSimpleChildClass *klass = TEST_SIMPLE_CHILD_CLASS (g_class);

	gobject_class->dispose = test_simple_child_dispose;
	gobject_class->finalize = test_simple_child_finalize;

	parent_class_simple_child = g_type_class_peek_parent (klass);
}

GType 
test_simple_child_get_type (void)
{
	static GType type = 0;
	if (type == 0) 
	{
			static const GTypeInfo info = 
			{
					sizeof (TestSimpleChildClass),
					NULL,   /* base_init */
					NULL,   /* base_finalize */
					test_simple_child_class_init, /* class_init */
					NULL,   /* class_finalize */
					NULL,   /* class_data */
					sizeof (TestSimpleChild),
					0,      /* n_preallocs */
					simple_child_instance_init    /* instance_init */
			};

			type = g_type_register_static (ATK_TYPE_OBJECT,
										   "TestSimpleChildType",
										   &info, 0);
	}
	return type;
}
