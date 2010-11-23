/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "atspi-private.h"
#include <string.h>

static void
atspi_action_interface_init (AtspiAction *action)
{
}

static void
atspi_component_interface_init (AtspiComponent *component)
{
}

atspi_editable_text_interface_init (AtspiEditableText *editable_text)
{
}

static void
atspi_image_interface_init (AtspiImage *image)
{
}

static void
atspi_selection_interface_init (AtspiSelection *selection)
{
}

static void
atspi_table_interface_init (AtspiTable *table)
{
}

static void
atspi_text_interface_init (AtspiText *text)
{
}

static void
atspi_value_interface_init (AtspiValue *value)
{
}

G_DEFINE_TYPE_WITH_CODE (AtspiAccessible, atspi_accessible, ATSPI_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_ACTION, atspi_action_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_COMPONENT, atspi_component_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_EDITABLE_TEXT, atspi_editable_text_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_IMAGE, atspi_image_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_SELECTION, atspi_selection_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_TABLE, atspi_table_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_TEXT, atspi_text_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_VALUE, atspi_value_interface_init))

static void
atspi_accessible_init (AtspiAccessible *accessible)
{
}

static void
atspi_accessible_finalize (GObject *obj)
{
  /*AtspiAccessible *accessible = ATSPI_ACCESSIBLE (obj); */

  /* TODO: Unref parent/children, etc. */
}

static void
atspi_accessible_class_init (AtspiAccessibleClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = atspi_accessible_finalize;
}

/* TODO: Generate following from spec? */
static const char *role_names [] =
{
  "invalid",
  "accel-label",
  "alert",
  "animation",
  "arrow",
  "calendar",
  "canvas",
  "check-box",
  "check-menu-item",
  "color-chooser",
  "column-header",
  "combo-box",
  "date-editor",
  "desktop-icon",
  "desktop-frame",
  "dial",
  "dialog",
  "directory-pane",
  "drawing-area",
  "file-chooser",
  "filler",
  "font-chooser",
  "frame",
  "glass-pane",
  "html-container",
  "icon",
  "image",
  "internalframe",
  "label",
  "layered-pane",
  "list",
  "list-item",
  "menu",
  "menu-bar",
  "menu-item",
  "option-pane",
  "page-tab",
  "page-tab-list",
  "panel",
  "password-text",
  "popup-menu",
  "progress-bar",
  "push-button",
  "radio-button",
  "radio-menu-item",
  "root-pane",
  "row-header",
  "scroll-bar",
  "scroll-pane",
  "separator",
  "slider",
  "spin-button",
  "split-pane",
  "statusbar",
  "table",
  "table-cell",
  "table-column-header",
  "table-row-header",
  "tear-off-menu-item",
  "terminal",
  "text",
  "toggle-button",
  "tool-bar",
  "tool-tip",
  "tree",
  "tree-table",
  "unknown",
  "viewport",
  "window",
  NULL,
  "header",
  "fooler",
  "paragraph",
  "ruler",
  "application",
  "autocomplete",
  "editbar",
  "embedded",
  "entry",
  "chart",
  "caption",
  "document_frame",
  "heading",
  "page",
  "section",
  "form",
  "redundant object",
  "link",
  "input method window"
};

#define MAX_ROLES (sizeof (role_names) / sizeof (char *))

/**
 * atspi_role_get_name
 * @role: an #AtspiAccessibleRole object to query.
 *
 * Get a localizeable string that indicates the name of an #AtspiAccessibleRole.
 * <em>DEPRECATED.</em>
 *
 * Returns: a localizable string name for an #AtspiAccessibleRole enumerated type.
 **/
gchar *
atspi_role_get_name (AtspiRole role)
{
  if (role < MAX_ROLES && role_names [(int) role])
    {
      return g_strdup (role_names [(int) role]);
    }
  else
    {
      return g_strdup ("");
    }
}

/**
 * atspi_accessible_get_name:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get the name of an #AtspiAccessible object.
 *
 * Returns: a UTF-8 string indicating the name of the #AtspiAccessible object.
 * or NULL on exception
 **/
gchar *
atspi_accessible_get_name (AtspiAccessible *obj, GError **error)
{
  g_return_val_if_fail (obj != NULL, NULL);
  if (!(obj->cached_properties & ATSPI_CACHE_NAME))
  {
    if (!_atspi_dbus_get_property (obj, atspi_interface_accessible, "Name", NULL, "s", &obj->name))
      return NULL;
    obj->cached_properties |= ATSPI_CACHE_NAME;
  }
  return g_strdup (obj->name);
}

