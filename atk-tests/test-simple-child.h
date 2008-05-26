/* This file contains both declaration and definition of the TestSimpleChild class 
 * derived from AtkObject. The TestSimpleChild instances can be used as children by 
 * the TestSimpleSelection and TestSimpleTable instances.
 */

#ifndef TESTSIMPLECHILD_H_
#define TESTSIMPLECHILD_H_
#include <glib-object.h>
#include <atk/atk.h> 

#define TEST_TYPE_SIMPLE_CHILD             (test_simple_child_get_type ())
#define TEST_SIMPLE_CHILD(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEST_TYPE_SIMPLE_CHILD, TestSimpleChild))
#define TEST_SIMPLE_CHILD_CLASS(vtable)    (G_TYPE_CHECK_CLASS_CAST ((vtable), TEST_TYPE_SIMPLE_CHILD, TestSimpleChildClass))
#define TEST_IS_SIMPLE_CHILD(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TEST_TYPE_SIMPLE_CHILD))
#define TEST_IS_SIMPLE_CHILD_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), TEST_TYPE_SIMPLE_CHILD))
#define TEST_SIMPLE_CHILD_GET_CLASS(inst)  (G_TYPE_INSTANCE_GET_CLASS ((inst), TEST_TYPE_SIMPLE_CHILD, TestSimpleChildClass))

typedef struct _TestSimpleChild TestSimpleChild;
typedef struct _TestSimpleChildClass TestSimpleChildClass;

struct _TestSimpleChild 
{
    AtkObject parent;
    gboolean disposed;
	gint id;	/* unique object ID */
};

struct _TestSimpleChildClass 
{
	AtkObjectClass parent;
};

#endif /*TESTSIMPLECHILD_H_*/
