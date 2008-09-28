#include <gmodule.h>
#include <atk/atk.h>
#include <my-atk.h>

static gchar *tdata_path = NULL;

static AtkComponent *comps[] = {NULL, NULL, NULL};
static const AtkRectangle extents[] = {{0,0,30,20}, {40,30,30,40}, {0,0,70,70}};
static const AtkLayer layers[] = {ATK_LAYER_WINDOW, ATK_LAYER_WIDGET, ATK_LAYER_MDI};
static const guint zorders[] = {0, -100, 100};
static const gboolean extent_may_changed[] = {TRUE, FALSE, TRUE};

G_MODULE_EXPORT void
test_init (gchar *path)
{
  int i;

  if (path == NULL)
     g_error("No test data path provided");
  tdata_path = path;

  g_type_init();
  for(i = 0; i < sizeof(comps) / sizeof(comps[0]); i++)
    {
      MyAtkComponent *mycomp = MY_ATK_COMPONENT(g_object_new(MY_TYPE_ATK_COMPONENT, NULL));
        
      mycomp->extent = extents[i];
      mycomp->is_extent_may_changed = extent_may_changed[i];
      mycomp->layer = layers[i];
      mycomp->zorder = zorders[i];
      
      comps[i] = ATK_COMPONENT(mycomp);
    }
    
  my_atk_object_add_child(MY_ATK_OBJECT(comps[2]), MY_ATK_OBJECT(comps[0]));
  my_atk_object_add_child(MY_ATK_OBJECT(comps[2]), MY_ATK_OBJECT(comps[1]));
}

G_MODULE_EXPORT void
test_next (int argc, char *argv[])
{
  g_print("Moving to next stage\n");
}

G_MODULE_EXPORT void
test_finished (int argc, char *argv[])
{
  g_print("Test has completed\n");
}

G_MODULE_EXPORT AtkObject *
test_get_root (void)
{
  return ATK_COMPONENT(comps[2]);
}