/**
 * atspi_accessible_get_description:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get the description of an #AtspiAccessible object.
 *
 * Returns: a UTF-8 string describing the #AtspiAccessible object.
 * or NULL on exception
 **/
gchar *
atspi_accessible_get_description (AtspiAccessible *obj, GError **error)
{
  g_return_val_if_fail (obj != NULL, NULL);

  if (!(obj->cached_properties & ATSPI_CACHE_DESCRIPTION))
  {
    if (!_atspi_dbus_call (obj, atspi_interface_accessible, "GetDescription", NULL, "=>s", &obj->description))
      return NULL;
    obj->cached_properties |= ATSPI_CACHE_DESCRIPTION;
  }
  return g_strdup (obj->description);
}

const char *str_parent = "Parent";

/**
 * atspi_accessible_get_parent:
 * @obj: a pointer to the #AtspiAccessible object to query.
 *
 * Get an #AtspiAccessible object's parent container.
 *
 * Returns: (transfer full): a pointer to the #AtspiAccessible object which
 *          contains the given #AtspiAccessible instance, or NULL if the @obj
 *          has no parent container.
 *
 **/
AtspiAccessible *
atspi_accessible_get_parent (AtspiAccessible *obj, GError **error)
{
  g_return_val_if_fail (obj != NULL, NULL);

  if (!(obj->cached_properties & ATSPI_CACHE_PARENT))
  {
    DBusMessage *message, *reply;
    DBusMessageIter iter, iter_variant;
    message = dbus_message_new_method_call (obj->parent.app->bus_name,
                                            obj->parent.path,
                                            DBUS_INTERFACE_PROPERTIES, "Get");
    if (!message)
      return NULL;
    dbus_message_append_args (message, DBUS_TYPE_STRING, &atspi_interface_accessible,
                               DBUS_TYPE_STRING, &str_parent,
                              DBUS_TYPE_INVALID);
    reply = _atspi_dbus_send_with_reply_and_block (message);
    if (!reply ||
       (strcmp (dbus_message_get_signature (reply), "v") != 0))
      return NULL;
    dbus_message_iter_init (reply, &iter);
    dbus_message_iter_recurse (&iter, &iter_variant);
    obj->accessible_parent = _atspi_dbus_return_accessible_from_iter (&iter_variant);
    dbus_message_unref (reply);
    obj->cached_properties |= ATSPI_CACHE_PARENT;
  }
  if (!obj->accessible_parent)
    return NULL;
  return g_object_ref (obj->accessible_parent);
}

/**
 * atspi_accessible_get_child_count:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get the number of children contained by an #AtspiAccessible object.
 *
 * Returns: a #long indicating the number of #AtspiAccessible children
 *          contained by an #AtspiAccessible object. or -1 on exception
 *
 **/
gint
atspi_accessible_get_child_count (AtspiAccessible *obj, GError *error)
{
  g_return_val_if_fail (obj != NULL, -1);

  /* TODO: MANAGES_DESCENDANTS */
  return g_list_length (obj->children);
}

/**
 * atspi_accessible_get_child_at_index:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 * @child_index: a #long indicating which child is specified.
 *
 * Get the #AtspiAccessible child of an #AtspiAccessible object at a given index.
 *
 * Returns: (transfer full): a pointer to the #AtspiAccessible child object at
 * index @child_index. or NULL on exception
 **/
AtspiAccessible *
atspi_accessible_get_child_at_index (AtspiAccessible *obj,
                            gint    child_index,
                            GError **error)
{
  AtspiAccessible *child;

  g_return_val_if_fail (obj != NULL, NULL);

  /* TODO: ManagesDescendants */
  child = g_list_nth_data (obj->children, child_index);
  if (!child)
    return NULL;
  return g_object_ref (child);
}

/**
 * atspi_accessible_get_index_in_parent
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get the index of an #AtspiAccessible object in its containing #AtspiAccessible.
 *
 * Returns: a #glong indicating the index of the #AtspiAccessible object
 *          in its parent (i.e. containing) #AtspiAccessible instance,
 *          or -1 if @obj has no containing parent or on exception.
 **/
gint
atspi_accessible_get_index_in_parent (AtspiAccessible *obj, GError **error)
{
  GList *l;
  gint i;

  g_return_val_if_fail (obj != NULL, -1);
  if (!obj->accessible_parent) return -1;
  l = obj->accessible_parent->children;
  while (l)
  {
    if (l->data == obj) return i;
    l = g_list_next (l);
    i++;
  }
  return -1;
}

typedef struct
{
  dbus_uint32_t type;
  GArray *targets;
} Accessibility_Relation;

