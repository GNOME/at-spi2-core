/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2007 IBM Corp.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/* collection.c: implements the Collection interface */

#include <string.h>
#include <strings.h>

#include "bridge.h"
#include <atk/atk.h>
#include <droute/droute.h>

#include "accessible-stateset.h"
#include "bitarray.h"
#include "spi-dbus.h"

#include "accessible-register.h"
#include "introspection.h"
#include "object.h"

typedef struct _MatchRulePrivate MatchRulePrivate;
struct _MatchRulePrivate
{
  gint *states;
  AtspiCollectionMatchType statematchtype;
  AtkAttributeSet *attributes;
  AtspiCollectionMatchType attributematchtype;
  gint *roles;
  AtspiCollectionMatchType rolematchtype;
  gchar **ifaces;
  AtspiCollectionMatchType interfacematchtype;
  gboolean invert;
};

static gboolean
child_interface_p (AtkObject *child, gchar *repo_id)
{
  if (!strncasecmp (repo_id, "action", 6))
    {
      AtkAction *iface;
      gint i, count;
      char *p;
      char name[64];
      if (!ATK_IS_ACTION (child))
        return FALSE;
      iface = ATK_ACTION (child);
      count = atk_action_get_n_actions (iface);
      if (count <= 0)
        return FALSE;
      if (repo_id[6] == '\0')
        return TRUE;
      p = strchr (repo_id, '(');
      if (!p)
        return FALSE;
      strncpy (name, p + 1, sizeof (name));
      name[sizeof (name) - 1] = '\0';
      p = strchr (name, ')');
      if (p)
        *p = '\0';
      for (i = 0; i < count; i++)
        {
          const char *action = atk_action_get_name (iface, i);
          if (!strcasecmp (name, action))
            return TRUE;
        }
      return FALSE;
    }
  if (!strcasecmp (repo_id, "accessible") || !strcasecmp (repo_id, "collection"))
    return ATK_IS_OBJECT (child);
  if (!strcasecmp (repo_id, "component"))
    return ATK_IS_COMPONENT (child);
  if (!strcasecmp (repo_id, "editabletext"))
    return ATK_IS_EDITABLE_TEXT (child);
  if (!strcasecmp (repo_id, "text"))
    return ATK_IS_TEXT (child);
  if (!strcasecmp (repo_id, "hypertext"))
    return ATK_IS_HYPERTEXT (child);
  if (!strcasecmp (repo_id, "image"))
    return ATK_IS_IMAGE (child);
  if (!strcasecmp (repo_id, "selection"))
    return ATK_IS_SELECTION (child);
  if (!strcasecmp (repo_id, "table"))
    return ATK_IS_TABLE (child);
  if (!strcasecmp (repo_id, "value"))
    return ATK_IS_VALUE (child);
  if (!strcasecmp (repo_id, "streamablecontent"))
    return ATK_IS_STREAMABLE_CONTENT (child);
  if (!strcasecmp (repo_id, "document"))
    return ATK_IS_DOCUMENT (child);
  return FALSE;
}

static gboolean
match_states_all_p (AtkObject *child, gint *set)
{
  AtkStateSet *chs;
  gint i;
  gboolean ret = TRUE;

  if (set == NULL || set[0] == BITARRAY_SEQ_TERM)
    return TRUE;

  chs = atk_object_ref_state_set (child);

  // TODO: use atk-state_set_contains_states; would be more efficient
  for (i = 0; set[i] != BITARRAY_SEQ_TERM; i++)
    {
      if (!atk_state_set_contains_state (chs, set[i]))
        {
          ret = FALSE;
          break;
        }
    }

  g_object_unref (chs);
  return ret;
}

static gboolean
match_states_any_p (AtkObject *child, gint *set)
{
  AtkStateSet *chs;
  gint i;
  gboolean ret = FALSE;

  if (set == NULL || set[0] == BITARRAY_SEQ_TERM)
    return TRUE;

  chs = atk_object_ref_state_set (child);

  for (i = 0; set[i] != BITARRAY_SEQ_TERM; i++)
    {
      if (atk_state_set_contains_state (chs, set[i]))
        {
          ret = TRUE;
          break;
        }
    }

  g_object_unref (chs);
  return ret;
}

static gboolean
match_states_none_p (AtkObject *child, gint *set)
{
  AtkStateSet *chs;
  gint i;
  gboolean ret = TRUE;

  if (set == NULL || set[0] == BITARRAY_SEQ_TERM)
    return TRUE;

  chs = atk_object_ref_state_set (child);

  for (i = 0; set[i] != BITARRAY_SEQ_TERM; i++)
    {
      if (atk_state_set_contains_state (chs, set[i]))
        {
          ret = FALSE;
          break;
        }
    }

  g_object_unref (chs);
  return ret;
}

