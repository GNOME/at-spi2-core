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
#include "atkmarshal.h"
#include "atk-enum-types.h"
#include "atkintl.h"

GPtrArray *extra_roles = NULL;

enum
{
  PROP_0,  /* gobject convention */

  PROP_NAME,
  PROP_DESCRIPTION,
  PROP_PARENT,      /* ancestry has changed */
  PROP_VALUE,
  PROP_ROLE,
  PROP_LAYER,
  PROP_MDI_ZORDER,
  PROP_TABLE_CAPTION,
  PROP_TABLE_COLUMN_DESCRIPTION,
  PROP_TABLE_COLUMN_HEADER,
  PROP_TABLE_ROW_DESCRIPTION,
  PROP_TABLE_ROW_HEADER,
  PROP_TABLE_SUMMARY,
  PROP_TABLE_CAPTION_OBJECT,
  PROP_LAST         /* gobject convention */
};

enum {
  CHILDREN_CHANGED,
  FOCUS_EVENT,
  PROPERTY_CHANGE,
  STATE_CHANGE,
  VISIBLE_DATA_CHANGED,
  ACTIVE_DESCENDANT_CHANGED,
  
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
static AtkLayer        atk_object_real_get_layer   (AtkObject       *object);
static AtkStateSet*    atk_object_real_ref_state_set
                                                   (AtkObject       *object);
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
static const gchar* atk_object_name_property_description = "accessible-description";
static const gchar* atk_object_name_property_parent = "accessible-parent";
static const gchar* atk_object_name_property_value = "accessible-value";
static const gchar* atk_object_name_property_role = "accessible-role";
static const gchar* atk_object_name_property_component_layer = "accessible-component-layer";
static const gchar* atk_object_name_property_component_mdi_zorder = "accessible-component-mdi-zorder";
static const gchar* atk_object_name_property_table_caption = "accessible-table-caption";
static const gchar* atk_object_name_property_table_column_description = "accessible-table-column-description";
static const gchar* atk_object_name_property_table_column_header = "accessible-table-column-header";
static const gchar* atk_object_name_property_table_row_description = "accessible-table-row-description";
static const gchar* atk_object_name_property_table_row_header = "accessible-table-row-header";
static const gchar* atk_object_name_property_table_summary = "accessible-table-summary";
static const gchar* atk_object_name_property_table_caption_object = "accessible-table-caption-object";

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