/**
 * atspi_accessible_get_relation_set:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get the set of #AtspiRelation objects which describe this #AtspiAccessible object's
 *       relationships with other #AtspiAccessible objects.
 *
 * Returns: an array of #AtspiAccessibleRelation pointers. or NULL on exception
 * TODO:: Annotate array type
 **/
GArray *
atspi_accessible_get_relation_set (AtspiAccessible *obj, GError **error)
{
  int i;
  int n_relations;
  GArray *relation_set;

  g_return_val_if_fail (obj != NULL, NULL);

  if (!_atspi_dbus_call (obj, atspi_interface_accessible, "GetRelationSet", error, "=>a(uao)", &relation_set))
    return NULL;

  return relation_set;
}

/**
 * atspi_accessible_get_role:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get the UI role of an #AtspiAccessible object.
 * A UTF-8 string describing this role can be obtained via atspi_accessible_getRoleName ().
 *
 * Returns: the #AtspiRole of the object.
 *
 **/
AtspiRole
atspi_accessible_get_role (AtspiAccessible *obj, GError **error)
{
  g_return_val_if_fail (obj != NULL, ATSPI_ROLE_INVALID);

  if (!(obj->cached_properties & ATSPI_CACHE_ROLE))
  {
    dbus_uint32_t role;
    /* TODO: Make this a property */
    if (_atspi_dbus_call (obj, atspi_interface_accessible, "GetRole", NULL, "=>u", &role))
    {
      obj->cached_properties |= ATSPI_CACHE_ROLE;
      obj->role = role;
    }
  }
  return obj->role;
}

/**
 * atspi_accessible_get_role_name:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get a UTF-8 string describing the role this object plays in the UI.
 * This method will return useful values for roles that fall outside the
 * enumeration used in atspi_accessible_getRole ().
 *
 * Returns: a UTF-8 string specifying the role of this #AtspiAccessible object.
 *
 **/
gchar *
atspi_accessible_get_role_name (AtspiAccessible *obj, GError **error)
{
  char *retval = NULL;

  g_return_val_if_fail (obj != NULL, g_strdup ("invalid"));

  _atspi_dbus_call (obj, atspi_interface_accessible, "GetRoleName", error, "=>s", &retval);

  return retval;
}

/**
 * atspi_accessible_get_localized_role_name:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get a UTF-8 string describing the (localized) role this object plays in the UI.
 * This method will return useful values for roles that fall outside the
 * enumeration used in atspi_accessible_getRole ().
 *
 * Returns: a UTF-8 string specifying the role of this #AtspiAccessible object.
 *
 **/
gchar *
atspi_accessible_get_localized_role_name (AtspiAccessible *obj, GError **error)
{
  char *retval = NULL;

  g_return_val_if_fail (obj != NULL, g_strdup ("invalid"));

  _atspi_dbus_call (obj, atspi_interface_accessible, "GetLocalizedRoleName", error, "=>s", &retval);

  return retval;
}

/**
 * atspi_accessible_get_state_set:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Gets the current state of an object.
 *
 * Returns: (transfer full): a pointer to an #AtspiStateSet representing the
 *          object's current state.
 **/
AtspiStateSet *
atspi_accessible_get_state_set (AtspiAccessible *obj)
{
  return obj->states;
}

/**
 * atspi_accessible_get_attributes:
 * @obj: The #AtspiAccessible being queried.
 *
 * Get the #AttributeSet representing any assigned 
 * name-value pair attributes or annotations for this object.
 * For typographic, textual, or textually-semantic attributes, see
 * atspi_text_get_attributes instead.
 *
 * Returns: (element-type gchar* gchar*) (transfer full): The name-value-pair
 *          attributes assigned to this object.
 */
GHashTable *
atspi_accessible_get_attributes (AtspiAccessible *obj, GError **error)
{
  DBusMessage *message;
  GHashTable *ret;

    g_return_val_if_fail (obj != NULL, NULL);

  message = _atspi_dbus_call_partial (obj, atspi_interface_accessible, "GetAttributes", error, "");
  ret = _atspi_dbus_hash_from_message (message);
  dbus_message_unref (message);
  return ret;
}

/**
 * atspi_accessible_get_attributes_as_array:
 * @obj: The #AtspiAccessible being queried.
 *
 * Get the #AttributeSet representing any assigned 
 * name-value pair attributes or annotations for this object.
 * For typographic, textual, or textually-semantic attributes, see
 * atspi_text_get_attributes_as_array instead.
 *
 * Returns: (element-type gchar*) (transfer full): The name-value-pair
 *          attributes assigned to this object.
 */
