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
  PROP_SELECTION,
  PROP_VALUE,
  PROP_VISIBLE_DATA,
  PROP_ROLE,
  PROP_TABLE_CAPTION,
  PROP_TABLE_COLUMN_DESCRIPTION,
  PROP_TABLE_COLUMN_HEADER,
  PROP_TABLE_ROW_DESCRIPTION,
  PROP_TABLE_ROW_HEADER,
  PROP_TABLE_SUMMARY,
  PROP_MODEL,
  PROP_LAST         /* gobject convention */
};

enum {
  CHILDREN_CHANGED,
  FOCUS_EVENT,
  PROPERTY_CHANGE,

  LAST_SIGNAL
};

static void            atk_object_class_init        (AtkObjectClass  *klass);
static void            atk_object_init              (AtkObject       *accessible,
                                                     AtkObjectClass  *klass);
static AtkRelationSet* atk_object_real_ref_relation_set 
                                                    (AtkObject       *accessible);

static void            atk_object_real_set_property (GObject         *object,
                                                     guint            prop_id,
                                                     const GValue    *value,
                                                     GParamSpec      *pspec);
static void            atk_object_real_get_property (GObject         *object,
                                                     guint            prop_id,
                                                     GValue          *value,
                                                     GParamSpec      *pspec);
static void            atk_object_finalize          (GObject         *object);
static G_CONST_RETURN gchar*
                       atk_object_real_get_name     (AtkObject       *object);
static G_CONST_RETURN gchar*
                       atk_object_real_get_description    
                                                   (AtkObject       *object);
static AtkObject*      atk_object_real_get_parent  (AtkObject       *object);
static AtkRole         atk_object_real_get_role    (AtkObject       *object);
static void            atk_object_real_set_name    (AtkObject       *object,
                                                    const gchar     *name);
static void            atk_object_real_set_description
                                                   (AtkObject       *object,
                                                    const gchar     *description);
static void            atk_object_real_set_parent  (AtkObject       *object,
                                                    AtkObject       *parent);
static void            atk_object_real_set_role    (AtkObject       *object,
                                                    AtkRole         role);
static guint           atk_object_real_connect_property_change_handler
                                                   (AtkObject       *obj,
                                                    AtkPropertyChangeHandler
                                                                    *handler);
static void            atk_object_real_remove_property_change_handler
                                                   (AtkObject       *obj,
                                                    guint           handler_id);
static void            atk_object_notify           (GObject         *obj,
                                                    GParamSpec      *pspec);


static guint atk_object_signals[LAST_SIGNAL] = { 0, };

static gpointer parent_class = NULL;

