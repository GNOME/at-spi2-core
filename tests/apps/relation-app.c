#include <gmodule.h>
#include <atk/atk.h>
#include <my-atk.h>

static AtkObject *root_accessible;

G_MODULE_EXPORT void
test_init (gchar *path)
{
  AtkObject *r1, *r2, *r3;
  AtkObject *m1, *m2, *m3;
  AtkRelationSet *rset;
  AtkRelation *rel;
  AtkObject *rls[3];

  root_accessible  = g_object_new(MY_TYPE_ATK_OBJECT, NULL);
  
  r1 = ATK_OBJECT(g_object_new(MY_TYPE_ATK_OBJECT,
				  "accessible-name", "r1",
				  "accessible-description", "",
				  "accessible-role", ATK_ROLE_INVALID,
				  NULL));
  my_atk_object_add_child(MY_ATK_OBJECT(root_accessible), MY_ATK_OBJECT(r1));

  r2 = ATK_OBJECT(g_object_new(MY_TYPE_ATK_OBJECT,
				  "accessible-name", "r2",
				  "accessible-description", "",
				  "accessible-role", ATK_ROLE_INVALID,
				  NULL));
  my_atk_object_add_child(MY_ATK_OBJECT(root_accessible), MY_ATK_OBJECT(r2));

  r3 = ATK_OBJECT(g_object_new(MY_TYPE_ATK_OBJECT,
				  "accessible-name", "r3",
				  "accessible-description", "",
				  "accessible-role", ATK_ROLE_INVALID,
				  NULL));
  my_atk_object_add_child(MY_ATK_OBJECT(root_accessible), MY_ATK_OBJECT(r3));

  m1 = ATK_OBJECT(g_object_new(MY_TYPE_ATK_OBJECT,
				  "accessible-name", "m1",
				  "accessible-description", "",
				  "accessible-role", ATK_ROLE_INVALID,
				  NULL));
  my_atk_object_add_child(MY_ATK_OBJECT(root_accessible), MY_ATK_OBJECT(m1));

  m2 = ATK_OBJECT(g_object_new(MY_TYPE_ATK_OBJECT,
				  "accessible-name", "m2",
				  "accessible-description", "",
				  "accessible-role", ATK_ROLE_INVALID,
				  NULL));
  my_atk_object_add_child(MY_ATK_OBJECT(root_accessible), MY_ATK_OBJECT(m2));

  m3 = ATK_OBJECT(g_object_new(MY_TYPE_ATK_OBJECT,
				  "accessible-name", "m3",
				  "accessible-description", "",
				  "accessible-role", ATK_ROLE_INVALID,
				  NULL));
  my_atk_object_add_child(MY_ATK_OBJECT(root_accessible), MY_ATK_OBJECT(m3));

  atk_object_add_relationship(root_accessible, ATK_RELATION_EMBEDS, r1);
  atk_object_add_relationship(root_accessible, ATK_RELATION_PARENT_WINDOW_OF, r2);
  atk_object_add_relationship(root_accessible, ATK_RELATION_DESCRIBED_BY, r3);

  rls[0] = m1;
  rls[1] = m2;
  rls[2] = m3;

  rset = atk_object_ref_relation_set(root_accessible);
  rel = atk_relation_new(rls, 3, ATK_RELATION_POPUP_FOR);
  atk_relation_set_add(rset, rel);
  g_object_unref(G_OBJECT(rset));
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
  return root_accessible;
}