  parent_class = g_type_class_peek_parent (klass);

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
  klass->get_layer = atk_object_real_get_layer;
  klass->get_mdi_zorder = NULL;
  klass->ref_state_set = atk_object_real_ref_state_set;
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
  klass->visible_data_changed = NULL;
  klass->active_descendant_changed = NULL;

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
                                   PROP_PARENT,
                                   g_param_spec_object (atk_object_name_property_parent,
                                                        "Accessible Parent",
                                                        "Is used to notify that the parent has changed ",
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
                                   PROP_ROLE,
                                   g_param_spec_int    (atk_object_name_property_role,
                                                        "Accessible Role",
                                                        "The accessible role of this object ",
                                                        0,
                                                        G_MAXINT,
                                                        0,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_LAYER,
                                   g_param_spec_int    (atk_object_name_property_component_layer,
                                                        "Accessible Layer",
                                                        "The accessible layer of this object ",
                                                        0,
                                                        G_MAXINT,
                                                        0,
                                                        G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_MDI_ZORDER,
                                   g_param_spec_int    (atk_object_name_property_component_mdi_zorder,
                                                        "Accessible MDI Value",
                                                        "The accessible MDI value of this object ",
                                                        G_MININT,
                                                        G_MAXINT,
                                                        G_MININT,
                                                        G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_TABLE_CAPTION,
                                   g_param_spec_string (atk_object_name_property_table_caption,
                                                        "Accessible Table Caption",
                                                        "Is used to notify that the table caption has changed; this property should not be used. accessible-table-caption-object should be used instead",
                                                        NULL,
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
                                   g_param_spec_string (atk_object_name_property_table_column_description,
                                                        "Accessible Table Column Description",
                                                        "Is used to notify that the table columnscription has changed ",
                                                        NULL,
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
                                   g_param_spec_string (atk_object_name_property_table_row_description,
                                                        "Accessible Table Row Description",
                                                        "Is used to notify that the table row description has changed ",
                                                        NULL,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_TABLE_SUMMARY,
                                   g_param_spec_object (atk_object_name_property_table_summary,
                                                        "Accessible Table Summary",
                                                        "Is used to notify that the table summary has changed ",
                                                        ATK_TYPE_OBJECT,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_TABLE_CAPTION_OBJECT,
                                   g_param_spec_object (atk_object_name_property_table_caption_object,
                                                        "Accessible Table Caption Object",
                                                        "Is used to notify that the table caption has changed ",
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
                  G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                  G_STRUCT_OFFSET (AtkObjectClass, property_change),
                  (GSignalAccumulator) NULL, NULL,
                  g_cclosure_marshal_VOID__POINTER,
                  G_TYPE_NONE, 1,
                  G_TYPE_POINTER);
  /*
   * The "state_change" signal supports details, one for each accessible 
   * state type
   * (see atkstate.c).
   */
  atk_object_signals[STATE_CHANGE] =
    g_signal_new ("state_change",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                  G_STRUCT_OFFSET (AtkObjectClass, state_change),
                  (GSignalAccumulator) NULL, NULL,
                  atk_marshal_VOID__STRING_BOOLEAN,
                  G_TYPE_NONE, 2,
                  G_TYPE_STRING,
                  G_TYPE_BOOLEAN);
  atk_object_signals[VISIBLE_DATA_CHANGED] =
    g_signal_new ("visible_data_changed",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (AtkObjectClass, visible_data_changed),
                  (GSignalAccumulator) NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
  atk_object_signals[ACTIVE_DESCENDANT_CHANGED] =
    g_signal_new ("active_descendant_changed",
		  G_TYPE_FROM_CLASS (klass),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
		  G_STRUCT_OFFSET (AtkObjectClass, active_descendant_changed),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__POINTER,
		  G_TYPE_NONE,
		  1, G_TYPE_POINTER);
}

static void
atk_object_init  (AtkObject        *accessible,
                  AtkObjectClass   *klass)
{
  accessible->name = NULL;
  accessible->description = NULL;
  accessible->accessible_parent = NULL;
  accessible->relation_set = atk_relation_set_new();
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
  if (!extra_roles)
    extra_roles = g_ptr_array_new ();

  g_ptr_array_add (extra_roles, g_strdup (name));
  return extra_roles->len + ATK_ROLE_LAST_DEFINED;
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
atk_object_get_role (AtkObject *accessible) 
{
  AtkObjectClass *klass;

  g_return_val_if_fail (ATK_IS_OBJECT (accessible), ATK_ROLE_UNKNOWN);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->get_role)
    return (klass->get_role) (accessible);
  else
    return ATK_ROLE_UNKNOWN;
}

/**
 * atk_object_get_layer:
 * @accessible: an #AtkObject
 *
 * Gets the layer of the accessible.
 * DEPRECATED: use atk_component_get_layer instead!
 *
 * Returns: an #AtkLayer which is the layer of the accessible
 **/
AtkLayer
atk_object_get_layer (AtkObject *accessible) 
{
  AtkObjectClass *klass;

  g_return_val_if_fail (ATK_IS_OBJECT (accessible), ATK_LAYER_INVALID);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->get_layer)
    return (klass->get_layer) (accessible);
  else
    return ATK_LAYER_INVALID;
}

/**
 * atk_object_get_mdi_zorder:
 * @accessible: an #AtkObject
 *
 * Gets the zorder of the accessible. The value G_MININT will be returned 
 * if the layer of the accessible is not ATK_LAYER_MDI.
 * DEPRECATED: use atk_component_get_mdi_zorder instead!
 *
 * Returns: a gint which is the zorder of the accessible, i.e. the depth at 
 * which the component is shown in relation to other components in the same 
 * container.
 **/
gint
atk_object_get_mdi_zorder (AtkObject *accessible) 
{
  AtkObjectClass *klass;

  g_return_val_if_fail (ATK_IS_OBJECT (accessible), G_MININT);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->get_mdi_zorder)
    return (klass->get_mdi_zorder) (accessible);
  else
    return G_MININT;
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
atk_object_ref_state_set (AtkObject *accessible) 
{
  AtkObjectClass *klass;

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

  g_return_if_fail (ATK_IS_OBJECT (accessible));

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->set_parent)
    {
      (klass->set_parent) (accessible, parent);
      g_object_notify (G_OBJECT (accessible), atk_object_name_property_parent);
    }
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

  g_return_if_fail (ATK_IS_OBJECT (accessible));

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->set_role)
    {
      (klass->set_role) (accessible, role);
      g_object_notify (G_OBJECT (accessible), atk_object_name_property_role);
    }
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

  g_return_if_fail (ATK_IS_OBJECT (accessible));

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->remove_property_change_handler)
    (klass->remove_property_change_handler) (accessible, handler_id);
}

/**
 * atk_object_notify_state_change:
 * @accessible: an #AtkObject
 * @state: an #AtkState whose state is changed
 * @value : a gboolean which indicates whether the state is being set on or off
 * 
 * Emits a state-change signal for the specified state. 
 **/
void
atk_object_notify_state_change (AtkObject *accessible,
                                AtkState  state,
                                gboolean  value)
{
  G_CONST_RETURN gchar* name;

  name = atk_state_type_get_name (state);
  g_signal_emit (accessible, atk_object_signals[STATE_CHANGE],
                 g_quark_from_string (name),
                 name, value, NULL);
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
  g_return_val_if_fail (accessible->relation_set, NULL);
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
    case PROP_ROLE:
      atk_object_set_role (accessible, g_value_get_int (value));
      break;
    case PROP_PARENT:
      atk_object_set_parent (accessible, g_value_get_object (value));
      break;
    case PROP_VALUE:
      if (ATK_IS_VALUE (accessible))
        atk_value_set_current_value (ATK_VALUE (accessible), value);
      break;
    case PROP_TABLE_SUMMARY:
      if (ATK_IS_TABLE (accessible))
        atk_table_set_summary (ATK_TABLE (accessible), g_value_get_object (value));
      break;
    case PROP_TABLE_CAPTION_OBJECT:
      if (ATK_IS_TABLE (accessible))
        atk_table_set_caption (ATK_TABLE (accessible), g_value_get_object (value));
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
    case PROP_ROLE:
      g_value_set_int (value, atk_object_get_role (accessible));
      break;
    case PROP_LAYER:
      if (ATK_IS_COMPONENT (accessible))
        g_value_set_int (value, atk_component_get_layer (ATK_COMPONENT (accessible)));
      break;
    case PROP_MDI_ZORDER:
      if (ATK_IS_COMPONENT (accessible))
        g_value_set_int (value, atk_component_get_mdi_zorder (ATK_COMPONENT (accessible)));
      break;
    case PROP_PARENT:
      g_value_set_object (value, atk_object_get_parent (accessible));
      break;
    case PROP_VALUE:
      if (ATK_IS_VALUE (accessible))
        atk_value_get_current_value (ATK_VALUE (accessible), value);
      break;
    case PROP_TABLE_SUMMARY:
      if (ATK_IS_TABLE (accessible))
        g_value_set_object (value, atk_table_get_summary (ATK_TABLE (accessible)));
      break;
    case PROP_TABLE_CAPTION_OBJECT:
      if (ATK_IS_TABLE (accessible))
        g_value_set_object (value, atk_table_get_caption (ATK_TABLE (accessible)));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
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
    g_object_unref (accessible->relation_set);

  if (accessible->accessible_parent)
    g_object_unref (accessible->accessible_parent);

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

static AtkLayer
atk_object_real_get_layer (AtkObject       *object)
{
  return object->layer;
}

static AtkStateSet*
atk_object_real_ref_state_set (AtkObject *accessible) 
{
  AtkStateSet *state_set;
  AtkObject *ap;

  state_set = atk_state_set_new ();

  ap = atk_object_get_parent (accessible);
  if (ap)
    if (ATK_IS_SELECTION (ap))
      {
        int i;

        atk_state_set_add_state (state_set, ATK_STATE_SELECTABLE);

        i = atk_object_get_index_in_parent (accessible);
        if (i >= 0)
          if (atk_selection_is_child_selected(ATK_SELECTION (ap), i))
            atk_state_set_add_state (state_set, ATK_STATE_SELECTED);
      } 

  return state_set; 
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
  if (object->accessible_parent)
    g_object_unref (object->accessible_parent);

  object->accessible_parent = parent;
  if (object->accessible_parent)
    g_object_ref (object->accessible_parent);
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

/**
 * atk_object_initialize:
 * @accessible: a #AtkObject
 * @data: a #gpointer which identifies the object for which the AtkObject was created.
 *
 * This function is called when implementing subclasses of #AtkObject.
 * It does initialization required for the new object. It is intended
 * that this function should called only in the ..._new() functions used
 * to create an instance of a subclass of #AtkObject
 **/
void
atk_object_initialize (AtkObject  *accessible,
                       gpointer   data)
{
  AtkObjectClass *klass;

  g_return_if_fail (ATK_IS_OBJECT (accessible));

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->initialize)
    klass->initialize (accessible, data);
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
  AtkPropertyValues values = { 0, };

  g_value_init (&values.new_value, pspec->value_type);
  g_object_get_property (obj, pspec->name, &values.new_value);
  values.property_name = pspec->name;
  g_signal_emit (obj, atk_object_signals[PROPERTY_CHANGE],
                 g_quark_from_string (pspec->name),
                 &values, NULL);
  g_value_unset (&values.new_value);
}

/**
 * atk_role_get_name:
 * @role: The #AtkRole whose name is required
 *
 * Gets the description string describing the #AtkRole @role.
 *
 * Returns: the string describing the AtkRole
 */
G_CONST_RETURN gchar*
atk_role_get_name (AtkRole role)
{
  GTypeClass *type_class;
  GEnumValue *value;
  gchar *name = NULL;

  type_class = g_type_class_ref (ATK_TYPE_ROLE);
  g_return_val_if_fail (G_IS_ENUM_CLASS (type_class), NULL);

  value = g_enum_get_value (G_ENUM_CLASS (type_class), role);

  if (value)
    {
      name = value->value_nick;
    }
  else
    {
      if (extra_roles)
        {
          gint n = role;

          n -= ATK_ROLE_LAST_DEFINED + 1;

          if (n < extra_roles->len)
            name = g_ptr_array_index (extra_roles, n);
        }
    }
  g_type_class_unref (type_class);
  return name;
}

/**
 * atk_role_get_localized_name:
 * @role: The #AtkRole whose localized name is required
 *
 * Gets the localized description string describing the #AtkRole @role.
 *
 * Returns: the localized string describing the AtkRole
 **/
G_CONST_RETURN gchar*
atk_role_get_localized_name (AtkRole role)
{
  G_CONST_RETURN gchar *name;
  gboolean gettext_initialized = FALSE;

#ifdef ENABLE_NLS
  if (!gettext_initialized)
    {
      bindtextdomain (GETTEXT_PACKAGE, ATKLOCALEDIR);
#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
      bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif
    }
#endif

  switch (role)
    {
      case ATK_ROLE_INVALID:
        name = _("invalid");
        break;
      case ATK_ROLE_ACCEL_LABEL:
        name = _("accel-label");
        break;
      case ATK_ROLE_ALERT:
        name = _("alert");
        break;
      case ATK_ROLE_ANIMATION:
        name = _("animation");
        break;
      case ATK_ROLE_ARROW:
        name = _("arrow");
        break;
      case ATK_ROLE_CALENDAR:
        name = _("calendar");
        break;
      case ATK_ROLE_CANVAS:
        name = _("canvas");
        break;
      case ATK_ROLE_CHECK_BOX:
        name = _("check-box");
        break;
      case ATK_ROLE_CHECK_MENU_ITEM:
        name = _("check-menu-item");
        break;
      case ATK_ROLE_COLOR_CHOOSER:
        name = _("color-chooser");
        break;
      case ATK_ROLE_COLUMN_HEADER:
        name = _("column-header");
        break;
      case ATK_ROLE_COMBO_BOX:
        name = _("combo-box");
        break;
      case ATK_ROLE_DATE_EDITOR:
        name = _("date-editor");
        break;
      case ATK_ROLE_DESKTOP_ICON:
        name = _("desktop-icon");
        break;
      case ATK_ROLE_DESKTOP_FRAME:
        name = _("desktop-frame");
        break;
      case ATK_ROLE_DIAL:
        name = _("dial");
        break;
      case ATK_ROLE_DIALOG:
        name = _("dialog");
        break;
      case ATK_ROLE_DIRECTORY_PANE:
        name = _("directory-pane");
        break;
      case ATK_ROLE_DRAWING_AREA:
        name = _("drawing-area");
        break;
      case ATK_ROLE_FILE_CHOOSER:
        name = _("file-chooser");
        break;
      case ATK_ROLE_FILLER:
        name = _("filler");
        break;
      case ATK_ROLE_FONT_CHOOSER:
        name = _("font-chooser");
        break;
      case ATK_ROLE_FRAME:
        name = _("frame");
        break;
      case ATK_ROLE_GLASS_PANE:
        name = _("glass-pane");
        break;
      case ATK_ROLE_HTML_CONTAINER:
        name = _("html-container");
        break;
      case ATK_ROLE_ICON:
        name = _("icon");
        break;
      case ATK_ROLE_IMAGE:
        name = _("image");
        break;
      case ATK_ROLE_INTERNAL_FRAME:
        name = _("internal-frame");
        break;
      case ATK_ROLE_LABEL:
        name = _("label");
        break;
      case ATK_ROLE_LAYERED_PANE:
        name = _("layered-pane");
        break;
      case ATK_ROLE_LIST:
        name = _("list");
        break;
      case ATK_ROLE_LIST_ITEM:
        name = _("list-item");
        break;
      case ATK_ROLE_MENU:
        name = _("menu");
        break;
      case ATK_ROLE_MENU_BAR:
        name = _("menu-bar");
        break;
      case ATK_ROLE_MENU_ITEM:
        name = _("menu-item");
        break;
      case ATK_ROLE_OPTION_PANE:
        name = _("option-pane");
        break;
      case ATK_ROLE_PAGE_TAB:
        name = _("page-tab");
        break;
      case ATK_ROLE_PAGE_TAB_LIST:
        name = _("page-tab-list");
        break;
      case ATK_ROLE_PANEL:
        name = _("panel");
        break;
      case ATK_ROLE_PASSWORD_TEXT:
        name = _("password-text");
        break;
      case ATK_ROLE_POPUP_MENU:
        name = _("popup-menu");
        break;
      case ATK_ROLE_PROGRESS_BAR:
        name = _("progress-bar");
        break;
      case ATK_ROLE_PUSH_BUTTON:
        name = _("push-button");
        break;
      case ATK_ROLE_RADIO_BUTTON:
        name = _("radio-button");
        break;
      case ATK_ROLE_RADIO_MENU_ITEM:
        name = _("radio-menu-item");
        break;
      case ATK_ROLE_ROOT_PANE:
        name = _("root-pane");
        break;
      case ATK_ROLE_ROW_HEADER:
        name = _("row-header");
        break;
      case ATK_ROLE_SCROLL_BAR:
        name = _("scroll-bar");
        break;
      case ATK_ROLE_SCROLL_PANE:
        name = _("scroll-pane");
        break;
      case ATK_ROLE_SEPARATOR:
        name = _("separator");
        break;
      case ATK_ROLE_SLIDER:
        name = _("slider");
        break;
      case ATK_ROLE_SPLIT_PANE:
        name = _("split-pane");
        break;
      case ATK_ROLE_SPIN_BUTTON:
        name = _("spin-button");
        break;
      case ATK_ROLE_STATUSBAR:
        name = _("statusbar");
        break;
      case ATK_ROLE_TABLE:
        name = _("table");
        break;
      case ATK_ROLE_TABLE_CELL:
        name = _("table-cell");
        break;
      case ATK_ROLE_TABLE_COLUMN_HEADER:
        name = _("table-column-header");
        break;
      case ATK_ROLE_TABLE_ROW_HEADER:
        name = _("table-row-header");
        break;
      case ATK_ROLE_TEAR_OFF_MENU_ITEM:
        name = _("tear-off-menu-item");
        break;
      case ATK_ROLE_TERMINAL:
        name = _("terminal");
        break;
      case ATK_ROLE_TEXT:
        name = _("text");
        break;
      case ATK_ROLE_TOGGLE_BUTTON:
        name = _("toggle-button");
        break;
      case ATK_ROLE_TOOL_BAR:
        name = _("tool-bar");
        break;
      case ATK_ROLE_TOOL_TIP:
        name = _("tool-tip");
        break;
      case ATK_ROLE_TREE:
        name = _("tree");
        break;
      case ATK_ROLE_TREE_TABLE:
        name = _("tree-table");
        break;
      case ATK_ROLE_UNKNOWN:
        name = _("unknown");
        break;
      case ATK_ROLE_VIEWPORT:
        name = _("viewport");
        break;
      case ATK_ROLE_WINDOW:
        name = _("window");
        break;
      case ATK_ROLE_HEADER:
        name = _("header");
        break;
      case ATK_ROLE_FOOTER:
        name = _("footer");
        break;
      case ATK_ROLE_PARAGRAPH:
        name = _("paragraph");
        break;
      case ATK_ROLE_RULER:
        name = _("ruler");
        break;
      case ATK_ROLE_APPLICATION:
        name = _("application");
        break;
      default:
        name = atk_role_get_name (role);
        break;
    }
  return name;
}

/**
 * atk_role_for_name:
 * @name: a string which is the (non-localized) name of an ATK role.
 *
 * Get the #AtkRole type corresponding to a rolew name.
 *
 * Returns: the #AtkRole enumerated type corresponding to the specified
name,
 *          or #ATK_ROLE_INVALID if no matching role is found.
 **/
AtkRole
atk_role_for_name (const gchar *name)
{
  GTypeClass *type_class;
  GEnumValue *value;
  AtkRole role = ATK_ROLE_INVALID;

  g_return_val_if_fail (name, ATK_ROLE_INVALID);

  type_class = g_type_class_ref (ATK_TYPE_ROLE);
  g_return_val_if_fail (G_IS_ENUM_CLASS (type_class), ATK_ROLE_INVALID);

  value = g_enum_get_value_by_nick (G_ENUM_CLASS (type_class), name);

  if (value)
    {
      role = value->value;
    }
  else
    {
     gint i;

      if (extra_roles)
        {
          for (i = 0; i < extra_roles->len; i++)
            {
              gchar *extra_role = (gchar *)g_ptr_array_index (extra_roles, i);

              g_return_val_if_fail (extra_role, ATK_ROLE_INVALID);

              if (strcmp (name, extra_role) == 0)
                {
                  role = i + 1 + ATK_ROLE_LAST_DEFINED;
                  break;
                }
            }
        }
    }
  g_type_class_unref (type_class);
  
  return role;
}

/**
 * atk_object_add_relationship:
 * @object: The #AtkObject to which an AtkRelation is to be added. 
 * @relationship: The #AtkRelationType of the relation
 * @target: The #AtkObject which is to be the target of the relation.
 *
 * Adds a relationship of the specified type with the specified target.
 *
 * Returns TRUE if the relationship is added.
 **/
gboolean
atk_object_add_relationship (AtkObject       *object,
                             AtkRelationType relationship,
                             AtkObject       *target)
{
  AtkObject *array[1];
  AtkRelation *relation;

  g_return_val_if_fail (ATK_IS_OBJECT (object), FALSE);
  g_return_val_if_fail (ATK_IS_OBJECT (target), FALSE);

  array[0] = target;
  relation = atk_relation_new (array, 1, relationship);
  atk_relation_set_add (object->relation_set, relation);
  g_object_unref (relation);

  return TRUE;
}

/**
 * atk_object_remove_relationship:
 * @object: The #AtkObject from which an AtkRelation is to be removed. 
 * @relationship: The #AtkRelationType of the relation
 * @target: The #AtkObject which is the target of the relation to be removed.
 *
 * Removes a relationship of the specified type with the specified target.
 *
 * Returns TRUE if the relationship is removed.
 **/
gboolean
atk_object_remove_relationship (AtkObject       *object,
                                AtkRelationType relationship,
                                AtkObject       *target)
{
  gint n_relations, i;
  gboolean ret = FALSE;
  AtkRelation *relation;

  g_return_val_if_fail (ATK_IS_OBJECT (object), FALSE);
  g_return_val_if_fail (ATK_IS_OBJECT (target), FALSE);

  n_relations = atk_relation_set_get_n_relations (object->relation_set);
  for (i = 0; i < n_relations; i++)
    {
      relation = atk_relation_set_get_relation (object->relation_set, i);
      if (atk_relation_get_relation_type (relation) == relationship)
        {
          GPtrArray *array;

          array = atk_relation_get_target (relation);
        
          if (g_ptr_array_index (array, 0) == target)
            {
              atk_relation_set_remove (object->relation_set, relation); 
              ret = TRUE;
              break;
            }
        }
    }

  return ret;
}
