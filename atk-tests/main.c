#include "atk/atk.h"
#include "hello.h"
#include "test-simple-table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

static AtkObject *root;
static GList *children;

gboolean quit(GIOChannel *source, GIOCondition condition, gpointer data)
{
  //g_signal_emit_by_name(root, "children-changed::remove", child);
  tcsetattr(0, TCSAFLUSH, data);
  exit(0);
}


// our parent class
static AtkObjectClass *parent_class = NULL;

/**************************************************************************/
/**************************************************************************/

#define GET_CLASS(instance) G_TYPE_INSTANCE_GET_CLASS (instance, AP_DOCVIEW_TYPE, ApDocViewClass)

static gint test_hello_get_n_children(AtkObject * obj)
{
printf("get n children: %p\n", obj);
  if (obj == root) return g_list_length(children);
  return 0;
}

static AtkStateSet *test_hello_ref_state_set(AtkObject * accessible)
{
    AtkStateSet *state_set;

    state_set = ATK_OBJECT_CLASS(parent_class)->ref_state_set(accessible);
    //atk_state_set_remove_state(state_set, ATK_STATE_FOCUSABLE);

    return state_set;
}

static void test_hello_real_initialize(AtkObject * obj, gpointer data)
{
  ATK_OBJECT_CLASS(parent_class)->initialize(obj, data);
  printf("Initialize: %p\n", obj);
}

static void test_hello_finalize(AtkObject * obj)
{
  printf("Finalizing: %p\n", obj);
}

static AtkObject *test_hello_ref_child(AtkObject * obj, gint i)
{
  gpointer data;
  printf("ref child: %p, index %d\n", obj, i);
  if (obj != root) return NULL;
  data = g_list_nth_data(children, i);
  if (data) return ATK_OBJECT(data);
  return NULL;
}

static void test_hello_class_init(test_helloClass * klass)
{
    AtkObjectClass *atk_class = ATK_OBJECT_CLASS(klass);

    parent_class = (AtkObjectClass *) g_type_class_peek_parent(klass);
    atk_class->ref_state_set = test_hello_ref_state_set;
    atk_class->get_n_children = test_hello_get_n_children;
    atk_class->ref_child = test_hello_ref_child;
    atk_class->initialize = test_hello_real_initialize;
}

static gint test_hello_get_caret_offset(AtkText * text)
{
  test_hello *obj = TEST_HELLO(text);
  return 0;
}

static void atk_text_interface_init (AtkTextIface * iface)
{
    g_return_if_fail (iface != NULL);
    iface->get_caret_offset = test_hello_get_caret_offset;
}

GType test_hello_get_type(void)
{
    static GType type = 0;

    if (!type)
    {
	static const GTypeInfo tinfo = {
	    sizeof(test_helloClass),
	    (GBaseInitFunc) NULL,	/* base init */
	    (GBaseFinalizeFunc) test_hello_finalize,	/* base finalize */
	    (GClassInitFunc) test_hello_class_init,	/* class init */
	    (GClassFinalizeFunc) NULL,	/* class finalize */
	    NULL,		/* class data */
	    sizeof(test_hello),	/* instance size */
	    0,			/* nb preallocs */
	    (GInstanceInitFunc) NULL,	/* instance init */
	    NULL		/* value table */
	};

	static const GInterfaceInfo atk_text_info = {
	    (GInterfaceInitFunc) atk_text_interface_init,
	    (GInterfaceFinalizeFunc) NULL,
	    NULL
	};

#if 0
	/* Following code would use the factory to fetch the parent type
	 * based on the atk type associated with GTK_TYPE_WIDGET */
	GType parent_type;
	AtkObjectFactory *factory;

	factory =
	    atk_registry_get_factory(atk_get_default_registry(),
				     GTK_TYPE_WIDGET);
	parent_type = atk_object_factory_get_accessible_type(factory);
#endif

	type = g_type_register_static(atk_object_get_type(),
				      "test_hello", &tinfo, (GTypeFlags) 0);
	g_type_add_interface_static (type, ATK_TYPE_TEXT, &atk_text_info);
    }
    return type;
}
static AtkObject *get_root()
{
  return root;
}

static void add_child(AtkObject *child)
{
  if (!child) return;
  children = g_list_append(children, child);
  g_signal_emit_by_name(root, "children-changed::add", child);
}

main(int argc, char *argv[])
{
  AtkUtilClass *klass;
  GMainLoop *mainloop;
  GIOChannel *stdin_channel;
  struct termios termios, rt;
  GModule *bridge;
  void (*gnome_accessibility_module_init)();
  AtkObject *table;

  g_type_init();
  klass = g_type_class_ref(ATK_TYPE_UTIL);
  klass->get_root = get_root;
  g_type_class_unref(klass);
  root = g_object_new(TEST_TYPE_HELLO, NULL);
  table = ATK_OBJECT(g_object_new(TEST_TYPE_SIMPLE_TABLE, NULL));
  add_child(table);
  /* The below line isn't really right -- normally gtk will build the path */
  bridge = g_module_open("/usr/lib/gtk-2.0/modules/libatk-bridge.so", G_MODULE_BIND_LOCAL|G_MODULE_BIND_LAZY);
  if (!bridge)
  {
    fprintf(stderr, "Couldn't load atk-bridge.so\n");
    exit(1);
  }
  if (!g_module_symbol(bridge, "gnome_accessibility_module_init", (gpointer *)&gnome_accessibility_module_init))
  {
    fprintf(stderr, "Couldn't find gnome_accessibility_module_init\n");    exit(1);
  }
  (*gnome_accessibility_module_init)();
  atk_object_set_name(root, "root object");

  mainloop = g_main_loop_new (NULL, FALSE);

  /* Make stdin "raw" so that we will see a key immediately */
  tcgetattr(0, &termios);
  rt = termios;
  cfmakeraw(&rt);
  tcsetattr(0, TCSAFLUSH, &rt);
  /* Set a watch on stdin so that we'll know when a key is pressed */
  stdin_channel = g_io_channel_unix_new(0);
  g_io_add_watch(stdin_channel, G_IO_IN, quit, &termios);

  g_main_loop_run (mainloop);
  return 0;
}
