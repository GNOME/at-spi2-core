#include <stdio.h>
#include <string.h>
#include <atk/atk.h>

#include "my-atk-action.h"

static GObjectClass *parent_class = NULL;
//implementaion of the interface
static gboolean my_atk_action_do_action(AtkAction *action, gint i)
{
    MyAtkAction *self = (MyAtkAction*)action;
    gboolean result = (i>=0) && (i < self->n);
    self->last_performed_action = result? i : -1;
    return result;
}
static gint my_atk_action_get_n_actions(AtkAction *action)
{
    MyAtkAction *self = (MyAtkAction*)action;
    return self->n;
}
static const gchar* my_atk_action_get_description(AtkAction *action, gint i)
{
    MyAtkAction *self = (MyAtkAction*)action;
    if((i>=0) && (i<self->n))
    {
         return self->actions[i].description;
    }
    else
    {
        printf("get_description: Wrong index.\n");
        return NULL;
    }
}
static const gchar* my_atk_action_get_name(AtkAction *action, gint i)
{
    MyAtkAction *self = (MyAtkAction*)action;
    if((i >= 0) && (i < self->n))
    {
         return self->actions[i].name;
    }
    else
    {
        printf("get_name: Wrong index.\n");
        return NULL;
    }
}
static const gchar* my_atk_action_get_localized_name(AtkAction *action, gint i)
{
    return my_atk_action_get_name(action,i);
}

static const gchar* my_atk_action_get_keybinding(AtkAction *action, gint i)
{
    MyAtkAction *self = (MyAtkAction*)action;
    if((i >= 0) && (i < self->n))
    {
        gchar* keyb = self->actions[i].keybinding;
        if(keyb == NULL || keybinding_note_define == NULL)
        {
            //anywhere(if action has keybinding or not) NULL will return
            return NULL;
        }
        else
        {
            //verify, if string mean "no keybinding"
            return strcmp(keyb, keybinding_note_define) != 0 ? keyb : NULL;
        }
    }
    else
    {
        printf("get_keybinding: Wrong index.\n");
        return NULL;
    }
}
static gboolean my_atk_action_set_description(AtkAction *action, gint i, const gchar *desc)
{
    MyAtkAction *self = (MyAtkAction*)action;

    if(!((i >= 0) && (i < self->n)) )
    {
        //index out of range, but this is not application error according documentation
        return FALSE;
    }
    //index in correct range
    if(self->actions[i].description == desc)
    {
        //self assignment - return immediately
        return TRUE;
    }
    if(self->actions[i].description != NULL)
    {
        //free old value of description if it is not NULL
        free(self->actions[i].description);
    }
    if(desc != NULL)
    {
        //dump new value of description if it is not NULL
        self->actions[i].description = (gchar*)strdup((const char*)desc);
    }
    return TRUE;
}
//////////
static void my_atk_action_instance_init(GTypeInstance *instance, gpointer g_class)
{
  int i;
    MyAtkAction *self = (MyAtkAction*)instance;
    self->n = DEFAULT_NUMBER_ACTIONS;
    self->actions = g_new(struct OneAction, self->n);
    if(self->actions == NULL)
    {
        self->n = 0;
        return;
    }
    //init fields of action 0 with values which differ from others actions
    self->actions[0].name = (gchar*)strdup(FIRST_ACTION_NAME);
    self->actions[0].description = (gchar*)strdup(FIRST_ACTION_DESCRIPTION);
    self->actions[0].keybinding = (gchar*)strdup(FIRST_ACTION_KEYBINDING);

    for(i = 1; i < self->n; i++)
    {
        self->actions[i].name = (gchar*)strdup(DEFAULT_ACTION_NAME);
        self->actions[i].description = (gchar*)strdup(DEFAULT_ACTION_DESCRIPTION);
        self->actions[i].keybinding = (gchar*)strdup(DEFAULT_ACTION_KEYBINDING);
    }
    self->disposed = FALSE;
    self->last_performed_action = -1;
}

static void
my_atk_action_interface_init(gpointer g_iface, gpointer iface_data)
{
    AtkActionIface *klass = (AtkActionIface *)g_iface;

    klass->do_action = my_atk_action_do_action;
    klass->get_n_actions = my_atk_action_get_n_actions;
    klass->get_description = my_atk_action_get_description;
    klass->get_name = my_atk_action_get_name;
    klass->get_localized_name = my_atk_action_get_localized_name;
    klass->get_keybinding = my_atk_action_get_keybinding;
    klass->set_description = my_atk_action_set_description;
}

static void
my_atk_action_dispose(GObject *obj)
{
    MyAtkAction *self = (MyAtkAction*)obj;

    if(self->disposed)
    {
        return;
    }
    self->disposed = TRUE;

    G_OBJECT_CLASS(parent_class)->dispose(obj);
}

static void 
my_atk_action_finalize(GObject *obj)
{
    MyAtkAction *self = (MyAtkAction*)obj;
  int i;

    for(i = 0; i < self->n; i++)
    {
        struct OneAction oneAction = self->actions[i];
        if(oneAction.name != NULL)
            free(oneAction.name);
        if(oneAction.description != NULL)
            free(oneAction.description);
        if(oneAction.keybinding != NULL)
            free(oneAction.keybinding);
    }
    if(self->actions != NULL)
        g_free(self->actions);

    G_OBJECT_CLASS(parent_class)->finalize(obj);
}
static void
my_atk_action_class_init (gpointer g_class, gpointer g_class_data)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(g_class);
    MyAtkActionClass *klass = MY_ATK_ACTION_CLASS (g_class);

    gobject_class->dispose = my_atk_action_dispose;
    gobject_class->finalize = my_atk_action_finalize;

    parent_class = g_type_class_peek_parent(klass);
}
GType my_atk_action_get_type(void)
{
    static GType type = 0;
    if(type == 0)
    {
        static const GTypeInfo info =
        {
            sizeof (MyAtkActionClass),
            NULL,   /* base_init */
            NULL,   /* base_finalize */
            my_atk_action_class_init, /* class_init */
            NULL,   /* class_finalize */
            NULL,   /* class_data */
            sizeof (MyAtkAction),
            0,      /* n_preallocs */
            my_atk_action_instance_init    /* instance_init */
        };

        static const GInterfaceInfo iface_info = 
        {
            (GInterfaceInitFunc) my_atk_action_interface_init,    /* interface_init */
            NULL,                                       /* interface_finalize */
            NULL                                        /* interface_data */
        };
        type = g_type_register_static (G_TYPE_OBJECT,
            "MyAtkAction",
            &info, 0);
        g_type_add_interface_static (type,
            ATK_TYPE_ACTION,
            &iface_info);
    }
    return type;
}