// TODO: need to convert at-spi roles/states to atk roles/states */
static gboolean
match_states_lookup (AtkObject *child, MatchRulePrivate *mrp)
{
  switch (mrp->statematchtype)
    {
    case ATSPI_Collection_MATCH_ALL:
      if (match_states_all_p (child, mrp->states))
        return TRUE;
      break;

    case ATSPI_Collection_MATCH_ANY:
      if (match_states_any_p (child, mrp->states))
        return TRUE;
      break;

    case ATSPI_Collection_MATCH_NONE:
      if (match_states_none_p (child, mrp->states))
        return TRUE;
      break;

    default:
      break;
    }

  return FALSE;
}

// TODO: Map at-spi -> atk roles at mrp creation instead of doing this;
// would be more efficient
#define spi_get_role(obj) spi_accessible_role_from_atk_role (atk_object_get_role (obj))

static gboolean
match_roles_all_p (AtkObject *child, gint *roles)
{
  if (roles == NULL || roles[0] == BITARRAY_SEQ_TERM)
    return TRUE;
  else if (roles[1] != BITARRAY_SEQ_TERM)
    return FALSE;

  return (spi_get_role (child) == roles[0]);
}

static gboolean
match_roles_any_p (AtkObject *child, gint *roles)
{
  AtspiRole role;
  int i;

  if (roles == NULL || roles[0] == BITARRAY_SEQ_TERM)
    return TRUE;

  role = spi_accessible_role_from_atk_role (atk_object_get_role (child));

  for (i = 0; roles[i] != BITARRAY_SEQ_TERM; i++)
    if (role == roles[i])
      return TRUE;

  return FALSE;
}

static gboolean
match_roles_none_p (AtkObject *child, gint *roles)
{
  AtspiRole role;
  int i;

  if (roles == NULL || roles[0] == BITARRAY_SEQ_TERM)
    return TRUE;

  role = spi_get_role (child);

  for (i = 0; roles[i] != BITARRAY_SEQ_TERM; i++)
    if (role == roles[i])
      return FALSE;

  return TRUE;
}

static gboolean
match_roles_lookup (AtkObject *child, MatchRulePrivate *mrp)
{
  switch (mrp->rolematchtype)
    {
    case ATSPI_Collection_MATCH_ALL:
      if (match_roles_all_p (child, mrp->roles))
        return TRUE;
      break;

    case ATSPI_Collection_MATCH_ANY:
      if (match_roles_any_p (child, mrp->roles))
        return TRUE;
      break;

    case ATSPI_Collection_MATCH_NONE:
      if (match_roles_none_p (child, mrp->roles))
        return TRUE;
      break;

    default:
      break;
    }
  return FALSE;
}

static gboolean
match_interfaces_all_p (AtkObject *obj, gchar **ifaces)
{
  gint i;

  if (ifaces == NULL)
    return TRUE;

  for (i = 0; ifaces[i]; i++)
    if (!child_interface_p (obj, ifaces[i]))
      {
        return FALSE;
      }
  return TRUE;
}

static gboolean
match_interfaces_any_p (AtkObject *obj, gchar **ifaces)
{
  gint i;

  if (ifaces == NULL)
    return TRUE;

  for (i = 0; ifaces[i]; i++)
    if (child_interface_p (obj, ifaces[i]))
      {
        return TRUE;
      }
  return FALSE;
}

static gboolean
match_interfaces_none_p (AtkObject *obj, gchar **ifaces)
{
  gint i;

  for (i = 0; ifaces[i]; i++)
    if (child_interface_p (obj, ifaces[i]))
      return FALSE;

  return TRUE;
}

static gboolean
match_interfaces_lookup (AtkObject *child, MatchRulePrivate *mrp)
{
  switch (mrp->interfacematchtype)
    {

    case ATSPI_Collection_MATCH_ALL:
      if (match_interfaces_all_p (child, mrp->ifaces))
        return TRUE;
      break;

    case ATSPI_Collection_MATCH_ANY:
      if (match_interfaces_any_p (child, mrp->ifaces))
        return TRUE;
      break;

    case ATSPI_Collection_MATCH_NONE:
      if (match_interfaces_none_p (child, mrp->ifaces))
        return TRUE;
      break;

    default:
      break;
    }

  return FALSE;
}

#define split_attributes(attributes) (g_strsplit (attributes, ";", 0))

