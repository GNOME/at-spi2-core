/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 * Copyright 2010, 2011 Novell, Inc.
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
#include "atspi-accessible-private.h"
#include <string.h>

static gboolean enable_caching = FALSE;
static guint quark_locale;

static void
atspi_action_interface_init (AtspiAction *action)
{
}

static void
atspi_collection_interface_init (AtspiCollection *component)
{
}

static void
atspi_component_interface_init (AtspiComponent *component)
{
}

static void
atspi_document_interface_init (AtspiDocument *document)
{
}

static void
atspi_editable_text_interface_init (AtspiEditableText *editable_text)
{
}

static void
atspi_hypertext_interface_init (AtspiHypertext *hypertext)
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
atspi_table_cell_interface_init (AtspiTableCell *cell)
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
                         G_ADD_PRIVATE (AtspiAccessible)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_ACTION, atspi_action_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_COLLECTION, atspi_collection_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_COMPONENT, atspi_component_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_DOCUMENT, atspi_document_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_EDITABLE_TEXT, atspi_editable_text_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_HYPERTEXT, atspi_hypertext_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_IMAGE, atspi_image_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_SELECTION, atspi_selection_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_TABLE, atspi_table_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_TABLE_CELL, atspi_table_cell_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_TEXT, atspi_text_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_VALUE, atspi_value_interface_init))

#ifdef DEBUG_REF_COUNTS
static gint accessible_count = 0;
#endif

static void
atspi_accessible_init (AtspiAccessible *accessible)
{
#ifdef DEBUG_REF_COUNTS
  accessible_count++;
  g_hash_table_insert (_atspi_get_live_refs (), accessible, NULL);
  g_print("at-spi: init: %d objects\n", accessible_count);
#endif

  accessible->priv = atspi_accessible_get_instance_private (accessible);

  accessible->children = g_ptr_array_new_with_free_func (g_object_unref);
}

static void
atspi_accessible_dispose (GObject *object)
{
  AtspiAccessible *accessible = ATSPI_ACCESSIBLE (object);
  AtspiEvent e;
  AtspiAccessible *parent;
  gint i;

  /* TODO: Only fire if object not already marked defunct */
  memset (&e, 0, sizeof (e));
  e.type = "object:state-changed:defunct";
  e.source = accessible;
  e.detail1 = 1;
  e.detail2 = 0;
  _atspi_send_event (&e);

  g_clear_object (&accessible->states);

  parent = accessible->accessible_parent;
  if (parent)
  {
    accessible->accessible_parent = NULL;
    if (parent->children)
      g_ptr_array_remove (parent->children, accessible);
    g_object_unref (parent);
  }

  if (accessible->children) for (i = accessible->children->len - 1; i >= 0; i--)
  {
    AtspiAccessible *child = g_ptr_array_index (accessible->children, i);
    if (child && child->accessible_parent == accessible)
    {
      child->accessible_parent = NULL;
      g_object_unref (accessible);
    }
  }

  if (accessible->children)
  {
    g_ptr_array_free (accessible->children, TRUE);
    accessible->children = NULL;
  }

  G_OBJECT_CLASS (atspi_accessible_parent_class) ->dispose (object);
}

static void
atspi_accessible_finalize (GObject *object)
{
  AtspiAccessible *accessible = ATSPI_ACCESSIBLE (object);

    g_free (accessible->description);
    g_free (accessible->name);
  if (accessible->attributes)
    g_hash_table_unref (accessible->attributes);

    if (accessible->priv->cache)
      g_hash_table_destroy (accessible->priv->cache);

#ifdef DEBUG_REF_COUNTS
  accessible_count--;
  g_hash_table_remove (_atspi_get_live_refs (), accessible);
  g_print ("at-spi: finalize: %d objects\n", accessible_count);
#endif

  G_OBJECT_CLASS (atspi_accessible_parent_class)->finalize (object);
}

static void
atspi_accessible_class_init (AtspiAccessibleClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = atspi_accessible_dispose;
  object_class->finalize = atspi_accessible_finalize;

  quark_locale = g_quark_from_string ("accessible-locale");
}

/**
 * atspi_accessible_get_name:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Gets the name of an #AtspiAccessible object.
 *
 * Returns: a UTF-8 string indicating the name of the #AtspiAccessible object 
 * or NULL on exception.
 **/
gchar *
atspi_accessible_get_name (AtspiAccessible *obj, GError **error)
{
  g_return_val_if_fail (obj != NULL, g_strdup (""));
  if (!_atspi_accessible_test_cache (obj, ATSPI_CACHE_NAME))
  {
    if (!_atspi_dbus_get_property (obj, atspi_interface_accessible, "Name", error,
                                   "s", &obj->name))
      return g_strdup ("");
    _atspi_accessible_add_cache (obj, ATSPI_CACHE_NAME);
  }
  return g_strdup (obj->name);
}

/**
 * atspi_accessible_get_description:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Gets the description of an #AtspiAccessible object.
 *
 * Returns: a UTF-8 string describing the #AtspiAccessible object 
 * or NULL on exception.
 **/
gchar *
atspi_accessible_get_description (AtspiAccessible *obj, GError **error)
{
  g_return_val_if_fail (obj != NULL, g_strdup (""));

  if (!_atspi_accessible_test_cache (obj, ATSPI_CACHE_DESCRIPTION))
  {
    if (!_atspi_dbus_get_property (obj, atspi_interface_accessible,
                                   "Description", error, "s",
                                   &obj->description))
      return g_strdup ("");
    _atspi_accessible_add_cache (obj, ATSPI_CACHE_DESCRIPTION);
  }
  return g_strdup (obj->description);
}

