/* ATK -  Accessibility Toolkit
 * Copyright 2001 Sun Microsystems Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <string.h>

#include <glib-object.h>

#include "atk.h"

/* New GObject properties registered by AtkObject */
enum
{
  PROP_0,  /* gobject convention */

  PROP_NAME,
  PROP_DESCRIPTION,
  PROP_PARENT,      /* ancestry has changed */
  PROP_CHILD,       /* a child has been added or removed */
  PROP_STATE,       /* AtkStateSet for the object has changed */
  PROP_TEXT,        /* Used only by AtkText implementors */
  PROP_CARET,       /* Used only by AtkText implementors */
  PROP_LAST         /* gobject convention */
};

enum {
  CHILDREN_CHANGED,
  FOCUS_EVENT,
  LAST_SIGNAL
};

static void            atk_object_class_init       (AtkObjectClass  *klass);
static void            atk_object_init             (AtkObject       *accessible,
                                                    AtkObjectClass  *klass);
static AtkRelationSet* atk_object_real_ref_relation_set (AtkObject *accessible);

static void            atk_object_real_set_property(GObject         *object,
                                                    guint            prop_id,
                                                    const GValue    *value,
                                                    GParamSpec      *pspec);
static void            atk_object_real_get_property(GObject         *object,
                                                    guint            prop_id,
                                                    GValue          *value,
                                                    GParamSpec      *pspec);
static void            atk_object_finalize         (GObject         *object);
static void            atk_object_real_set_role    (AtkObject       *object,
                                                    AtkRole         role);
static void            atk_object_focus_event_unimplemented
                                                   (AtkObject       *object,
                                                    gboolean        focus_in);

static guint atk_object_signals[LAST_SIGNAL] = { 0, };

static gpointer parent_class = NULL;

static const gchar* atk_object_name_property_name = "accessible-name";
static const gchar* atk_object_name_property_state = "accessible-state";
static const gchar* atk_object_name_property_description = "accessible-description";
static const gchar* atk_object_name_property_child = "accessible-child";
static const gchar* atk_object_name_property_parent = "accessible-parent";

GType
atk_object_get_type (void)
{
  static GType type = 0;

  if (!type)
    {
      static const GTypeInfo typeInfo =
      {
        sizeof (AtkObjectClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) atk_object_class_init,
        (GClassFinalizeFunc) NULL,
        NULL,
        sizeof (AtkObject),
        0,
        (GInstanceInitFunc) atk_object_init,
      } ;
      type = g_type_register_static (G_TYPE_OBJECT, "AtkObject", &typeInfo, 0) ;
    }
  return type;
}