static gboolean
match_attributes_all_p (AtkObject *child, AtkAttributeSet *attributes)
{
  int i, k;
  AtkAttributeSet *oa;
  gint length, oa_length;
  gboolean flag = FALSE;

  if (attributes == NULL || g_slist_length (attributes) == 0)
    return TRUE;

  oa = atk_object_get_attributes (child);
  length = g_slist_length (attributes);
  oa_length = g_slist_length (oa);

  for (i = 0; i < length; i++)
    {
      AtkAttribute *attr = g_slist_nth_data (attributes, i);
      for (k = 0; k < oa_length; k++)
        {
          AtkAttribute *oa_attr = g_slist_nth_data (oa, k);
          if (!g_ascii_strcasecmp (oa_attr->name, attr->name) &&
              !g_ascii_strcasecmp (oa_attr->value, attr->value))
            {
              flag = TRUE;
              break;
            }
          else
            flag = FALSE;
        }
      if (!flag)
        {
          atk_attribute_set_free (oa);
          return FALSE;
        }
    }
  atk_attribute_set_free (oa);
  return TRUE;
}

static gboolean
match_attributes_any_p (AtkObject *child, AtkAttributeSet *attributes)
{
  int i, k;

  AtkAttributeSet *oa;
  gint length, oa_length;

  length = g_slist_length (attributes);
  if (length == 0)
    return TRUE;

  oa = atk_object_get_attributes (child);
  oa_length = g_slist_length (oa);

  for (i = 0; i < length; i++)
    {
      AtkAttribute *attr = g_slist_nth_data (attributes, i);
      for (k = 0; k < oa_length; k++)
        {
          AtkAttribute *oa_attr = g_slist_nth_data (oa, k);
          if (!g_ascii_strcasecmp (oa_attr->name, attr->name) &&
              !g_ascii_strcasecmp (oa_attr->value, attr->value))
            {
              atk_attribute_set_free (oa);
              return TRUE;
            }
        }
    }
  atk_attribute_set_free (oa);
  return FALSE;
}

static gboolean
match_attributes_none_p (AtkObject *child, AtkAttributeSet *attributes)
{
  int i, k;

  AtkAttributeSet *oa;
  gint length, oa_length;

  length = g_slist_length (attributes);
  if (length == 0)
    return TRUE;

  oa = atk_object_get_attributes (child);
  oa_length = g_slist_length (oa);

  for (i = 0; i < length; i++)
    {
      AtkAttribute *attr = g_slist_nth_data (attributes, i);
      for (k = 0; k < oa_length; k++)
        {
          AtkAttribute *oa_attr = g_slist_nth_data (oa, k);
          if (!g_ascii_strcasecmp (oa_attr->name, attr->name) &&
              !g_ascii_strcasecmp (oa_attr->value, attr->value))
            {
              atk_attribute_set_free (oa);
              return FALSE;
            }
        }
    }
  atk_attribute_set_free (oa);
  return TRUE;
}

static gboolean
match_attributes_lookup (AtkObject *child, MatchRulePrivate *mrp)
{
  switch (mrp->attributematchtype)
    {

    case ATSPI_Collection_MATCH_ALL:
      if (match_attributes_all_p (child, mrp->attributes))
        return TRUE;
      break;

    case ATSPI_Collection_MATCH_ANY:
      if (match_attributes_any_p (child, mrp->attributes))
        return TRUE;
      break;

    case ATSPI_Collection_MATCH_NONE:
      if (match_attributes_none_p (child, mrp->attributes))
        return TRUE;
      break;

    default:
      break;
    }
  return FALSE;
}

static int
sort_order_canonical (MatchRulePrivate *mrp, GList *ls, gint kount, gint max, AtkObject *obj, glong index, gboolean flag, AtkObject *pobj, gboolean traverse)
{
  gint i = index;
  glong acount = atk_object_get_n_accessible_children (obj);
  gboolean prev = pobj ? TRUE : FALSE;

  if (acount > ATSPI_MAX_CHILDREN)
    acount = ATSPI_MAX_CHILDREN;
  for (; i < acount && (max == 0 || kount < max); i++)
    {
      AtkObject *child = atk_object_ref_accessible_child (obj, i);

      if (!child)
        continue;

      if (prev && child == pobj)
        {
          g_object_unref (child);
          return kount;
        }

      if (flag && match_interfaces_lookup (child, mrp) && match_states_lookup (child, mrp) && match_roles_lookup (child, mrp) && match_attributes_lookup (child, mrp))
        {

          ls = g_list_append (ls, child);
          kount++;
        }

      if (!flag)
        flag = TRUE;

      if (traverse)
        kount = sort_order_canonical (mrp, ls, kount,
                                      max, child, 0, TRUE,
                                      pobj, traverse);
      g_object_unref (child);
    }
  return kount;
}

