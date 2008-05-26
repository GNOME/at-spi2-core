/* This file contains both declaration and definition of the TestSimpleText class,  
 * a simple wrapper for a text string not longer than MAX_DESCR_LENGTH symbols. 
 */

#ifndef TESTSIMPLETEXT_H_
#define TESTSIMPLETEXT_H_
#include <glib-object.h>
#include <atk/atk.h> 

#define TEST_TYPE_SIMPLE_TEXT             (test_simple_text_get_type ())
#define TEST_SIMPLE_TEXT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEST_TYPE_SIMPLE_TEXT, TestSimpleText))
#define TEST_SIMPLE_TEXT_CLASS(vtable)    (G_TYPE_CHECK_CLASS_CAST ((vtable), TEST_TYPE_SIMPLE_TEXT, TestSimpleTextClass))
#define TEST_IS_SIMPLE_TEXT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TEST_TYPE_SIMPLE_TEXT))
#define TEST_IS_SIMPLE_TEXT_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), TEST_TYPE_SIMPLE_TEXT))
#define TEST_SIMPLE_TEXT_GET_CLASS(inst)  (G_TYPE_INSTANCE_GET_CLASS ((inst), TEST_TYPE_SIMPLE_TEXT, TestSimpleTextClass))

/* Maximum length of the descriptive text allowed. */
#define MAX_DESCR_LENGTH 1023

typedef struct _TestSimpleText TestSimpleText;
typedef struct _TestSimpleTextClass TestSimpleTextClass;

struct _TestSimpleText 
{
    AtkObject parent;
    gboolean disposed;
    
    gchar* text;     
};

struct _TestSimpleTextClass 
{
    AtkObjectClass parent;
};

#endif /*TESTSIMPLETEXT_H_*/