static void
atk_object_class_init (AtkObjectClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  gobject_class->set_property = atk_object_real_set_property;
  gobject_class->get_property = atk_object_real_get_property;
  gobject_class->finalize = atk_object_finalize;

  klass->ref_relation_set = atk_object_real_ref_relation_set;
  klass->set_role = atk_object_real_set_role;
  klass->focus_event = atk_object_focus_event_unimplemented;

  klass->children_changed = NULL;

  g_object_class_install_property (gobject_class,
                                   PROP_NAME,
                                   g_param_spec_string (atk_object_name_property_name,
                                                        "Accessible Name",
                                                        "Object instance\'s name formatted for "
                                                           "assistive technology access",
                                                        NULL,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_DESCRIPTION,
                                   g_param_spec_string (atk_object_name_property_description,
                                                        "Accessible Description",
                                                        "Description of an object, formatted for "
                                                        "assistive technology access",
                                                        NULL,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_STATE,
                                   g_param_spec_int    (atk_object_name_property_state,
                                                        "Accessible State Set",
                                                        "The accessible state set of this object "
                                                        "or its UI component",
                                                        0,
                                                        G_MAXINT,
                                                        0,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_CHILD,
                                   g_param_spec_object (atk_object_name_property_child,
                                                        "Accessible Child",
                                                        "Is used to notify that a child has been added or removed ",
                                                        ATK_TYPE_OBJECT,
                                                        G_PARAM_READWRITE));
   g_object_class_install_property (gobject_class,
                                    PROP_PARENT,
                                   g_param_spec_object (atk_object_name_property_parent,
                                                        "Accessible Parent",
                                                        "Is used to notify that the parent has been changed ",
                                                        ATK_TYPE_OBJECT,
                                                        G_PARAM_READWRITE));
#if 0
  g_object_class_install_property (gobject_class,
                                   PROP_TEXT,
                                   g_param_spec_ccallback ("accessible_text", "Accessible Text",
                                                           "This object\'s accessible text contents",
                                                           G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_CARET,
                                   g_param_spec_ccallback ("accessible_caret", "Accessible Text Caret",
                                                           "The current text caret state and position "
                                                           "for this component",
                                                           G_PARAM_READWRITE));
#endif
  atk_object_signals[CHILDREN_CHANGED] =
    g_signal_newc ("accessible_children_changed",
                   G_TYPE_FROM_CLASS (klass),
                   G_SIGNAL_RUN_LAST,
                   G_STRUCT_OFFSET (AtkObjectClass, children_changed), /* still need to declare and define this func */
                   NULL, NULL,
                   g_cclosure_marshal_VOID__UINT_POINTER,
                   G_TYPE_NONE,
                   2, G_TYPE_UINT, ATK_TYPE_OBJECT);
  atk_object_signals[FOCUS_EVENT] =
    g_signal_newc ("focus_event",
                   G_TYPE_FROM_CLASS (klass),
                   G_SIGNAL_RUN_LAST,
                   G_STRUCT_OFFSET (AtkObjectClass, focus_event), 
                   NULL, NULL,
                   g_cclosure_marshal_VOID__BOOLEAN,
                   G_TYPE_NONE,
                   1, G_TYPE_BOOLEAN);
}

static void
atk_object_init  (AtkObject        *accessible,
                  AtkObjectClass   *klass)
{
}

GType
atk_implementor_get_type (void)
{
  static GType type = 0;

  if (!type)
    {
      static const GTypeInfo typeInfo =
      {
        sizeof (AtkImplementorIface),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
      } ;

      type = g_type_register_static (G_TYPE_INTERFACE, "AtkImplementorIface", &typeInfo, 0) ;
    }

  return type;
}

/**
 * atk_object_get_name
 * @accessible: a #AtkObject
 * return values: a character string representing the accessible name of the object.
 *
 * Gets the accessible name of the accessible
 **/
G_CONST_RETURN gchar*
atk_object_get_name (AtkObject *accessible)
{
  AtkObjectClass *klass;

  g_return_val_if_fail (accessible != NULL, NULL);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), NULL);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->get_name)
    return (klass->get_name) (accessible);
  else
    return NULL;
}

/**
 * atk_object_get_description
 * @accessible: a #AtkObject
 * return values: a character string representing the accessible description of the accessible.
 *
 * Gets the accessible description of the accessible
 **/
G_CONST_RETURN gchar*
atk_object_get_description (AtkObject *accessible)
{
  AtkObjectClass *klass;

  g_return_val_if_fail (accessible != NULL, NULL);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), NULL);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->get_description)
    return (klass->get_description) (accessible);
  else
    return NULL;
}

/**
 * atk_object_get_parent
 * @accessible: a #AtkObject
 * return values: a #AtkObject representing the accessible parent of the accessible.
 *
 * Gets the accessible description of the accessible
 **/
AtkObject*
atk_object_get_parent (AtkObject *accessible)
{
  AtkObjectClass *klass;

  g_return_val_if_fail (accessible != NULL, NULL);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), NULL);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->get_parent)
    return (klass->get_parent) (accessible);
  else
    return NULL;
}

/**
 * atk_object_get_n_accessible_children
 * @accessible: a #AtkObject
 * return values: a gint representing the number of accessible children of the accessible.
 *
 * Gets the number of accessible children of the accessible
 **/
gint
atk_object_get_n_accessible_children (AtkObject *accessible)
{
  AtkObjectClass *klass;

  g_return_val_if_fail (accessible != NULL, 0);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), 0);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->get_n_children)
    return (klass->get_n_children) (accessible);
  else
    return 0;
}

/**
 * atk_object_ref_accessible_child
 * @accessible: a #AtkObject
 * @i: a gint representing the position of the child, starting from 0
 * return values: a #AtkObject representing the specified accessible child of the accessible.
 *
 * Returns a reference to the specified accessible child of the object.
 * The accessible children are 0-based so the first accessible child is
 * at index 0, the second at index 1 and so on.
 **/
AtkObject*
atk_object_ref_accessible_child (AtkObject   *accessible,
                                 gint        i)
{
  AtkObjectClass *klass;

  g_return_val_if_fail (accessible != NULL, NULL);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), NULL);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->ref_child)
    return (klass->ref_child) (accessible, i);
  else
    return NULL;
}