GArray *
atspi_accessible_get_attributes_as_array (AtspiAccessible *obj, GError **error)
{
  DBusMessage *message;
  GArray *ret;

    g_return_val_if_fail (obj != NULL, NULL);

  message = _atspi_dbus_call_partial (obj, atspi_interface_accessible, "GetAttributes", error, "");
  ret = _atspi_dbus_attribute_array_from_message (message);
  dbus_message_unref (message);
  return ret;
}

/**
 * atspi_accessible_get_host_application:
 * @obj: The #AtspiAccessible being queried.
 *
 * Get the containing #AtspiApplication for an object.
 *
 * Returns: (transfer full): the containing AtspiApplication instance for
 *          this object.
 */
AtspiAccessible *
atspi_accessible_get_host_application (AtspiAccessible *obj, GError **error)
{
  AtspiAccessible *parent;

  for (;;)
  {
    parent = atspi_accessible_get_parent (obj, NULL);
    if (!parent || parent == obj ||
        atspi_accessible_get_role (parent, NULL) == ATSPI_ROLE_DESKTOP_FRAME)
    return obj;
    obj = parent;
  }
}

/* Interface query methods */

/**
 * atspi_accessible_is_action:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiAction.
 *
 * Returns: #TRUE if @obj implements the #AtspiAction interface,
 *          #FALSE otherwise.
 **/
gboolean
atspi_accessible_is_action (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_action);
}

/**
 * atspi_accessible_is_application:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiApplication.
 *
 * Returns: #TRUE if @obj implements the #AtspiApplication interface,
 *          #FALSE otherwise.
 **/
gboolean
atspi_accessible_is_application (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_application);
}

/**                      
 * atspi_accessible_is_collection:                                                                                                                                                                          * @obj: a pointer to the #AtspiAccessible instance to query.                                                                                                                                          
 *                          
 * Query whether the specified #AtspiAccessible implements #AtspiCollection.    
 * Returns: #TRUE if @obj implements the #AtspiCollection interface,                                                                                                               
 *          #FALSE otherwise.
 **/
gboolean
atspi_accessible_is_collection (AtspiAccessible *obj)
{
#if 0
     g_warning ("Collections not implemented");
     return _atspi_accessible_is_a (obj,
			      atspi_interface_collection);
#else
     return FALSE;
#endif
}

/**
 * atspi_accessible_is_component:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiComponent.
 *
 * Returns: #TRUE if @obj implements the #AtspiComponent interface,
 *          #FALSE otherwise.
 **/
gboolean
atspi_accessible_is_component (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_component);
}

/**
 * atspi_accessible_is_document:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiDocument.
 *
 * Returns: #TRUE if @obj implements the #AtspiDocument interface,
 *          #FALSE otherwise.
 **/
gboolean
atspi_accessible_is_document (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_document);
}

/**
 * atspi_accessible_is_editable_text:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiEditableText.
 *
 * Returns: #TRUE if @obj implements the #AtspiEditableText interface,
 *          #FALSE otherwise.
 **/
gboolean
atspi_accessible_is_editable_text (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_editable_text);
}
                                                                                                                                                                        
/**
 * atspi_accessible_is_hypertext:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiHypertext.
 *
 * Returns: #TRUE if @obj implements the #AtspiHypertext interface,
 *          #FALSE otherwise.
 **/
gboolean
atspi_accessible_is_hypertext (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_hypertext);
}

/**
 * atspi_accessible_is_image:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiImage.
 *
 * Returns: #TRUE if @obj implements the #AtspiImage interface,
 *          #FALSE otherwise.
**/
gboolean
atspi_accessible_is_image (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_image);
}

/**
 * atspi_accessible_is_selection:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiSelection.
 *
 * Returns: #TRUE if @obj implements the #AtspiSelection interface,
 *          #FALSE otherwise.
**/
gboolean
atspi_accessible_is_selection (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_selection);
}

/**
 * atspi_accessible_is_table:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiTable.
 *
 * Returns: #TRUE if @obj implements the #AtspiTable interface,
 *          #FALSE otherwise.
**/
gboolean
atspi_accessible_is_table (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_table);
}

/**
 * atspi_accessible_is_streamable_content:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements
 *          #AtspiStreamableContent.
 *
 * Returns: #TRUE if @obj implements the #AtspiStreamableContent interface,
 *          #FALSE otherwise.
**/
gboolean
atspi_accessible_is_streamable_content (AtspiAccessible *obj)
{
#if 0
  return _atspi_accessible_is_a (obj,
			      atspi_interface_streamable_content);
#else
  g_warning ("Streamable content not implemented");
  return FALSE;
#endif
}