static int
sort_order_rev_canonical (MatchRulePrivate *mrp, GList *ls, gint kount, gint max, AtkObject *obj, gboolean flag, AtkObject *pobj)
{
  AtkObject *nextobj;
  AtkObject *parent;
  glong indexinparent;

  /* This breaks us out of the recursion. */
  if (!obj || obj == pobj)
    {
      return kount;
    }

  /* Add to the list if it matches */
  if (flag && match_interfaces_lookup (obj, mrp) && match_states_lookup (obj, mrp) && match_roles_lookup (obj, mrp) && match_attributes_lookup (obj, mrp) && (max == 0 || kount < max))
    {
      ls = g_list_append (ls, obj);
      kount++;
    }

  /* Get the current nodes index in it's parent and the parent object. */
  indexinparent = atk_object_get_index_in_parent (obj);
  parent = atk_object_get_parent (obj);

  if (indexinparent > 0 && (max == 0 || kount < max))
    {
      /* there are still some siblings to visit so get the previous sibling
         and get it's last descendant.
         First, get the previous sibling */
      nextobj = atk_object_ref_accessible_child (parent, indexinparent - 1);

      /* Now, drill down the right side to the last descendant */
      while (nextobj && atk_object_get_n_accessible_children (nextobj) > 0)
        {
          AtkObject *follow;
          gint count = atk_object_get_n_accessible_children (nextobj);
          if (count > ATSPI_MAX_CHILDREN)
            count = ATSPI_MAX_CHILDREN;

          follow = atk_object_ref_accessible_child (nextobj, count - 1);
          g_object_unref (nextobj);
          nextobj = follow;
        }
      /* recurse with the last descendant */
      kount = sort_order_rev_canonical (mrp, ls, kount, max,
                                        nextobj, TRUE, pobj);
      if (nextobj)
        g_object_unref (nextobj);
    }
  else if (max == 0 || kount < max)
    {
      /* no more siblings so next node must be the parent */
      kount = sort_order_rev_canonical (mrp, ls, kount, max,
                                        parent, TRUE, pobj);
    }
  return kount;
}

static int
query_exec (MatchRulePrivate *mrp, AtspiCollectionSortOrder sortby, GList *ls, gint kount, gint max, AtkObject *obj, glong index, gboolean flag, AtkObject *pobj, gboolean traverse)
{
  switch (sortby)
    {
    case ATSPI_Collection_SORT_ORDER_CANONICAL:
      kount = sort_order_canonical (mrp, ls, 0, max, obj, index, flag,
                                    pobj, traverse);
      break;
    case ATSPI_Collection_SORT_ORDER_REVERSE_CANONICAL:
      kount = sort_order_canonical (mrp, ls, 0, max, obj, index, flag,
                                    pobj, traverse);
      break;
    default:
      kount = 0;
      g_warning ("Sort method not implemented yet");
      break;
    }

  return kount;
}

static gboolean
bitarray_to_seq (dbus_uint32_t *array, int array_count, int **ret)
{
  int out_size = 4;
  int out_count = 0;
  int i, j;
  int *out = (int *) g_malloc (out_size * sizeof (int));

  for (i = 0; i < array_count; i++)
    {
      for (j = 0; j < 32; j++)
        {
          if (array[i] & (1 << j))
            {
              if (out_count == out_size - 2)
                {
                  out_size <<= 1;
                  out = (int *) g_realloc (out, out_size * sizeof (int));
                }
              out[out_count++] = i * 32 + j;
            }
        }
    }
  out[out_count] = BITARRAY_SEQ_TERM;
  *ret = out;
  return TRUE;
}