/**
 * atk_object_ref_accessible_child
 * @accessible: a #AtkObject
 * return values: a #AtkRelationSet representing the relation set of the object.
 *
 * Returns a relation to the relation set associated of the accessible.
 **/
AtkRelationSet*
atk_object_ref_relation_set (AtkObject *accessible)
{
  AtkObjectClass *klass;

  g_return_val_if_fail (accessible != NULL, NULL);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), NULL);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->ref_relation_set)
    return (klass->ref_relation_set) (accessible);
  else
    return NULL;
}

/**
 * atk_role_register
 * @name: a character string describing the new role.
 * return values: a #AtkRole value for the new role.
 *
 * Returns a #AtkRole value for the new role.
 **/
AtkRole
atk_role_register (const gchar *name)
{
  /* TODO: associate name with new type */
  static guint type = ATK_ROLE_LAST_DEFINED;
  return (++type);
}

/**
 * atk_object_get_role
 * @accessible: a #AtkObject
 * return values: a #AtkRole which is the role of the accessible
 *
 * Gets the role of the accessible
 **/
AtkRole
atk_object_get_role (AtkObject *accessible) {
  AtkObjectClass *klass;

  g_return_val_if_fail (accessible != NULL, ATK_ROLE_UNKNOWN);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), ATK_ROLE_UNKNOWN);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->get_role)
    return (klass->get_role) (accessible);
  else
    return ATK_ROLE_UNKNOWN;
}

/**
 * atk_object_ref_state_set
 * @accessible: a #AtkObject
 * return values: a reference to a #AtkStateSet which is the state set of the accessible
 *
 * returns a reference to the state set of the accessible; the caller should
 * unreference it.
 **/
AtkStateSet*
atk_object_ref_state_set (AtkObject *accessible) {
  AtkObjectClass *klass;

  g_return_val_if_fail (accessible != NULL, NULL);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), NULL);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->ref_state_set)
    return (klass->ref_state_set) (accessible);
  else
    return NULL;
}

/**
 * atk_object_get_index_in_parent
 * @accessible: a #AtkObject
 * return values: a gint which is the index of the accessible in its parent
 *
 * Gets the 0-based index of this accessible in its parent; returns -1 if the
 * accessible does not have an accessible parent.
 **/
gint
atk_object_get_index_in_parent (AtkObject *accessible)
{
  AtkObjectClass *klass;

  g_return_val_if_fail (accessible != NULL, -1);
  g_return_val_if_fail (ATK_OBJECT (accessible), -1);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->get_index_in_parent)
    return (klass->get_index_in_parent) (accessible);
  else
    return -1;
}

/**
 * atk_object_set_name
 * @accessible: a #AtkObject
 * @name : a character string to be set as the accessible name
 *
 * Sets the accessible name of the accessible
 **/
void
atk_object_set_name (AtkObject    *accessible,
                     const gchar  *name)
{
  AtkObjectClass *klass;

  g_return_if_fail (accessible != NULL);
  g_return_if_fail (ATK_IS_OBJECT (accessible));
  g_return_if_fail (name != NULL);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->set_name)
  {
    (klass->set_name) (accessible, name);
    g_object_notify (G_OBJECT (accessible), atk_object_name_property_name);
  }
}

/**
 * atk_object_set_description
 * @accessible: a #AtkObject
 * @description : a character string to be set as the accessible description
 *
 * Sets the accessible description of the accessible
 **/
void
atk_object_set_description (AtkObject   *accessible,
                            const gchar *description)
{
  AtkObjectClass *klass;

  g_return_if_fail (accessible != NULL);
  g_return_if_fail (ATK_IS_OBJECT (accessible));
  g_return_if_fail (description != NULL);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->set_description)
  {
    (klass->set_description) (accessible, description);
    g_object_notify (G_OBJECT (accessible), atk_object_name_property_description);
  }
}

/**
 * atk_object_set_parent
 * @accessible: a #AtkObject
 * @parent : a #AtkObject to be set as the accessible parent
 *
 * Sets the accessible parent of the accessible
 **/
void
atk_object_set_parent (AtkObject *accessible,
                       AtkObject *parent)
{
  AtkObjectClass *klass;

  g_return_if_fail (accessible != NULL);
  g_return_if_fail (ATK_IS_OBJECT (accessible));

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->set_parent)
    (klass->set_parent) (accessible, parent);
}