const char *str_parent = "Parent";

/**
 * atspi_accessible_get_parent:
 * @obj: a pointer to the #AtspiAccessible object to query.
 *
 * Gets an #AtspiAccessible object's parent container.
 *
 * Returns: (nullable) (transfer full): a pointer to the
 *          #AtspiAccessible object which contains the given
 *          #AtspiAccessible instance, or NULL if the @obj has no
 *          parent container.
 *
 **/
AtspiAccessible *
atspi_accessible_get_parent (AtspiAccessible *obj, GError **error)
{
  g_return_val_if_fail (obj != NULL, NULL);

  if (obj->parent.app &&
      !_atspi_accessible_test_cache (obj, ATSPI_CACHE_PARENT))
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
    reply = _atspi_dbus_send_with_reply_and_block (message, error);
    if (!reply)
      return NULL;
    if (strcmp (dbus_message_get_signature (reply), "v") != 0)
    {
      dbus_message_unref (reply);
      return NULL;
    }
    dbus_message_iter_init (reply, &iter);
    dbus_message_iter_recurse (&iter, &iter_variant);
    obj->accessible_parent = _atspi_dbus_return_accessible_from_iter (&iter_variant);
    dbus_message_unref (reply);
    _atspi_accessible_add_cache (obj, ATSPI_CACHE_PARENT);
  }
  if (!obj->accessible_parent)
    return NULL;
  return g_object_ref (obj->accessible_parent);
}

/**
 * atspi_accessible_get_child_count:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Gets the number of children contained by an #AtspiAccessible object.
 *
 * Returns: a #long indicating the number of #AtspiAccessible children
 *          contained by an #AtspiAccessible object or -1 on exception.
 *
 **/
gint
atspi_accessible_get_child_count (AtspiAccessible *obj, GError **error)
{
  g_return_val_if_fail (obj != NULL, -1);

  if (!_atspi_accessible_test_cache (obj, ATSPI_CACHE_CHILDREN))
  {
    dbus_int32_t ret;
    if (!_atspi_dbus_get_property (obj, atspi_interface_accessible,
                                   "ChildCount", error, "i", &ret))
      return -1;
    return ret;
  }

  if (!obj->children)
    return 0;	/* assume it's disposed */

  return obj->children->len;
}

/**
 * atspi_accessible_get_child_at_index:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 * @child_index: a #long indicating which child is specified.
 *
 * Gets the #AtspiAccessible child of an #AtspiAccessible object at a given index.
 *
 * Returns: (transfer full): a pointer to the #AtspiAccessible child object at
 * index @child_index or NULL on exception.
 **/
AtspiAccessible *
atspi_accessible_get_child_at_index (AtspiAccessible *obj,
                            gint    child_index,
                            GError **error)
{
  AtspiAccessible *child;
  DBusMessage *reply;

  g_return_val_if_fail (obj != NULL, NULL);

  if (_atspi_accessible_test_cache (obj, ATSPI_CACHE_CHILDREN))
  {
    if (!obj->children)
      return NULL;	/* assume disposed */

    child = g_ptr_array_index (obj->children, child_index);
    if (child)
      return g_object_ref (child);
  }

  reply = _atspi_dbus_call_partial (obj, atspi_interface_accessible,
                                   "GetChildAtIndex", error, "i", child_index);
  child = _atspi_dbus_return_accessible_from_message (reply);

  if (!child)
    return NULL;

  if (_atspi_accessible_test_cache (obj, ATSPI_CACHE_CHILDREN))
  {
      if (child_index >= obj->children->len)
        g_ptr_array_set_size (obj->children, child_index + 1);
    g_ptr_array_index (obj->children, child_index) = g_object_ref (child);
  }
  return child;
}

/**
 * atspi_accessible_get_index_in_parent:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Gets the index of an #AtspiAccessible object within its parent's 
 * #AtspiAccessible children list.
 *
 * Returns: a #glong indicating the index of the #AtspiAccessible object
 *          in its parent,
 *          or -1 if @obj has no containing parent or on exception.
 **/