static dbus_bool_t
read_mr (DBusMessageIter *iter, MatchRulePrivate *mrp)
{
  DBusMessageIter iter_struct, iter_array, iter_dict, iter_dict_entry;
  dbus_uint32_t *array;
  dbus_int32_t matchType;
  int array_count;
  AtkAttribute *attr;
  int i;

  dbus_message_iter_recurse (iter, &iter_struct);

  /* states */
  dbus_message_iter_recurse (&iter_struct, &iter_array);
  dbus_message_iter_get_fixed_array (&iter_array, &array, &array_count);
  bitarray_to_seq (array, array_count, &mrp->states);
  for (i = 0; mrp->states[i] != BITARRAY_SEQ_TERM; i++)
    {
      mrp->states[i] = spi_atk_state_from_spi_state (mrp->states[i]);
    }
  dbus_message_iter_next (&iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &matchType);
  dbus_message_iter_next (&iter_struct);
  mrp->statematchtype = matchType;
  ;

  /* attributes */
  mrp->attributes = NULL;
  dbus_message_iter_recurse (&iter_struct, &iter_dict);
  while (dbus_message_iter_get_arg_type (&iter_dict) != DBUS_TYPE_INVALID)
    {
      const char *key, *val;
      const char *p, *q;
      dbus_message_iter_recurse (&iter_dict, &iter_dict_entry);
      dbus_message_iter_get_basic (&iter_dict_entry, &key);
      dbus_message_iter_next (&iter_dict_entry);
      dbus_message_iter_get_basic (&iter_dict_entry, &val);
      p = q = val;
      for (;;)
        {
          if (*q == '\0' || (*q == ':' && (q == val || q[-1] != '\\')))
            {
              char *tmp;
              attr = g_new (AtkAttribute, 1);
              attr->name = g_strdup (key);
              attr->value = g_strdup (p);
              attr->value[q - p] = '\0';
              tmp = attr->value;
              while (*tmp != '\0')
                {
                  if (*tmp == '\\')
                    memmove (tmp, tmp + 1, strlen (tmp));
                  else
                    tmp++;
                }
              mrp->attributes = g_slist_prepend (mrp->attributes, attr);
              if (*q == '\0')
                break;
              else
                p = ++q;
            }
          else
            q++;
        }
      dbus_message_iter_next (&iter_dict);
    }
  dbus_message_iter_next (&iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &matchType);
  mrp->attributematchtype = matchType;
  ;
  dbus_message_iter_next (&iter_struct);

  /* Get roles and role match */
  dbus_message_iter_recurse (&iter_struct, &iter_array);
  dbus_message_iter_get_fixed_array (&iter_array, &array, &array_count);
  bitarray_to_seq (array, array_count, &mrp->roles);
  dbus_message_iter_next (&iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &matchType);
  mrp->rolematchtype = matchType;
  ;
  dbus_message_iter_next (&iter_struct);

  /* Get interfaces and interface match */
  dbus_message_iter_recurse (&iter_struct, &iter_array);
  mrp->ifaces = g_new0 (gchar *, 16);
  i = 0;
  while (i < 15 && dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
    {
      char *iface;
      dbus_message_iter_get_basic (&iter_array, &iface);
      mrp->ifaces[i] = g_strdup (iface);
      i++;
      dbus_message_iter_next (&iter_array);
    }
  dbus_message_iter_next (&iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &matchType);
  mrp->interfacematchtype = matchType;
  ;
  dbus_message_iter_next (&iter_struct);
  /* get invert */
  dbus_message_iter_get_basic (&iter_struct, &mrp->invert);
  dbus_message_iter_next (iter);
  return TRUE;
}

static DBusMessage *
return_and_free_list (DBusMessage *message, GList *ls)
{
  DBusMessage *reply;
  DBusMessageIter iter, iter_array;
  GList *item;

  reply = dbus_message_new_method_return (message);
  if (!reply)
    return NULL;
  dbus_message_iter_init_append (reply, &iter);
  if (!dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "(so)", &iter_array))
    goto oom;
  for (item = ls; item; item = g_list_next (item))
    {
      spi_object_append_reference (&iter_array, ATK_OBJECT (item->data));
    }
  if (!dbus_message_iter_close_container (&iter, &iter_array))
    goto oom;
  g_list_free (ls);
  return reply;
oom:
  // TODO: Handle out of memory
  g_list_free (ls);
  return reply;
}

static void
free_mrp_data (MatchRulePrivate *mrp)
{
  g_free (mrp->states);
  atk_attribute_set_free (mrp->attributes);
  g_free (mrp->roles);
  g_strfreev (mrp->ifaces);
}

static DBusMessage *
GetMatchesFrom (DBusMessage *message,
                AtkObject *current_object,
                MatchRulePrivate *mrp,
                const AtspiCollectionSortOrder sortby,
                const dbus_bool_t isrestrict,
                dbus_int32_t count,
                const dbus_bool_t traverse)
{
  GList *ls = NULL;
  AtkObject *parent;
  glong index = atk_object_get_index_in_parent (current_object);

  ls = g_list_append (ls, current_object);

  if (!isrestrict)
    {
      parent = atk_object_get_parent (current_object);
      query_exec (mrp, sortby, ls, 0, count, parent, index,
                  FALSE, NULL, traverse);
    }
  else
    query_exec (mrp, sortby, ls, 0, count,
                current_object, 0, FALSE, NULL, traverse);

  ls = g_list_remove (ls, ls->data);

  if (sortby == ATSPI_Collection_SORT_ORDER_REVERSE_CANONICAL)
    ls = g_list_reverse (ls);

  free_mrp_data (mrp);
  return return_and_free_list (message, ls);
}

/*
  inorder traversal from a given object in the hierarchy
*/

static int
inorder (AtkObject *collection, MatchRulePrivate *mrp, GList *ls, gint kount, gint max, AtkObject *obj, gboolean flag, AtkObject *pobj, dbus_bool_t traverse)
{
  int i = 0;

  /* First, look through the children recursively. */
  kount = sort_order_canonical (mrp, ls, kount, max, obj, 0, TRUE,
                                NULL, TRUE);

  /* Next, we look through the right subtree */
  while ((max == 0 || kount < max) && obj && obj != collection)
    {
      AtkObject *parent = atk_object_get_parent (obj);
      i = atk_object_get_index_in_parent (obj);
      kount = sort_order_canonical (mrp, ls, kount, max, parent,
                                    i + 1, TRUE, FALSE, TRUE);
      obj = parent;
    }

  return kount;
}