/**
 * atspi_accessible_is_text:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiText.
 *
 * Returns: #TRUE if @obj implements the #AtspiText interface,
 *          #FALSE otherwise.
**/
gboolean
atspi_accessible_is_text (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_text);
}

/**
 * atspi_accessible_is_value:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiValue.
 *
 * Returns: #TRUE if @obj implements the #AtspiValue interface,
 *          #FALSE otherwise.
**/
gboolean
atspi_accessible_is_value (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_value);
}

/**
 * atspi_accessible_get_application:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiApplication interface for an #AtspiAccessible.
 *
 * Returns: a pointer to an #AtspiApplication interface instance, or
 *          NULL if @obj does not implement #AtspiApplication.
 **/
AtspiApplication *
atspi_accessible_get_application (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_application) ?
          g_object_ref (ATSPI_ACTION (accessible)) : NULL);  
}

#if 0
/**
 * atspi_accessible_get_action:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiAction interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiAction interface
 *          instance, or NULL if @obj does not implement #AtspiAction.
 **/
AtspiAction *
atspi_accessible_get_action (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_action) ?
          accessible : NULL);  
}

/**
 * atspi_accessible_get_collection:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiCollection interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiCollection interface
 *          instance, or NULL if @obj does not implement #AtspiCollection.
 **/
AtspiCollection *
atspi_accessible_get_collection (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_collection) ?
          g_object_ref (ATSPI_COLLECTION (accessible)) : NULL);  
}
#endif

/**
 * atspi_accessible_get_component:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiComponent interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiComponent interface
 *          instance, or NULL if @obj does not implement #AtspiComponent.
 **/
AtspiComponent *
atspi_accessible_get_component (AtspiAccessible *obj)
{
  return (_atspi_accessible_is_a (obj, atspi_interface_component) ?
          g_object_ref (ATSPI_COMPONENT (obj)) : NULL);
}

/**
 * atspi_accessible_get_document:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiDocument interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiDocument interface
 *          instance, or NULL if @obj does not implement #AtspiDocument.
 **/