static const gchar* atk_object_name_property_name = "accessible-name";
static const gchar* atk_object_name_property_state = "accessible-state";
static const gchar* atk_object_name_property_description = "accessible-description";
static const gchar* atk_object_name_property_child = "accessible-child";
static const gchar* atk_object_name_property_parent = "accessible-parent";
static const gchar* atk_object_name_property_text = "accessible-text";
static const gchar* atk_object_name_property_caret = "accessible-caret";
static const gchar* atk_object_name_property_selection = "accessible-selection";
static const gchar* atk_object_name_property_value = "accessible-value";
static const gchar* atk_object_name_property_visible = "accessible-visible-data";
static const gchar* atk_object_name_property_role = "accessible-role";
static const gchar* atk_object_name_property_table_caption = "accessible-table-caption";
static const gchar* atk_object_name_property_table_column_description = "accessible-table-column-description";
static const gchar* atk_object_name_property_table_column_header = "accessible-table-column-header";
static const gchar* atk_object_name_property_table_row_description = "accessible-table-row-description";
static const gchar* atk_object_name_property_table_row_header = "accessible-table-row-header";
static const gchar* atk_object_name_property_table_summary = "accessible-table-summary";
static const gchar* atk_object_name_property_model = "accessible-model";

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
  gobject_class->notify = atk_object_notify;

  klass->get_name = atk_object_real_get_name;
  klass->get_description = atk_object_real_get_description;
  klass->get_parent = atk_object_real_get_parent;
  klass->get_n_children = NULL;
  klass->ref_child = NULL;
  klass->get_index_in_parent = NULL;
  klass->ref_relation_set = atk_object_real_ref_relation_set;
  klass->get_role = atk_object_real_get_role;
  klass->ref_state_set = NULL;
  klass->set_name = atk_object_real_set_name;
  klass->set_description = atk_object_real_set_description;
  klass->set_parent = atk_object_real_set_parent;
  klass->set_role = atk_object_real_set_role;
  klass->connect_property_change_handler = 
         atk_object_real_connect_property_change_handler;
  klass->remove_property_change_handler = 
         atk_object_real_remove_property_change_handler;

  /*
   * We do not define default signal handlers here
   */
  klass->children_changed = NULL;
  klass->focus_event = NULL;
  klass->property_change = NULL;

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
                                                        "Is used to notify that the parent has changed ",
                                                        ATK_TYPE_OBJECT,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_TEXT,
                                   g_param_spec_object (atk_object_name_property_text,
                                                        "Accessible Text",
                                                        "Is used to notify that the text has changed ",
                                                        ATK_TYPE_OBJECT,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_CARET,
                                   g_param_spec_int    (atk_object_name_property_caret,
                                                        "Accessible Caret",
                                                        "Is used to notify that the caret position has changed ",
                                                        0,
                                                        G_MAXINT,
                                                        0,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_SELECTION,
                                   g_param_spec_object (atk_object_name_property_selection,
                                                        "Accessible Selection",
                                                        "Is used to notify that the selection has changed ",
                                                        ATK_TYPE_OBJECT,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_VALUE,
                                   g_param_spec_double (atk_object_name_property_value,
                                                        "Accessible Value",
                                                        "Is used to notify that the value has changed ",
                                                        0.0,
                                                        G_MAXDOUBLE,
                                                        0.0,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_VISIBLE_DATA,
                                   g_param_spec_object (atk_object_name_property_visible,
                                                        "Accessible Visible Data",
                                                        "Is used to notify that the visual appearance of the object has changed ",
                                                        ATK_TYPE_OBJECT,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_ROLE,
                                   g_param_spec_int    (atk_object_name_property_role,
                                                        "Accessible Role",
                                                        "The accessible role this object ",
                                                        0,
                                                        G_MAXINT,
                                                        0,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_TABLE_CAPTION,
                                   g_param_spec_object (atk_object_name_property_table_caption,
                                                        "Accessible Table Caption",
                                                        "Is used to notify that the table caption has changed ",
                                                        ATK_TYPE_OBJECT,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_TABLE_COLUMN_HEADER,
                                   g_param_spec_object (atk_object_name_property_table_column_header,
                                                        "Accessible Table Column Header",
                                                        "Is used to notify that the table column header has changed ",
                                                        ATK_TYPE_OBJECT,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_TABLE_COLUMN_DESCRIPTION,
                                   g_param_spec_int    (atk_object_name_property_table_column_description,
                                                        "Accessible Table Column Description",
                                                        "Is used to notify that the table columnscription has changed ",
                                                        0,
                                                        G_MAXINT,
                                                        0,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_TABLE_ROW_HEADER,
                                   g_param_spec_object (atk_object_name_property_table_row_header,
                                                        "Accessible Table Row Header",
                                                        "Is used to notify that the table row header has changed ",
                                                        ATK_TYPE_OBJECT,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_TABLE_ROW_DESCRIPTION,
                                   g_param_spec_int    (atk_object_name_property_table_row_description,
                                                        "Accessible Table Row Description",
                                                        "Is used to notify that the table row description has changed ",
                                                        0,
                                                        G_MAXINT,
                                                        0,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_TABLE_SUMMARY,
                                   g_param_spec_object (atk_object_name_property_table_summary,
                                                        "Accessible Table Summary",
                                                        "Is used to notify that the table summary has changed ",
                                                        ATK_TYPE_OBJECT,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_MODEL,
                                   g_param_spec_object (atk_object_name_property_model,
                                                        "Accessible Model",
                                                        "Is used to notify that the model for Table or Tree has changed ",
                                                        ATK_TYPE_OBJECT,
                                                        G_PARAM_READWRITE));
  /*
   * The signal "children_changed" supports two details:
   * "add" and "remove"
   */
  atk_object_signals[CHILDREN_CHANGED] =
    g_signal_new ("children_changed",
		  G_TYPE_FROM_CLASS (klass),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
		  G_STRUCT_OFFSET (AtkObjectClass, children_changed),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__UINT_POINTER,
		  G_TYPE_NONE,
		  2, G_TYPE_UINT, G_TYPE_POINTER);
  atk_object_signals[FOCUS_EVENT] =
    g_signal_new ("focus_event",
		  G_TYPE_FROM_CLASS (klass),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (AtkObjectClass, focus_event), 
		  NULL, NULL,
		  g_cclosure_marshal_VOID__BOOLEAN,
		  G_TYPE_NONE,
		  1, G_TYPE_BOOLEAN);
  atk_object_signals[PROPERTY_CHANGE] =
    g_signal_new ("property_change",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (AtkObjectClass, property_change),
                  (GSignalAccumulator) NULL, NULL,
                  g_cclosure_marshal_VOID__POINTER,
                  G_TYPE_NONE, 1,
                  G_TYPE_POINTER);
}

static void
atk_object_init  (AtkObject        *accessible,
                  AtkObjectClass   *klass)
{
  accessible->name = NULL;
  accessible->description = NULL;
  accessible->accessible_parent = NULL;
  accessible->relation_set = NULL;
  accessible->role = ATK_ROLE_UNKNOWN;
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
 * atk_object_get_name:
 * @accessible: an #AtkObject
 *
 * Gets the accessible name of the accessible.
 *
 * Returns: a character string representing the accessible name of the object.
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
 * atk_object_get_description:
 * @accessible: an #AtkObject
 *
 * Gets the accessible description of the accessible.
 *
 * Returns: a character string representing the accessible description
 * of the accessible.
 *
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
 * atk_object_get_parent:
 * @accessible: an #AtkObject
 *
 * Gets the accessible parent of the accessible.
 *
 * Returns: a #AtkObject representing the accessible parent of the accessible
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
 * atk_object_get_n_accessible_children:
 * @accessible: an #AtkObject
 *
 * Gets the number of accessible children of the accessible.
 *
 * Returns: an integer representing the number of accessible children
 * of the accessible.
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
 * atk_object_ref_accessible_child:
 * @accessible: an #AtkObject
 * @i: a gint representing the position of the child, starting from 0
 *
 * Gets a reference to the specified accessible child of the object.
 * The accessible children are 0-based so the first accessible child is
 * at index 0, the second at index 1 and so on.
 *
 * Returns: an #AtkObject representing the specified accessible child
 * of the accessible.
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
 * atk_object_ref_relation_set:
 * @accessible: an #AtkObject
 *
 * Gets the #AtkRelationSet associated with the object.
 *
 * Returns: an #AtkRelationSet representing the relation set of the object.
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
 * atk_role_register:
 * @name: a character string describing the new role.
 *
 * Registers the role specified by @name.
 *
 * Returns: an #AtkRole for the new role.
 **/
AtkRole
atk_role_register (const gchar *name)
{
  /* TODO: associate name with new type */
  static guint type = ATK_ROLE_LAST_DEFINED;
  return (++type);
}

/**
 * atk_object_get_role:
 * @accessible: an #AtkObject
 *
 * Gets the role of the accessible.
 *
 * Returns: an #AtkRole which is the role of the accessible
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
 * atk_object_ref_state_set:
 * @accessible: an #AtkObject
 *
 * Gets a reference to the state set of the accessible; the caller must
 * unreference it when it is no longer needed.
 *
 * Returns: a reference to an #AtkStateSet which is the state
 * set of the accessible
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
 * atk_object_get_index_in_parent:
 * @accessible: an #AtkObject
 *
 * Gets the 0-based index of this accessible in its parent; returns -1 if the
 * accessible does not have an accessible parent.
 *
 * Returns: an integer which is the index of the accessible in its parent
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
 * atk_object_set_name:
 * @accessible: an #AtkObject
 * @name: a character string to be set as the accessible name
 *
 * Sets the accessible name of the accessible.
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
 * atk_object_set_description:
 * @accessible: an #AtkObject
 * @description : a character string to be set as the accessible description
 *
 * Sets the accessible description of the accessible.
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
 * atk_object_set_parent:
 * @accessible: an #AtkObject
 * @parent : an #AtkObject to be set as the accessible parent
 *
 * Sets the accessible parent of the accessible.
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
 * atk_object_set_role:
 * @accessible: an #AtkObject
 * @role : an #AtkRole to be set as the role
 *
 * Sets the role of the accessible.
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
 * atk_object_connect_property_change_handler:
 * @accessible: an #AtkObject
 * @handler : a function to be called when a property changes its value
 *
 * Specifies a function to be called when a property changes value.
 *
 * Returns: a #guint which is the handler id used in 
 * atk_object_remove_property_change_handler()
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
 * atk_object_remove_property_change_handler:
 * @accessible: an #AtkObject
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
 * atk_implementor_ref_accessible:
 * @implementor: The #GObject instance which should implement #AtkImplementorIface
 * if a non-null return value is required.
 * 
 * Gets a reference to an object's #AtkObject implementation, if
 * the object implements #AtkObjectIface
 *
 * Returns: a reference to an object's #AtkObject implementation
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
    case PROP_VALUE:
      if (ATK_IS_VALUE (accessible))
        atk_value_set_current_value (ATK_VALUE (accessible), value);
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
    case PROP_VALUE:
      if (ATK_IS_VALUE (accessible))
        atk_value_get_current_value (ATK_VALUE (accessible), value);
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

static G_CONST_RETURN gchar*
atk_object_real_get_name (AtkObject *object)
{
  return object->name;
}

static G_CONST_RETURN gchar*
atk_object_real_get_description (AtkObject *object)
{
  return object->description;
}

static AtkObject*
atk_object_real_get_parent (AtkObject       *object)
{
  return object->accessible_parent;
}

static AtkRole
atk_object_real_get_role (AtkObject       *object)
{
  return object->role;
}

static void
atk_object_real_set_name (AtkObject       *object,
                          const gchar     *name)
{
  g_free (object->name);
  object->name = g_strdup (name);
}

static void
atk_object_real_set_description (AtkObject       *object,
                                 const gchar     *description)
{
  g_free (object->description);
  object->description = g_strdup (description);
}

static void
atk_object_real_set_parent (AtkObject       *object,
                            AtkObject       *parent)
{
  object->accessible_parent = parent;

}

static void
atk_object_real_set_role (AtkObject *object,
                          AtkRole   role)
{
  object->role = role;
}

static guint
atk_object_real_connect_property_change_handler (AtkObject                *obj,
                                                 AtkPropertyChangeHandler *handler)
{
  return g_signal_connect_closure_by_id (obj,
                                         atk_object_signals[PROPERTY_CHANGE],
                                         0,
                                         g_cclosure_new (
                                         G_CALLBACK (handler), NULL,
                                         (GClosureNotify) NULL),
                                         FALSE);
}

static void
atk_object_real_remove_property_change_handler (AtkObject           *obj,
                                          guint               handler_id)
{
  g_signal_handler_disconnect (obj, handler_id);
}

/*
 * This function is a signal handler for notify signal which gets emitted
 * when a property changes value.
 *
 * It constructs an AtkPropertyValues structure and emits a "property_changed"
 * signal which causes the user specified AtkPropertyChangeHandler
 * to be called.
 */
static void
atk_object_notify (GObject     *obj,
                   GParamSpec  *pspec)
{
  AtkPropertyValues values;

  memset (&values.old_value, 0, sizeof (GValue));
  memset (&values.new_value, 0, sizeof (GValue));
  g_value_init (&values.new_value, pspec->value_type);
  g_object_get_property(obj, pspec->name, &values.new_value);
  values.property_name = pspec->name;
  g_signal_emit (obj, atk_object_signals[PROPERTY_CHANGE], 0,
                 &values, NULL);
}