/*
  GetMatchesInOrder: get matches from a given object in an inorder traversal.
*/

static DBusMessage *
GetMatchesInOrder (DBusMessage *message,
                   AtkObject *current_object,
                   MatchRulePrivate *mrp,
                   const AtspiCollectionSortOrder sortby,
                   const dbus_bool_t recurse,
                   dbus_int32_t count,
                   const dbus_bool_t traverse)
{
  GList *ls = NULL;
  AtkObject *obj;

  ls = g_list_append (ls, current_object);

  obj = ATK_OBJECT (spi_register_path_to_object (spi_global_register, dbus_message_get_path (message)));

  inorder (obj, mrp, ls, 0, count,
           current_object, TRUE, NULL, traverse);

  ls = g_list_remove (ls, ls->data);

  if (sortby == ATSPI_Collection_SORT_ORDER_REVERSE_CANONICAL)
    ls = g_list_reverse (ls);

  free_mrp_data (mrp);
  return return_and_free_list (message, ls);
}

/*
  GetMatchesInBackOrder: get matches from a given object in a
  reverse order traversal.
*/

static DBusMessage *
GetMatchesInBackOrder (DBusMessage *message,
                       AtkObject *current_object,
                       MatchRulePrivate *mrp,
                       const AtspiCollectionSortOrder sortby,
                       dbus_int32_t count)
{
  GList *ls = NULL;
  AtkObject *collection;

  ls = g_list_append (ls, current_object);

  collection = ATK_OBJECT (spi_register_path_to_object (spi_global_register, dbus_message_get_path (message)));

  sort_order_rev_canonical (mrp, ls, 0, count, current_object,
                            FALSE, collection);

  ls = g_list_remove (ls, ls->data);

  if (sortby == ATSPI_Collection_SORT_ORDER_REVERSE_CANONICAL)
    ls = g_list_reverse (ls);

  free_mrp_data (mrp);
  return return_and_free_list (message, ls);
}

static DBusMessage *
GetMatchesTo (DBusMessage *message,
              AtkObject *current_object,
              MatchRulePrivate *mrp,
              const AtspiCollectionSortOrder sortby,
              const dbus_bool_t recurse,
              const dbus_bool_t isrestrict,
              dbus_int32_t count,
              const dbus_bool_t traverse)
{
  GList *ls = NULL;
  AtkObject *obj;
  ls = g_list_append (ls, current_object);

  if (recurse)
    {
      obj = ATK_OBJECT (atk_object_get_parent (current_object));
      query_exec (mrp, sortby, ls, 0, count,
                  obj, 0, TRUE, current_object, traverse);
    }
  else
    {
      obj = ATK_OBJECT (spi_register_path_to_object (spi_global_register, dbus_message_get_path (message)));
      query_exec (mrp, sortby, ls, 0, count,
                  obj, 0, TRUE, current_object, traverse);
    }

  ls = g_list_remove (ls, ls->data);

  if (sortby != ATSPI_Collection_SORT_ORDER_REVERSE_CANONICAL)
    ls = g_list_reverse (ls);

  free_mrp_data (mrp);
  return return_and_free_list (message, ls);
}

static DBusMessage *
impl_GetMatchesFrom (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  char *current_object_path = NULL;
  AtkObject *current_object;
  DBusMessageIter iter;
  MatchRulePrivate rule;
  dbus_uint32_t sortby;
  dbus_uint32_t tree;
  dbus_int32_t count;
  dbus_bool_t traverse;
  const char *signature;

  signature = dbus_message_get_signature (message);
  if (strcmp (signature, "o(aiia{ss}iaiiasib)uuib") != 0)
    {
      return droute_invalid_arguments_error (message);
    }

  dbus_message_iter_init (message, &iter);
  dbus_message_iter_get_basic (&iter, &current_object_path);
  current_object = ATK_OBJECT (spi_register_path_to_object (spi_global_register, current_object_path));
  if (!current_object)
    {
      // TODO: object-not-found error
      return spi_dbus_general_error (message);
    }
  dbus_message_iter_next (&iter);
  if (!read_mr (&iter, &rule))
    {
      return spi_dbus_general_error (message);
    }
  dbus_message_iter_get_basic (&iter, &sortby);
  dbus_message_iter_next (&iter);
  dbus_message_iter_get_basic (&iter, &tree);
  dbus_message_iter_next (&iter);
  dbus_message_iter_get_basic (&iter, &count);
  dbus_message_iter_next (&iter);
  dbus_message_iter_get_basic (&iter, &traverse);
  dbus_message_iter_next (&iter);

  switch (tree)
    {
    case ATSPI_Collection_TREE_RESTRICT_CHILDREN:
      return GetMatchesFrom (message, current_object,
                             &rule, sortby, TRUE, count, traverse);
      break;
    case ATSPI_Collection_TREE_RESTRICT_SIBLING:
      return GetMatchesFrom (message, current_object,
                             &rule, sortby, FALSE, count, traverse);
      break;
    case ATSPI_Collection_TREE_INORDER:
      return GetMatchesInOrder (message, current_object,
                                &rule, sortby, TRUE, count, traverse);
      break;
    default:
      return NULL;
    }
}