AtspiDocument *
atspi_accessible_get_document (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_document) ?
          g_object_ref (ATSPI_DOCUMENT (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_editable_text:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiEditableText interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiEditableText interface
 *          instance, or NULL if @obj does not implement #AtspiEditableText.
 **/
AtspiEditableText *
atspi_accessible_get_editable_text (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_editable_text) ?
          g_object_ref (ATSPI_EDITABLE_TEXT (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_hypertext:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiHypertext interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiHypertext interface
 *          instance, or NULL if @obj does not implement #AtspiHypertext.
 **/
AtspiHypertext *
atspi_accessible_get_hypertext (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_hypertext) ?
          g_object_ref (ATSPI_HYPERTEXT (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_image:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiImage interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiImage interface instance, or
 *          NULL if @obj does not implement #AtspiImage.
 **/
AtspiImage *
atspi_accessible_get_image (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_image) ?
          g_object_ref (ATSPI_IMAGE (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_selection:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiSelection interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiSelection interface
 *          instance, or NULL if @obj does not implement #AtspiSelection.
 **/
AtspiSelection *
atspi_accessible_get_selection (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_selection) ?
          g_object_ref (ATSPI_SELECTION (accessible)) : NULL);  
}

#if 0
/**
 * atspi_accessible_get_streamable_content:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiStreamableContent interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiStreamableContent interface
 *          instance, or NULL if @obj does not implement #AtspiStreamableContent.
 **/
AtspiStreamableContent *
atspi_accessible_get_streamable_content (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_streamable_content) ?
          accessible : NULL);  
}
#endif

/**
 * atspi_accessible_get_table:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiTable interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiTable interface instance, or
 *          NULL if @obj does not implement #AtspiTable.
 **/
AtspiTable *
atspi_accessible_get_table (AtspiAccessible *obj)
{
  return (_atspi_accessible_is_a (obj, atspi_interface_table) ?
          g_object_ref (ATSPI_TABLE (obj)) : NULL);  
}

/**
 * atspi_accessible_get_text:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiTable interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiText interface instance, or
 *          NULL if @obj does not implement #AtspiTable.
 **/
AtspiText *
atspi_accessible_get_text (AtspiAccessible *obj)
{
  return (_atspi_accessible_is_a (obj, atspi_interface_text) ?
          g_object_ref (ATSPI_TEXT (obj)) : NULL);
}

/**
 * atspi_accessible_get_value:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiTable interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiValue interface instance, or
 *          NULL if @obj does not implement #AtspiValue.
 **/
AtspiValue *
atspi_accessible_get_value (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_value) ?
          g_object_ref (ATSPI_VALUE (accessible)) : NULL);  
}

#if 0
static gboolean
cspi_init_relation_type_table (AccessibleRelationType *relation_type_table)
{
  int i;
  for (i = 0; i < Accessibility_RELATION_LAST_DEFINED; ++i)
    {
      relation_type_table [i] = SPI_RELATION_NULL;
    }
  relation_type_table [Accessibility_RELATION_NULL] = SPI_RELATION_NULL;
  relation_type_table [Accessibility_RELATION_LABEL_FOR] = SPI_RELATION_LABEL_FOR;
  relation_type_table [Accessibility_RELATION_LABELLED_BY] = SPI_RELATION_LABELED_BY;
  relation_type_table [Accessibility_RELATION_CONTROLLER_FOR] = SPI_RELATION_CONTROLLER_FOR;
  relation_type_table [Accessibility_RELATION_CONTROLLED_BY] = SPI_RELATION_CONTROLLED_BY;
  relation_type_table [Accessibility_RELATION_MEMBER_OF] = SPI_RELATION_MEMBER_OF;
  relation_type_table [Accessibility_RELATION_TOOLTIP_FOR] = SPI_RELATION_NULL;
  relation_type_table [Accessibility_RELATION_NODE_CHILD_OF] = SPI_RELATION_NODE_CHILD_OF;
  relation_type_table [Accessibility_RELATION_EXTENDED] = SPI_RELATION_EXTENDED;
  relation_type_table [Accessibility_RELATION_FLOWS_TO] = SPI_RELATION_FLOWS_TO;
  relation_type_table [Accessibility_RELATION_FLOWS_FROM] = SPI_RELATION_FLOWS_FROM;
  relation_type_table [Accessibility_RELATION_SUBWINDOW_OF] = SPI_RELATION_SUBWINDOW_OF;
  relation_type_table [Accessibility_RELATION_EMBEDS] = SPI_RELATION_EMBEDS;
  relation_type_table [Accessibility_RELATION_EMBEDDED_BY] = SPI_RELATION_EMBEDDED_BY;
  relation_type_table [Accessibility_RELATION_POPUP_FOR] = SPI_RELATION_POPUP_FOR;
  relation_type_table [Accessibility_RELATION_PARENT_WINDOW_OF] = SPI_RELATION_PARENT_WINDOW_OF;
  relation_type_table [Accessibility_RELATION_DESCRIBED_BY] = SPI_RELATION_DESCRIBED_BY;
  relation_type_table [Accessibility_RELATION_DESCRIPTION_FOR] = SPI_RELATION_DESCRIPTION_FOR;
  return TRUE;
}

static AccessibleRelationType
cspi_relation_type_from_spi_relation_type (Accessibility_RelationType type)
{
  /* array is sized according to IDL RelationType because IDL RelationTypes are the index */	
  static AccessibleRelationType cspi_relation_type_table [Accessibility_RELATION_LAST_DEFINED];
  static gboolean is_initialized = FALSE;
  AccessibleRelationType cspi_type;
  if (!is_initialized)
    {
      is_initialized = cspi_init_relation_type_table (cspi_relation_type_table);	    
    }
  if (type >= 0 && type < Accessibility_RELATION_LAST_DEFINED)
    {
      cspi_type = cspi_relation_type_table [type];	    
    }
  else
    {
      cspi_type = SPI_RELATION_NULL;
    }
  return cspi_type; 
}
/**
 * AccessibleRelation_getRelationType:
 * @obj: a pointer to the #AtspiAccessibleRelation object to query.
 *
 * Get the type of relationship represented by an #AtspiAccessibleRelation.
 *
 * Returns: an #AtspiAccessibleRelationType indicating the type of relation
 *         encapsulated in this #AtspiAccessibleRelation object.
 *
 **/
AccessibleRelationType
AccessibleRelation_getRelationType (AccessibleRelation *obj)
{
  cspi_return_val_if_fail (obj, SPI_RELATION_NULL);
  return cspi_relation_type_from_spi_relation_type (obj->type);
}

/**
 * AccessibleRelation_getNTargets:
 * @obj: a pointer to the #AtspiAccessibleRelation object to query.
 *
 * Get the number of objects which this relationship has as its
 *       target objects (the subject is the #AtspiAccessible from which this
 *       #AtspiAccessibleRelation originated).
 *
 * Returns: a short integer indicating how many target objects which the
 *       originating #AtspiAccessible object has the #AtspiAccessibleRelation
 *       relationship with.
 **/
int
AccessibleRelation_getNTargets (AccessibleRelation *obj)
{
  cspi_return_val_if_fail (obj, -1);
  return obj->targets->len;
}

/**
 * AccessibleRelation_getTarget:
 * @obj: a pointer to the #AtspiAccessibleRelation object to query.
 * @i: a (zero-index) integer indicating which (of possibly several) target is requested.
 *
 * Get the @i-th target of a specified #AtspiAccessibleRelation relationship.
 *
 * Returns: an #AtspiAccessible which is the @i-th object with which the
 *      originating #AtspiAccessible has relationship specified in the
 *      #AtspiAccessibleRelation object.
 *
 **/
Accessible *
AccessibleRelation_getTarget (AccessibleRelation *obj, int i)
{
  cspi_return_val_if_fail (obj, NULL);

  if (i < 0 || i >= obj->targets->len) return NULL;
  return cspi_object_add (
			 g_array_index (obj->targets, Accessible *, i));
}

/**
 * AccessibleStateSet_ref:
 * @obj: a pointer to the #AtspiAccessibleStateSet object on which to operate.
 *
 * Increment the reference count for an #AtspiAccessibleStateSet object.
 *
 **/
void
AccessibleStateSet_ref (AccessibleStateSet *obj)
{
  spi_state_set_cache_ref (obj);
}

/**
 * AccessibleStateSet_unref:
 * @obj: a pointer to the #AtspiAccessibleStateSet object on which to operate.
 *
 * Decrement the reference count for an #AtspiAccessibleStateSet object.
 *
 **/
void
AccessibleStateSet_unref (AccessibleStateSet *obj)
{
  spi_state_set_cache_unref (obj);
}

static Accessibility_StateType
spi_state_to_dbus (AccessibleState state)
{
#define MAP_STATE(a) \
  case SPI_STATE_##a: \
    return Accessibility_STATE_##a

  switch (state)
    {
      MAP_STATE (INVALID);
      MAP_STATE (ACTIVE);
      MAP_STATE (ARMED);
      MAP_STATE (BUSY);
      MAP_STATE (CHECKED);
      MAP_STATE (DEFUNCT);
      MAP_STATE (EDITABLE);
      MAP_STATE (ENABLED);
      MAP_STATE (EXPANDABLE);
      MAP_STATE (EXPANDED);
      MAP_STATE (FOCUSABLE);
      MAP_STATE (FOCUSED);
      MAP_STATE (HORIZONTAL);
      MAP_STATE (ICONIFIED);
      MAP_STATE (MODAL);
      MAP_STATE (MULTI_LINE);
      MAP_STATE (MULTISELECTABLE);
      MAP_STATE (OPAQUE);
      MAP_STATE (PRESSED);
      MAP_STATE (RESIZABLE);
      MAP_STATE (SELECTABLE);
      MAP_STATE (SELECTED);
      MAP_STATE (SENSITIVE);
      MAP_STATE (SHOWING);
      MAP_STATE (SINGLE_LINE);
      MAP_STATE (STALE);
      MAP_STATE (TRANSIENT);
      MAP_STATE (VERTICAL);
      MAP_STATE (VISIBLE);
      MAP_STATE (MANAGES_DESCENDANTS);
      MAP_STATE (INDETERMINATE);
      MAP_STATE (TRUNCATED);
      MAP_STATE (REQUIRED);
      MAP_STATE (INVALID_ENTRY);
      MAP_STATE (SUPPORTS_AUTOCOMPLETION);
      MAP_STATE (SELECTABLE_TEXT);
      MAP_STATE (IS_DEFAULT);
      MAP_STATE (VISITED);
    default:
      return Accessibility_STATE_INVALID;
  }
#undef MAP_STATE
}	      

/**
 * AccessibleStateSet_contains:
 * @obj: a pointer to the #AtspiAccessibleStateSet object on which to operate.
 * @state: an #AtspiAccessibleState for which the specified #AtspiAccessibleStateSet
 *       will be queried.
 *
 * Determine whether a given #AtspiAccessibleStateSet includes a given state; that is,
 *       whether @state is true for the stateset in question.
 *
 * Returns: #TRUE if @state is true/included in the given #AtspiAccessibleStateSet,
 *          otherwise #FALSE.
 *
 **/
gboolean
AccessibleStateSet_contains (AccessibleStateSet *obj,
			     AccessibleState state)
{
  return spi_state_set_cache_contains (obj, spi_state_to_dbus (state));
}

/**
 * AccessibleStateSet_add:
 * @obj: a pointer to the #AtspiAccessibleStateSet object on which to operate.
 * @state: an #AtspiAccessibleState to be added to the specified #AtspiAccessibleStateSet
 *
 * Add a particular #AtspiAccessibleState to an #AtspiAccessibleStateSet (i.e. set the
 *       given state to #TRUE in the stateset.
 *
 **/
void
AccessibleStateSet_add (AccessibleStateSet *obj,
			AccessibleState state)
{
  spi_state_set_cache_add (obj, spi_state_to_dbus (state));
}

/**
 * AccessibleStateSet_remove:
 * @obj: a pointer to the #AtspiAccessibleStateSet object on which to operate.
 * @state: an #AtspiAccessibleState to be removed from the specified #AtspiAccessibleStateSet
 *
 * Remove a particular #AtspiAccessibleState to an #AtspiAccessibleStateSet (i.e. set the
 *       given state to #FALSE in the stateset.)
 *
 **/
void
AccessibleStateSet_remove (AccessibleStateSet *obj,
			   AccessibleState state)
{
  spi_state_set_cache_remove (obj, spi_state_to_dbus (state));
}

/**
 * AccessibleStateSet_equals:
 * @obj: a pointer to the first #AtspiAccessibleStateSet object on which to operate.
 * @obj2: a pointer to the second #AtspiAccessibleStateSet object on which to operate.
 *
 * Determine whether two instances of #AtspiAccessibleStateSet are equivalent (i.e.
 *         consist of the same #AtspiAccessibleStates).  Useful for checking multiple
 *         state variables at once; construct the target state then compare against it.
 *
 * @see AccessibleStateSet_compare().
 *
 * Returns: #TRUE if the two #AtspiAccessibleStateSets are equivalent,
 *          otherwise #FALSE.
 *
 **/
gboolean
AccessibleStateSet_equals (AccessibleStateSet *obj,
                           AccessibleStateSet *obj2)
{
  gboolean   eq;
  AtkStateSet *cmp;

  if (obj == obj2)
    {
      return TRUE;
    }

  cmp = spi_state_set_cache_xor (obj, obj2);
  eq = spi_state_set_cache_is_empty (cmp);
  spi_state_set_cache_unref (cmp);

  return eq;
}

/**
 * AccessibleStateSet_compare:
 * @obj: a pointer to the first #AtspiAccessibleStateSet object on which to operate.
 * @obj2: a pointer to the second #AtspiAccessibleStateSet object on which to operate.
 *
 * Determine the differences between two instances of #AtspiAccessibleStateSet.
 * Not Yet Implemented.
 *.
 * @see AccessibleStateSet_equals().
 *
 * Returns: an #AtspiAccessibleStateSet object containing all states contained on one of
 *          the two sets but not the other.
 *
 **/
AccessibleStateSet *
AccessibleStateSet_compare (AccessibleStateSet *obj,
                            AccessibleStateSet *obj2)
{
  return spi_state_set_cache_xor (obj, obj2);
}

/**
 * AccessibleStateSet_isEmpty:
 * @obj: a pointer to the #AtspiAccessibleStateSet object on which to operate.
 *
 * Determine whether a given #AtspiAccessibleStateSet is the empty set.
 *
 * Returns: #TRUE if the given #AtspiAccessibleStateSet contains no (true) states,
 *          otherwise #FALSE.
 *
 **/
gboolean
AccessibleStateSet_isEmpty (AccessibleStateSet *obj)
{
  return spi_state_set_cache_is_empty (obj);
}
#endif

gboolean
_atspi_accessible_is_a (AtspiAccessible *accessible,
		      const char *interface_name)
{
  int n;

  if (accessible == NULL)
    {
      return FALSE;
    }

  n = _atspi_get_iface_num (interface_name);
  if (n == -1) return FALSE;
  return (gboolean) ((accessible->interfaces & (1 << n))? TRUE: FALSE);
}

/* TODO: Move to a finalizer */
static void
cspi_object_destroyed (AtspiAccessible *accessible)
{
  gboolean cached;
  AtspiEvent e;

  /* TODO: Only fire if object not already marked defunct */
  memset (&e, 0, sizeof (e));
  e.type = "object:state-change:defunct";
  e.source = accessible;
  e.detail1 = 1;
  e.detail2 = 0;
  _atspi_send_event (&e);

    g_free (accessible->parent.path);

    if (accessible->states)
      g_object_unref (accessible->states);
    g_free (accessible->description);
    g_free (accessible->name);
}

AtspiAccessible *
atspi_accessible_new (AtspiApplication *app, const gchar *path)
{
  AtspiAccessible *accessible;
  
  accessible = g_object_new (ATSPI_TYPE_ACCESSIBLE, NULL);
  g_return_val_if_fail (accessible != NULL, NULL);

  accessible->parent.app = g_object_ref (app);
  accessible->parent.path = g_strdup (path);

  return accessible;
}