/**
 * atk_object_set_role
 * @accessible: a #AtkObject
 * @role : a #AtkRole to be set as the role
 *
 * Sets the role of the accessible
 **/
void
atk_object_set_role (AtkObject *accessible, 
                     AtkRole   role)
{
  AtkObjectClass *klass;

  g_return_if_fail (accessible != NULL);
  g_return_if_fail (ATK_IS_OBJECT (accessible));

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->set_role)
    (klass->set_role) (accessible, role);
}

/**
 * atk_object_connect_property_change_handler
 * @accessible: a #AtkObject
 * @handler : a #AtkPropertyChangeHandler, a function to be called when a property changes its value
 * return values: a guint which is the handler id used in atk_object_remove_property_change_handler
 *
 * Specifies a function to be called when a property changes value.
 **/
guint
atk_object_connect_property_change_handler (AtkObject *accessible,
                                            AtkPropertyChangeHandler *handler)
{
  AtkObjectClass *klass;

  g_return_val_if_fail (accessible != NULL, 0);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), 0);
  g_return_val_if_fail ((handler != NULL), 0);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->connect_property_change_handler)
    return (klass->connect_property_change_handler) (accessible, handler);
  else
    return 0;
}

/**
 * atk_object_remove_property_change_handler
 * @accessible: a #AtkObject
 * @handler_id : a guint which identifies the handler to be removed.
 * 
 * Removes a property change handler.
 **/
void
atk_object_remove_property_change_handler  (AtkObject *accessible,
                                            guint      handler_id)
{
  AtkObjectClass *klass;

  g_return_if_fail (accessible != NULL);
  g_return_if_fail (ATK_IS_OBJECT (accessible));

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->remove_property_change_handler)
    (klass->remove_property_change_handler) (accessible, handler_id);
}

/**
 * atk_implementor_ref_accessible
 * @object: The GObject instance which should implement #AtkImplementorIface
 * if a non-null return value is required.
 * Return a reference to an object's AtkObject implementation, if
 * the object implements AtkObjectIface.
 */
AtkObject *
atk_implementor_ref_accessible (AtkImplementor *object)
{
  AtkImplementorIface *iface;
  AtkObject           *accessible = NULL;

  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (ATK_IS_IMPLEMENTOR (object), NULL);

  iface = ATK_IMPLEMENTOR_GET_IFACE (object);

  if (iface != NULL) 
    accessible =  iface->ref_accessible (object);

  g_return_val_if_fail ((accessible != NULL), NULL);

  return accessible;
}

static AtkRelationSet*
atk_object_real_ref_relation_set (AtkObject *accessible)
{
  if (accessible->relation_set)
    g_object_ref (accessible->relation_set); 

  return accessible->relation_set;
}

static void
atk_object_real_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  AtkObject *accessible;

  accessible = ATK_OBJECT (object);

  switch (prop_id)
  {
    case PROP_NAME:
      atk_object_set_name (accessible, g_value_get_string (value));
      break;
    case PROP_DESCRIPTION:
      atk_object_set_description (accessible, g_value_get_string (value));
      break;
    case PROP_STATE:
      g_print ("This interface does not support setting the state set of an accessible object\n");
      break;
    default:
      break;
  }
}

static void
atk_object_real_get_property (GObject      *object,
                              guint         prop_id,
                              GValue       *value,
                              GParamSpec   *pspec)
{
  AtkObject *accessible;

  accessible = ATK_OBJECT (object);

  switch (prop_id)
  {
    case PROP_NAME:
      g_value_set_string (value, atk_object_get_name (accessible));
      break;
    case PROP_DESCRIPTION:
      g_value_set_string (value, atk_object_get_description (accessible));
      break;
    default:
      break;
  }
}

static void
atk_object_finalize (GObject *object)
{
  AtkObject        *accessible;

  g_return_if_fail (ATK_IS_OBJECT (object));

  accessible = ATK_OBJECT (object);

  g_free (accessible->name);
  g_free (accessible->description);

  /*
   * Free memory allocated for relation set if it have been allocated.
   */
  if (accessible->relation_set)
  {
    g_object_unref (accessible->relation_set);
  }

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
atk_object_real_set_role (AtkObject *object,
                          AtkRole   role)
{
  object->role = role;
}

static void
atk_object_focus_event_unimplemented (AtkObject       *object,
                                      gboolean        focus_in)
{
  g_warning ("AtkObjectClass::focus_event not implemented for `%s'", 
              g_type_name (G_OBJECT_TYPE (object)));
}