static DBusMessage *
impl_GetMatchesTo (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  char *current_object_path = NULL;
  AtkObject *current_object;
  DBusMessageIter iter;
  MatchRulePrivate rule;
  dbus_uint32_t sortby;
  dbus_uint32_t tree;
  dbus_bool_t recurse;
  dbus_int32_t count;
  dbus_bool_t traverse;
  const char *signature;

  signature = dbus_message_get_signature (message);
  if (strcmp (signature, "o(aiia{ss}iaiiasib)uubib") != 0)
    {
      return droute_invalid_arguments_error (message);
    }

  dbus_message_iter_init (message, &iter);
  dbus_message_iter_get_basic (&iter, &current_object_path);
  current_object = ATK_OBJECT (spi_register_path_to_object (spi_global_register, current_object_path));
  if (!current_object)
    {
      // TODO: object-not-found error
      return spi_dbus_general_error (message);
    }
  dbus_message_iter_next (&iter);
  if (!read_mr (&iter, &rule))
    {
      return spi_dbus_general_error (message);
    }
  dbus_message_iter_get_basic (&iter, &sortby);
  dbus_message_iter_next (&iter);
  dbus_message_iter_get_basic (&iter, &tree);
  dbus_message_iter_next (&iter);
  dbus_message_iter_get_basic (&iter, &recurse);
  dbus_message_iter_next (&iter);
  dbus_message_iter_get_basic (&iter, &count);
  dbus_message_iter_next (&iter);
  dbus_message_iter_get_basic (&iter, &traverse);
  dbus_message_iter_next (&iter);

  switch (tree)
    {
    case ATSPI_Collection_TREE_RESTRICT_CHILDREN:
      return GetMatchesTo (message, current_object,
                           &rule, sortby, recurse, TRUE, count, traverse);
      break;
    case ATSPI_Collection_TREE_RESTRICT_SIBLING:
      return GetMatchesTo (message, current_object,
                           &rule, sortby, recurse, FALSE, count, traverse);
      break;
    case ATSPI_Collection_TREE_INORDER:
      return GetMatchesInBackOrder (message, current_object,
                                    &rule, sortby, count);
      break;
    default:
      return NULL;
    }
}

static void
append_accessible_properties (DBusMessageIter *iter, AtkObject *obj, GArray *properties)
{
  DBusMessageIter iter_struct, iter_dict, iter_dict_entry;
  AtkStateSet *set;
  gint i;
  gint count;

  dbus_message_iter_open_container (iter, DBUS_TYPE_STRUCT, NULL, &iter_struct);
  spi_object_append_reference (&iter_struct, obj);
  dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "{sv}", &iter_dict);
  if (properties && properties->len)
    {
      gint i;
      for (i = 0; i < properties->len; i++)
        {
          gchar *prop = g_array_index (properties, char *, i);
          DRoutePropertyFunction func;
          GType type;
          func = _atk_bridge_find_property_func (prop, &type);
          if (func && G_TYPE_CHECK_INSTANCE_TYPE (obj, type))
            {
              dbus_message_iter_open_container (&iter_dict, DBUS_TYPE_DICT_ENTRY,
                                                NULL, &iter_dict_entry);
              dbus_message_iter_append_basic (&iter_dict_entry, DBUS_TYPE_STRING, &prop);
              func (&iter_dict_entry, obj);
              dbus_message_iter_close_container (&iter_dict, &iter_dict_entry);
            }
        }
    }
  else
    {
      GHashTableIter hi;
      gpointer key, value;
      g_hash_table_iter_init (&hi, spi_global_app_data->property_hash);
      while (g_hash_table_iter_next (&hi, &key, &value))
        {
          const DRouteProperty *prop = value;
          GType type = _atk_bridge_type_from_iface (key);
          if (!G_TYPE_CHECK_INSTANCE_TYPE (obj, type))
            continue;
          for (; prop->name; prop++)
            {
              const char *p = key + strlen (key);
              gchar *property_name;
              while (p[-1] != '.')
                p--;
              if (!strcmp (p, "Accessible"))
                property_name = g_strdup (prop->name);
              else
                property_name = g_strconcat (p, ".", prop->name, NULL);
              dbus_message_iter_open_container (&iter_dict, DBUS_TYPE_DICT_ENTRY,
                                                NULL, &iter_dict_entry);
              dbus_message_iter_append_basic (&iter_dict_entry, DBUS_TYPE_STRING, &property_name);
              g_free (property_name);
              prop->get (&iter_dict_entry, obj);
              dbus_message_iter_close_container (&iter_dict, &iter_dict_entry);
            }
        }
    }
  dbus_message_iter_close_container (&iter_struct, &iter_dict);
  dbus_message_iter_close_container (iter, &iter_struct);

  set = atk_object_ref_state_set (obj);
  if (set)
    {
      gboolean md = atk_state_set_contains_state (set, ATK_STATE_MANAGES_DESCENDANTS);
      g_object_unref (set);
      if (md)
        return;
    }
  count = atk_object_get_n_accessible_children (obj);
  if (count > ATSPI_MAX_CHILDREN)
    count = ATSPI_MAX_CHILDREN;
  for (i = 0; i < count; i++)
    {
      AtkObject *child = atk_object_ref_accessible_child (obj, i);
      if (child)
        {
          append_accessible_properties (iter, child, properties);
          g_object_unref (child);
        }
    }
}