gint
atspi_accessible_get_index_in_parent (AtspiAccessible *obj, GError **error)
{
  gint i = 0;
  dbus_int32_t ret = -1;

  g_return_val_if_fail (obj != NULL, -1);
  if (_atspi_accessible_test_cache (obj, ATSPI_CACHE_PARENT))
  {
    if (!obj->accessible_parent)
      return -1;

    if (!_atspi_accessible_test_cache (obj->accessible_parent, ATSPI_CACHE_CHILDREN) || !obj->accessible_parent->children)
        goto dbus;

    for (i = 0; i < obj->accessible_parent->children->len; i++)
      if (g_ptr_array_index (obj->accessible_parent->children, i) == obj)
        return i;
  }

dbus:
  _atspi_dbus_call (obj, atspi_interface_accessible,
                    "GetIndexInParent", NULL, "=>i", &ret);
  return ret;
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
 * Gets the set of #AtspiRelation objects which describes this #AtspiAccessible object's
 * relationships with other #AtspiAccessible objects.
 *
 * Returns: (element-type AtspiRelation*) (transfer full): a #GArray of
 *          #AtspiRelation pointers or NULL on exception.
 **/
GArray *
atspi_accessible_get_relation_set (AtspiAccessible *obj, GError **error)
{
  DBusMessage *reply;
  DBusMessageIter iter, iter_array;
  GArray *ret;

  g_return_val_if_fail (obj != NULL, NULL);

  reply = _atspi_dbus_call_partial (obj, atspi_interface_accessible, "GetRelationSet", error, "");
  if (!reply)
    return NULL;
  _ATSPI_DBUS_CHECK_SIG (reply, "a(ua(so))", error, NULL);

  ret = g_array_new (TRUE, TRUE, sizeof (AtspiRelation *));
  dbus_message_iter_init (reply, &iter);
  dbus_message_iter_recurse (&iter, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
  {
    AtspiRelation *relation;
    relation = _atspi_relation_new_from_iter (&iter_array);
    ret = g_array_append_val (ret, relation);
    dbus_message_iter_next (&iter_array);
  }
  dbus_message_unref (reply);
  return ret;
}

/**
 * atspi_accessible_get_role:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Gets the UI role played by an #AtspiAccessible object.
 * This role's name can be obtained via atspi_accessible_get_role_name ().
 *
 * Returns: the #AtspiRole of an #AtspiAccessible object.
 *
 **/
AtspiRole
atspi_accessible_get_role (AtspiAccessible *obj, GError **error)
{
  g_return_val_if_fail (obj != NULL, ATSPI_ROLE_INVALID);

  if (!_atspi_accessible_test_cache (obj, ATSPI_CACHE_ROLE))
  {
    dbus_uint32_t role;
    /* TODO: Make this a property */
    if (_atspi_dbus_call (obj, atspi_interface_accessible, "GetRole", error, "=>u", &role))
    {
      obj->role = role;
    _atspi_accessible_add_cache (obj, ATSPI_CACHE_ROLE);
    }
  }
  return obj->role;
}

/**
 * atspi_accessible_get_role_name:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Gets a UTF-8 string corresponding to the name of the role played by an object.
 * This method will return useful values for roles that fall outside the
 * enumeration used in atspi_accessible_get_role ().
 *
 * Returns: a UTF-8 string specifying the type of UI role played by an
 * #AtspiAccessible object.
 *
 **/
gchar *
atspi_accessible_get_role_name (AtspiAccessible *obj, GError **error)
{
  gchar *retval = NULL;
  AtspiRole role;

  g_return_val_if_fail (obj != NULL, NULL);

  role = atspi_accessible_get_role (obj, error);
  if (role >= 0 && role < ATSPI_ROLE_COUNT && role != ATSPI_ROLE_EXTENDED)
    return atspi_role_get_name (role);

  _atspi_dbus_call (obj, atspi_interface_accessible, "GetRoleName", error, "=>s", &retval);

  if (!retval)
    retval = g_strdup ("");

  return retval;
}

/**
 * atspi_accessible_get_localized_role_name:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Gets a UTF-8 string corresponding to the name of the role played by an 
 * object, translated to the current locale.
 * This method will return useful values for roles that fall outside the
 * enumeration used in atspi_accessible_getRole ().
 *
 * Returns: a localized, UTF-8 string specifying the type of UI role played 
 * by an #AtspiAccessible object.
 *
 **/
gchar *
atspi_accessible_get_localized_role_name (AtspiAccessible *obj, GError **error)
{
  char *retval = NULL;

  g_return_val_if_fail (obj != NULL, NULL);

  _atspi_dbus_call (obj, atspi_interface_accessible, "GetLocalizedRoleName", error, "=>s", &retval);

  if (!retval)
    return g_strdup ("");

  return retval;
}

static AtspiStateSet *
defunct_set ()
{
  AtspiStateSet *set = atspi_state_set_new (NULL);
  atspi_state_set_add (set, ATSPI_STATE_DEFUNCT);
  return set;
}

/**
 * atspi_accessible_get_state_set:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Gets the states currently held by an object.
 *
 * Returns: (transfer full): a pointer to an #AtspiStateSet representing an
 * object's current state set.
 **/
AtspiStateSet *
atspi_accessible_get_state_set (AtspiAccessible *obj)
{
  /* TODO: Should take a GError **, but would be an API break */
  if (!obj->parent.app || !obj->parent.app->bus)
    return defunct_set ();

  if (!_atspi_accessible_test_cache (obj, ATSPI_CACHE_STATES))
  {
    DBusMessage *reply;
    DBusMessageIter iter;
    reply = _atspi_dbus_call_partial (obj, atspi_interface_accessible,
                                      "GetState", NULL, "");
    _ATSPI_DBUS_CHECK_SIG (reply, "au", NULL, defunct_set ());
    dbus_message_iter_init (reply, &iter);
    _atspi_dbus_set_state (obj, &iter);
    dbus_message_unref (reply);
    _atspi_accessible_add_cache (obj, ATSPI_CACHE_STATES);
  }

  return g_object_ref (obj->states);
}

/**
 * atspi_accessible_get_attributes:
 * @obj: The #AtspiAccessible being queried.
 *
 * Gets the #AttributeSet representing any assigned 
 * name-value pair attributes or annotations for this object.
 * For typographic, textual, or textually-semantic attributes, see
 * atspi_text_get_attributes instead.
 *
 * Returns: (element-type gchar* gchar*) (transfer full): The name-value-pair
 * attributes assigned to this object.
 */
GHashTable *
atspi_accessible_get_attributes (AtspiAccessible *obj, GError **error)
{
  DBusMessage *message;

    g_return_val_if_fail (obj != NULL, NULL);

  if (obj->priv->cache)
  {
    GValue *val = g_hash_table_lookup (obj->priv->cache, "Attributes");
    if (val)
      return g_value_dup_boxed (val);
  }

  if (!_atspi_accessible_test_cache (obj, ATSPI_CACHE_ATTRIBUTES))
  {
    message = _atspi_dbus_call_partial (obj, atspi_interface_accessible,
                                        "GetAttributes", error, "");
    obj->attributes = _atspi_dbus_return_hash_from_message (message);
    _atspi_accessible_add_cache (obj, ATSPI_CACHE_ATTRIBUTES);
  }

  if (!obj->attributes)
    return NULL;
  return g_hash_table_ref (obj->attributes);
}

static void
add_to_attribute_array (gpointer key, gpointer value, gpointer data)
{
  GArray **array = (GArray **)data;
  gchar *str = g_strconcat (key, ":", value, NULL);
  *array = g_array_append_val (*array, str);
}

/**
 * atspi_accessible_get_attributes_as_array:
 * @obj: The #AtspiAccessible being queried.
 *
 * Gets a #GArray representing any assigned 
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

    g_return_val_if_fail (obj != NULL, NULL);

  if (obj->priv->cache)
  {
    GValue *val = g_hash_table_lookup (obj->priv->cache, "Attributes");
    if (val)
    {
      GArray *array = g_array_new (TRUE, TRUE, sizeof (gchar *));
      GHashTable *attributes = g_value_get_boxed (val);
      g_hash_table_foreach (attributes, add_to_attribute_array, &array);
      return array;
    }
  }

  message = _atspi_dbus_call_partial (obj, atspi_interface_accessible, "GetAttributes", error, "");
  return _atspi_dbus_return_attribute_array_from_message (message);
}

/**
 * atspi_accessible_get_application:
 * @obj: The #AtspiAccessible being queried.
 *
 * Gets the containing #AtspiApplication for an object.
 *
 * Returns: (transfer full): the containing #AtspiApplication instance for
 *          this object.
 */
AtspiAccessible *
atspi_accessible_get_application (AtspiAccessible *obj, GError **error)
{
  AtspiAccessible *parent;

  g_object_ref (obj);
  for (;;)
  {
    parent = atspi_accessible_get_parent (obj, NULL);
    if (!parent && obj->parent.app &&
        atspi_accessible_get_role (obj, NULL) != ATSPI_ROLE_APPLICATION)
    {
      AtspiAccessible *root = g_object_ref (obj->parent.app->root);
      if (root)
      {
        g_object_unref (obj);
        if (atspi_accessible_get_role (root, NULL) == ATSPI_ROLE_DESKTOP_FRAME)
        {
          g_object_unref (root);
          return NULL;
        }
        return root;
      }
    }
    if (!parent || parent == obj ||
        atspi_accessible_get_role (parent, NULL) == ATSPI_ROLE_DESKTOP_FRAME)
  {
    if (parent)
      g_object_unref (parent);
    return obj;
  }
    g_object_unref (obj);
    obj = parent;
  }
}

/* Application-specific methods */

/**
 * atspi_accessible_get_toolkit_name:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Gets the toolkit name for an #AtspiAccessible object.
 * Only works on application root objects.
 *
 * Returns: a UTF-8 string indicating the toolkit name for the #AtspiAccessible object or NULL on exception.
 **/
gchar *
atspi_accessible_get_toolkit_name (AtspiAccessible *obj, GError **error)
{
  g_return_val_if_fail (obj != NULL, NULL);

  if (!obj->parent.app)
    return NULL;

  if (!obj->parent.app->toolkit_name)
    _atspi_dbus_get_property (obj, atspi_interface_application, "ToolkitName",
                              error, "s", &obj->parent.app->toolkit_name);

  return g_strdup (obj->parent.app->toolkit_name);
}

/**
 * atspi_accessible_get_toolkit_version:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Gets the toolkit version for an #AtspiAccessible object.
 * Only works on application root objects.
 *
 * Returns: a UTF-8 string indicating the toolkit version for the #AtspiAccessible object or NULL on exception.
 **/
gchar *
atspi_accessible_get_toolkit_version (AtspiAccessible *obj, GError **error)
{
  g_return_val_if_fail (obj != NULL, NULL);

  if (!obj->parent.app)
    return NULL;

  if (!obj->parent.app->toolkit_version)
    _atspi_dbus_get_property (obj, atspi_interface_application, "Version",
                              error, "s", &obj->parent.app->toolkit_version);

  return g_strdup (obj->parent.app->toolkit_version);
}

/**
 * atspi_accessible_get_atspi_version:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Gets the AT-SPI IPC specification version supported by the application
 * pointed to by the #AtspiAccessible object.
 * Only works on application root objects.
 *
 * Returns: a UTF-8 string indicating the AT-SPI version for the #AtspiAccessible object or NULL on exception.
 **/
gchar *
atspi_accessible_get_atspi_version (AtspiAccessible *obj, GError **error)
{
  g_return_val_if_fail (obj != NULL, NULL);

  if (!obj->parent.app)
    return NULL;

  if (!obj->parent.app->atspi_version)
    _atspi_dbus_get_property (obj, atspi_interface_application, "AtspiVersion",
                              error, "s", &obj->parent.app->atspi_version);

  return g_strdup (obj->parent.app->atspi_version);
}

/**
 * atspi_accessible_get_id:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Gets the application id for a #AtspiAccessible object.
 * Only works on application root objects.
 *
 * Returns: a positive #gint indicating the id for the #AtspiAccessible object 
 * or -1 on exception.
 **/
gint
atspi_accessible_get_id (AtspiAccessible *obj, GError **error)
{
  gint ret = -1;

  g_return_val_if_fail (obj != NULL, -1);

  if (!_atspi_dbus_get_property (obj, atspi_interface_application, "Id", error, "i", &ret))
      return -1;
  return ret;
}


/* Interface query methods */

static gboolean
_atspi_accessible_is_a (AtspiAccessible *accessible,
		      const char *interface_name)
{
  int n;

  if (accessible == NULL)
    {
      return FALSE;
    }

  if (!_atspi_accessible_test_cache (accessible, ATSPI_CACHE_INTERFACES))
  {
    DBusMessage *reply;
    DBusMessageIter iter;
    reply = _atspi_dbus_call_partial (accessible, atspi_interface_accessible,
                                      "GetInterfaces", NULL, "");
    _ATSPI_DBUS_CHECK_SIG (reply, "as", NULL, FALSE);
    dbus_message_iter_init (reply, &iter);
    _atspi_dbus_set_interfaces (accessible, &iter);
    dbus_message_unref (reply);
    _atspi_accessible_add_cache (accessible, ATSPI_CACHE_INTERFACES);
  }

  n = _atspi_get_iface_num (interface_name);
  if (n == -1) return FALSE;
  return (gboolean) ((accessible->interfaces & (1 << n))? TRUE: FALSE);
}

/**
 * atspi_accessible_is_action:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements the 
 * #AtspiAction interface.
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
 * Query whether the specified #AtspiAccessible implements the
 * #AtspiApplication interface.
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
 * atspi_accessible_is_collection:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements the
 * #AtspiCollection interface.
 *
 * Returns: #TRUE if @obj implements the #AtspiCollection interface,
 *          #FALSE otherwise.
 **/
gboolean
atspi_accessible_is_collection (AtspiAccessible *obj)
{
     return _atspi_accessible_is_a (obj,
			      atspi_interface_collection);
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
 * Query whether the specified #AtspiAccessible implements the
 * #AtspiDocument interface.
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
 * Query whether the specified #AtspiAccessible implements the
 * #AtspiEditableText interface.
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
 * Query whether the specified #AtspiAccessible implements the
 * #AtspiHypertext interface.
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
 * atspi_accessible_is_hyperlink:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements the 
 * #AtspiHyperlink interface.
 *
 * Returns: #TRUE if @obj implements the #AtspiHypertext interface,
 *          #FALSE otherwise.
 **/
gboolean
atspi_accessible_is_hyperlink (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_hyperlink);
}

/**
 * atspi_accessible_is_image:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements the
 * #AtspiImage interface.
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
 * Query whether the specified #AtspiAccessible implements the
 * #AtspiSelection interface.
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
 * Query whether the specified #AtspiAccessible implements the
 * #AtspiTable interface.
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
 * atspi_accessible_is_table_cell:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements the
 * #AtspiTableCell interface.
 *
 * Returns: #TRUE if @obj implements the #AtspiTable interface,
 *          #FALSE otherwise.
**/
gboolean
atspi_accessible_is_table_cell (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_table_cell);
}

/**
 * atspi_accessible_is_streamable_content:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements the
 * #AtspiStreamableContent interface.
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
 * Query whether the specified #AtspiAccessible implements the 
 * #AtspiText interface.
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
 * Query whether the specified #AtspiAccessible implements the
 * #AtspiValue interface.
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
 * atspi_accessible_get_action: (rename-to atspi_accessible_get_action_iface)
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiAction interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiAction interface
 *          instance, or NULL if @obj does not implement #AtspiAction.
 *
 * Deprecated: 2.10: Use atspi_accessible_get_action_iface instead.
 **/
AtspiAction *
atspi_accessible_get_action (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_action) ?
          g_object_ref (ATSPI_ACTION (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_action_iface:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiAction interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiAction interface
 *          instance, or NULL if @obj does not implement #AtspiAction.
 **/
AtspiAction *
atspi_accessible_get_action_iface (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_action) ?
          g_object_ref (ATSPI_ACTION (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_collection: (rename-to atspi_accessible_get_collection_iface)
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiCollection interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiCollection interface
 *          instance, or NULL if @obj does not implement #AtspiCollection.
 *
 * Deprecated: 2.10: Use atspi_accessible_get_collection_iface instead.
 **/
AtspiCollection *
atspi_accessible_get_collection (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_collection) ?
          g_object_ref (ATSPI_COLLECTION (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_collection_iface:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiCollection interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiCollection interface
 *          instance, or NULL if @obj does not implement #AtspiCollection.
 **/
AtspiCollection *
atspi_accessible_get_collection_iface (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_collection) ?
          g_object_ref (ATSPI_COLLECTION (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_component: (rename-to atspi_accessible_get_component_iface)
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiComponent interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiComponent interface
 *          instance, or NULL if @obj does not implement #AtspiComponent.
 *
 * Deprecated: 2.10: Use atspi_accessible_get_component_iface instead.
 **/
AtspiComponent *
atspi_accessible_get_component (AtspiAccessible *obj)
{
  return (_atspi_accessible_is_a (obj, atspi_interface_component) ?
          g_object_ref (ATSPI_COMPONENT (obj)) : NULL);
}

/**
 * atspi_accessible_get_component_iface:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiComponent interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiComponent interface
 *          instance, or NULL if @obj does not implement #AtspiComponent.
 **/
AtspiComponent *
atspi_accessible_get_component_iface (AtspiAccessible *obj)
{
  return (_atspi_accessible_is_a (obj, atspi_interface_component) ?
          g_object_ref (ATSPI_COMPONENT (obj)) : NULL);
}

/**
 * atspi_accessible_get_document: (rename-to atspi_accessible_get_document_iface)
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiDocument interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiDocument interface
 *          instance, or NULL if @obj does not implement #AtspiDocument.
 *
 * Deprecated: 2.10: Use atspi_accessible_get_document_iface instead.
 **/
AtspiDocument *
atspi_accessible_get_document (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_document) ?
          g_object_ref (ATSPI_DOCUMENT (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_document_iface:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiDocument interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiDocument interface
 *          instance, or NULL if @obj does not implement #AtspiDocument.
 **/
AtspiDocument *
atspi_accessible_get_document_iface (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_document) ?
          g_object_ref (ATSPI_DOCUMENT (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_editable_text: (rename-to atspi_accessible_get_editable_text_iface)
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiEditableText interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiEditableText interface
 *          instance, or NULL if @obj does not implement #AtspiEditableText.
 *
 * Deprecated: 2.10: Use atspi_accessible_get_editable_text_iface instead.
 **/
AtspiEditableText *
atspi_accessible_get_editable_text (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_editable_text) ?
          g_object_ref (ATSPI_EDITABLE_TEXT (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_editable_text_iface:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiEditableText interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiEditableText interface
 *          instance, or NULL if @obj does not implement #AtspiEditableText.
 **/
AtspiEditableText *
atspi_accessible_get_editable_text_iface (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_editable_text) ?
          g_object_ref (ATSPI_EDITABLE_TEXT (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_hyperlink:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Gets the #AtspiHyperlink interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): the #AtspiHyperlink object associated with
 *          the given #AtspiAccessible, or NULL if not supported.
 **/
AtspiHyperlink *
atspi_accessible_get_hyperlink (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_hyperlink) ?
          _atspi_hyperlink_new (accessible->parent.app, accessible->parent.path) : NULL);
}

/**
 * atspi_accessible_get_hypertext: (rename-to atspi_accessible_get_hypertext_iface)
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiHypertext interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiHypertext interface
 *          instance, or NULL if @obj does not implement #AtspiHypertext.
 *
 * Deprecated: 2.10: Use atspi_accessible_get_hypertext_iface instead.
 **/
AtspiHypertext *
atspi_accessible_get_hypertext (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_hypertext) ?
          g_object_ref (ATSPI_HYPERTEXT (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_hypertext_iface:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiHypertext interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiHypertext interface
 *          instance, or NULL if @obj does not implement #AtspiHypertext.
 **/
AtspiHypertext *
atspi_accessible_get_hypertext_iface (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_hypertext) ?
          g_object_ref (ATSPI_HYPERTEXT (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_image: (rename-to atspi_accessible_get_image_iface)
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiImage interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiImage interface instance, or
 *          NULL if @obj does not implement #AtspiImage.
 *
 * Deprecated: 2.10: Use atspi_accessible_get_image_iface instead.
 **/
AtspiImage *
atspi_accessible_get_image (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_image) ?
          g_object_ref (ATSPI_IMAGE (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_image_iface:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiImage interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiImage interface instance, or
 *          NULL if @obj does not implement #AtspiImage.
 **/
AtspiImage *
atspi_accessible_get_image_iface (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_image) ?
          g_object_ref (ATSPI_IMAGE (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_selection: (rename-to atspi_accessible_get_selection_iface)
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiSelection interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiSelection interface
 *          instance, or NULL if @obj does not implement #AtspiSelection.
 *
 * Deprecated: 2.10: Use atspi_accessible_get_selection_iface instead.
 **/
AtspiSelection *
atspi_accessible_get_selection (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_selection) ?
          g_object_ref (ATSPI_SELECTION (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_selection_iface:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiSelection interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiSelection interface
 *          instance, or NULL if @obj does not implement #AtspiSelection.
 **/
AtspiSelection *
atspi_accessible_get_selection_iface (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_selection) ?
          g_object_ref (ATSPI_SELECTION (accessible)) : NULL);  
}

#if 0
/**
 * atspi_accessible_get_streamable_content:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiStreamableContent interface for an #AtspiAccessible.
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
 * atspi_accessible_get_table: (rename-to atspi_accessible_get_table_iface)
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiTable interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiTable interface instance, or
 *          NULL if @obj does not implement #AtspiTable.
 *
 * Deprecated: 2.10: Use atspi_accessible_get_table_iface instead.
 **/
AtspiTable *
atspi_accessible_get_table (AtspiAccessible *obj)
{
  return (_atspi_accessible_is_a (obj, atspi_interface_table) ?
          g_object_ref (ATSPI_TABLE (obj)) : NULL);  
}

/**
 * atspi_accessible_get_table_iface:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiTable interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiTable interface instance, or
 *          NULL if @obj does not implement #AtspiTable.
 **/
AtspiTable *
atspi_accessible_get_table_iface (AtspiAccessible *obj)
{
  return (_atspi_accessible_is_a (obj, atspi_interface_table) ?
          g_object_ref (ATSPI_TABLE (obj)) : NULL);  
}

/**
 * atspi_accessible_get_table_cell:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiTableCell interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiTableCell interface instance,
 *          or NULL if @obj does not implement #AtspiTable.
 **/
AtspiTableCell *
atspi_accessible_get_table_cell (AtspiAccessible *obj)
{
  return (_atspi_accessible_is_a (obj, atspi_interface_table_cell) ?
          g_object_ref (ATSPI_TABLE_CELL (obj)) : NULL);  
}

/**
 * atspi_accessible_get_text: (rename-to atspi_accessible_get_text_iface)
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiTable interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiText interface instance, or
 *          NULL if @obj does not implement #AtspiText.
 *
 * Deprecated: 2.10: Use atspi_accessible_get_text_iface instead.
 **/
AtspiText *
atspi_accessible_get_text (AtspiAccessible *obj)
{
  return (_atspi_accessible_is_a (obj, atspi_interface_text) ?
          g_object_ref (ATSPI_TEXT (obj)) : NULL);
}

/**
 * atspi_accessible_get_text_iface:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiTable interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiText interface instance, or
 *          NULL if @obj does not implement #AtspiText.
 **/
AtspiText *
atspi_accessible_get_text_iface (AtspiAccessible *obj)
{
  return (_atspi_accessible_is_a (obj, atspi_interface_text) ?
          g_object_ref (ATSPI_TEXT (obj)) : NULL);
}

/**
 * atspi_accessible_get_value: (rename-to atspi_accessible_get_value_iface)
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiTable interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiValue interface instance, or
 *          NULL if @obj does not implement #AtspiValue.
 *
 * Deprecated: 2.10: Use atspi_accessible_get_value_iface instead.
 **/
AtspiValue *
atspi_accessible_get_value (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_value) ?
          g_object_ref (ATSPI_VALUE (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_value_iface:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Gets the #AtspiTable interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiValue interface instance, or
 *          NULL if @obj does not implement #AtspiValue.
 **/
AtspiValue *
atspi_accessible_get_value_iface (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_value) ?
          g_object_ref (ATSPI_VALUE (accessible)) : NULL);  
}

static void
append_const_val (GArray *array, const gchar *val)
{
  gchar *dup = g_strdup (val);

  if (dup)
    g_array_append_val (array, dup);
}

/**
 * atspi_accessible_get_interfaces:
 * @obj: The #AtspiAccessible to query.
 *
 * A set of pointers to all interfaces supported by an #AtspiAccessible.
 *
 * Returns: (element-type gchar*) (transfer full): A #GArray of strings
 *          describing the interfaces supported by the object.  Interfaces are
 *          denoted in short-hand (i.e. "Component", "Text" etc.).
 **/
GArray *
atspi_accessible_get_interfaces (AtspiAccessible *obj)
{
  GArray *ret = g_array_new (TRUE, TRUE, sizeof (gchar *));

  g_return_val_if_fail (obj != NULL, NULL);

  append_const_val (ret, "Accessible");
  if (atspi_accessible_is_action (obj))
    append_const_val (ret, "Action");
  if (atspi_accessible_is_collection (obj))
    append_const_val (ret, "Collection");
  if (atspi_accessible_is_component (obj))
    append_const_val (ret, "Component");
  if (atspi_accessible_is_document (obj))
    append_const_val (ret, "Document");
  if (atspi_accessible_is_editable_text (obj))
    append_const_val (ret, "EditableText");
  if (atspi_accessible_is_hypertext (obj))
    append_const_val (ret, "Hypertext");
  if (atspi_accessible_is_hyperlink (obj))
    append_const_val (ret, "Hyperlink");
  if (atspi_accessible_is_image (obj))
    append_const_val (ret, "Image");
  if (atspi_accessible_is_selection (obj))
    append_const_val (ret, "Selection");
  if (atspi_accessible_is_table (obj))
    append_const_val (ret, "Table");
  if (atspi_accessible_is_table_cell (obj))
    append_const_val (ret, "TableCell");
  if (atspi_accessible_is_text (obj))
    append_const_val (ret, "Text");
  if (atspi_accessible_is_value (obj))
    append_const_val (ret, "Value");

  return ret;
}

AtspiAccessible * 
_atspi_accessible_new (AtspiApplication *app, const gchar *path)
{
  AtspiAccessible *accessible;
  
  accessible = g_object_new (ATSPI_TYPE_ACCESSIBLE, NULL);
  g_return_val_if_fail (accessible != NULL, NULL);

  accessible->parent.app = g_object_ref (app);
  accessible->parent.path = g_strdup (path);

  return accessible;
}

/**
 * atspi_accessible_set_cache_mask:
 * @accessible: The #AtspiAccessible to operate on.  Must be the desktop or
 *             the root of an application.
 * @mask: An #AtspiCache specifying a bit mask of the types of data to cache.
 *
 * Sets the type of data to cache for accessibles.
 * If this is not set for an application or is reset to ATSPI_CACHE_UNDEFINED,
 * then the desktop's cache flag will be used.
 * If the desktop's cache flag is also undefined, then all possible data will
 * be cached.
 * This function is intended to work around bugs in toolkits where the proper
 * events are not raised / to aid in testing for such bugs.
 **/
void
atspi_accessible_set_cache_mask (AtspiAccessible *accessible, AtspiCache mask)
{
  g_return_if_fail (accessible != NULL);
  g_return_if_fail (accessible->parent.app != NULL);
  g_return_if_fail (accessible == accessible->parent.app->root);
  accessible->parent.app->cache = mask;
  enable_caching = TRUE;
}

/**
 * atspi_accessible_clear_cache:
 * @obj: The #AtspiAccessible whose cache to clear.
 *
 * Clears the cached information for the given accessible and all of its
 * descendants.
 */
void
atspi_accessible_clear_cache (AtspiAccessible *obj)
{
  gint i;

  if (obj)
  {
    obj->cached_properties = ATSPI_CACHE_NONE;
    for (i = 0; i < obj->children->len; i++)
      atspi_accessible_clear_cache (g_ptr_array_index (obj->children, i));
  }
}

/**
 * atspi_accessible_get_process_id:
 * @accessible: The #AtspiAccessible to query.
 * @error: a pointer to a %NULL #GError pointer
 *
 * Returns the process id associated with the given accessible.  Mainly
 * added for debugging; it is a shortcut to explicitly querying the
 * accessible's app->bus_name and then calling GetConnectionUnixProcessID.
 *
 * Returns: The process ID or undetermined value if @error is set.
 **/
guint
atspi_accessible_get_process_id (AtspiAccessible *accessible, GError **error)
{
  DBusMessage *message, *reply;
  DBusConnection *bus = _atspi_bus ();
  dbus_uint32_t pid = -1;
  DBusError d_error;

  if (!accessible->parent.app || !accessible->parent.app->bus_name)
    {
      g_set_error_literal(error, ATSPI_ERROR, ATSPI_ERROR_IPC, "Process is defunct");
      return -1;
    }

  message = dbus_message_new_method_call ("org.freedesktop.DBus",
                                          "/org/freedesktop/DBus",
                                          "org.freedesktop.DBus",
                                          "GetConnectionUnixProcessID");
  dbus_message_append_args (message, DBUS_TYPE_STRING,
                            &accessible->parent.app->bus_name,
                            DBUS_TYPE_INVALID);
  dbus_error_init (&d_error);
  reply = dbus_connection_send_with_reply_and_block (bus, message, -1, &d_error);
  dbus_message_unref (message);
  if (reply)
  {
    if (!strcmp (dbus_message_get_signature (reply), "u"))
      dbus_message_get_args (reply, NULL, DBUS_TYPE_UINT32, &pid, DBUS_TYPE_INVALID);
    dbus_message_unref (reply);
  }
  if (dbus_error_is_set (&d_error))
    {
      g_set_error_literal(error, ATSPI_ERROR, ATSPI_ERROR_IPC, "Process is defunct");
      dbus_error_free (&d_error);
    }
  return pid;
}

AtspiCache
_atspi_accessible_get_cache_mask (AtspiAccessible *accessible)
{
  AtspiCache mask;

  if (!accessible->parent.app)
    return ATSPI_CACHE_NONE;

 mask = accessible->parent.app->cache;
  if (mask == ATSPI_CACHE_UNDEFINED &&
      accessible->parent.app->root &&
      accessible->parent.app->root->accessible_parent)
  {
    AtspiAccessible *desktop = atspi_get_desktop (0);
    mask = desktop->parent.app->cache;
    g_object_unref (desktop);
  }

  if (mask == ATSPI_CACHE_UNDEFINED)
    mask = ATSPI_CACHE_DEFAULT;

  return mask;
}

gboolean
_atspi_accessible_test_cache (AtspiAccessible *accessible, AtspiCache flag)
{
  AtspiCache mask = _atspi_accessible_get_cache_mask (accessible);
  AtspiCache result = accessible->cached_properties & mask & flag;
  if (accessible->states && atspi_state_set_contains (accessible->states, ATSPI_STATE_TRANSIENT))
    return FALSE;
  return (result != 0 && (atspi_main_loop || enable_caching ||
                          flag == ATSPI_CACHE_INTERFACES) &&
          !atspi_no_cache);
}

void
_atspi_accessible_add_cache (AtspiAccessible *accessible, AtspiCache flag)
{
  AtspiCache mask = _atspi_accessible_get_cache_mask (accessible);

  accessible->cached_properties |= flag & mask;
}

/**
 * atspi_accessible_get_locale:
 * @accessible: an #AtspiAccessible
 *
 * Gets a UTF-8 string indicating the POSIX-style LC_MESSAGES locale
 * of @accessible.
 *
 * Since: 2.7.91
 *
 * Returns: a UTF-8 string indicating the POSIX-style LC_MESSAGES
 *          locale of @accessible.
 **/
const gchar*
atspi_accessible_get_object_locale (AtspiAccessible *accessible, GError **error)
{
  gchar *locale;

  g_return_val_if_fail (accessible != NULL, NULL);

  locale = g_object_get_qdata (G_OBJECT (accessible), quark_locale);
  if (!locale)
  {
    if (!_atspi_dbus_get_property (accessible, atspi_interface_accessible,
                                   "Locale", error, "s", &locale))
      return NULL;
    if (locale)
      g_object_set_qdata_full (G_OBJECT (accessible), quark_locale, locale,
                               g_free);
  }
  return locale;
}

void
free_value (gpointer data)
{
  GValue *value = data;

  g_value_unset (value);
  g_free (value);
}

GHashTable *
_atspi_accessible_ref_cache (AtspiAccessible *accessible)
{
  AtspiAccessiblePrivate *priv = accessible->priv;

  priv->cache_ref_count++;
  if (priv->cache)
    return g_hash_table_ref (priv->cache);
  priv->cache = g_hash_table_new_full (g_str_hash, g_str_equal, g_free,
                                       free_value);
  return priv->cache;
}

void
_atspi_accessible_unref_cache (AtspiAccessible *accessible)
{
  AtspiAccessiblePrivate *priv = accessible->priv;

  if (priv->cache)
  {
    g_hash_table_unref (priv->cache);
    if (--priv->cache_ref_count == 0)
      priv->cache = NULL;
  }
}