static DBusMessage *
impl_GetTree (DBusConnection *bus,
              DBusMessage *message,
              void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;
  DBusMessage *reply;
  DBusMessageIter iter, iter_array;
  MatchRulePrivate rule;
  GArray *properties;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data),
                        droute_not_yet_handled_error (message));

  if (strcmp (dbus_message_get_signature (message), "(aiia{ss}iaiiasib)as") != 0)
    return droute_invalid_arguments_error (message);

  properties = g_array_new (TRUE, TRUE, sizeof (char *));
  dbus_message_iter_init (message, &iter);
  if (!read_mr (&iter, &rule))
    {
      return spi_dbus_general_error (message);
    }

  dbus_message_iter_recurse (&iter, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
    {
      const char *prop;
      dbus_message_iter_get_basic (&iter_array, &prop);
      g_array_append_val (properties, prop);
      dbus_message_iter_next (&iter_array);
    }

  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_iter_init_append (reply, &iter);
      dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "((so)a{sv})",
                                        &iter_array);
      append_accessible_properties (&iter_array, object, properties);
      dbus_message_iter_close_container (&iter, &iter_array);
    }
  return reply;
}

static DBusMessage *
impl_GetMatches (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkObject *obj = ATK_OBJECT (spi_register_path_to_object (spi_global_register, dbus_message_get_path (message)));
  DBusMessageIter iter;
  MatchRulePrivate rule;
  dbus_uint32_t sortby;
  dbus_int32_t count;
  dbus_bool_t traverse;
  GList *ls = NULL;
  const char *signature;

  signature = dbus_message_get_signature (message);
  if (strcmp (signature, "(aiia{ss}iaiiasib)uib") != 0)
    {
      return droute_invalid_arguments_error (message);
    }

  dbus_message_iter_init (message, &iter);
  if (!read_mr (&iter, &rule))
    {
      return spi_dbus_general_error (message);
    }
  dbus_message_iter_get_basic (&iter, &sortby);
  dbus_message_iter_next (&iter);
  dbus_message_iter_get_basic (&iter, &count);
  dbus_message_iter_next (&iter);
  dbus_message_iter_get_basic (&iter, &traverse);
  dbus_message_iter_next (&iter);
  ls = g_list_prepend (ls, obj);
  count = query_exec (&rule, sortby, ls, 0, count,
                      obj, 0, TRUE, NULL, traverse);
  ls = g_list_remove (ls, ls->data);

  if (sortby == ATSPI_Collection_SORT_ORDER_REVERSE_CANONICAL)
    ls = g_list_reverse (ls);
  free_mrp_data (&rule);
  return return_and_free_list (message, ls);
}

static dbus_bool_t
impl_get_Version (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_uint32 (iter, SPI_DBUS_COLLECTION_VERSION);
}

static DRouteMethod methods[] = {
  { impl_GetMatchesFrom, "GetMatchesFrom" },
  { impl_GetMatchesTo, "GetMatchesTo" },
  { impl_GetTree, "GetTree" },
  { impl_GetMatches, "GetMatches" },
  { NULL, NULL }
};

static DRouteProperty properties[] = {
  { impl_get_Version, NULL, "version" },
  { NULL, NULL, NULL }
};

void
spi_initialize_collection (DRoutePath *path)
{
  spi_atk_add_interface (path,
                         ATSPI_DBUS_INTERFACE_COLLECTION, spi_org_a11y_atspi_Collection,
                         methods, properties);
};
