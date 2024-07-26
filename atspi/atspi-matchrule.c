/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 * Copyright 2010, 2011 Novell, Inc.
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

#include "atspi-private.h"

/**
 * AtspiMatchRule:
 *
 * An interface that allows the definition of match rules
 * for accessible objects.
 */

G_DEFINE_TYPE (AtspiMatchRule, atspi_match_rule, G_TYPE_OBJECT)

static void
atspi_match_rule_init (AtspiMatchRule *match_rule)
{
}

static void
atspi_match_rule_dispose (GObject *object)
{
  AtspiMatchRule *rule = ATSPI_MATCH_RULE (object);

  if (rule->states)
    {
      g_object_unref (rule->states);
      rule->states = NULL;
    }

  if (rule->attributes)
    {
      g_hash_table_unref (rule->attributes);
      rule->attributes = NULL;
    }

  G_OBJECT_CLASS (atspi_match_rule_parent_class)->dispose (object);
}

static void
atspi_match_rule_finalize (GObject *object)
{
  AtspiMatchRule *rule = ATSPI_MATCH_RULE (object);
  gint i;

  if (rule->interfaces)
    {
      for (i = 0; i < rule->interfaces->len; i++)
        g_free (g_array_index (rule->interfaces, gchar *, i));
      g_array_free (rule->interfaces, TRUE);
    }

  if (rule->attributes)
    g_hash_table_unref (rule->attributes);

  G_OBJECT_CLASS (atspi_match_rule_parent_class)->finalize (object);
}

static void
atspi_match_rule_class_init (AtspiMatchRuleClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = atspi_match_rule_dispose;
  object_class->finalize = atspi_match_rule_finalize;
}

/**
 * atspi_match_rule_new:
 * @states: (allow-none): An #AtspiStateSet specifying the states to match or
 *          NULL if none.
 * @statematchtype: An #AtspiCollectionMatchType specifying how to interpret
 *                  @states.
 * @attributes: (element-type gchar* gchar*) (allow-none): A #GHashTable
 *              specifying attributes to match. To specify multiple attribute
 *              values, separate each value with a :: If an attribute value
 *              contains a :, then it can be escaped by preceding it with a \.
 *              A backslash can likewise be escaped by inserting a double
 *              backslash.
 * @attributematchtype: An #AtspiCollectionMatchType specifying how to
 *          interpret @attributes.
 * @interfaces: (element-type gchar*) (allow-none): An array of interfaces to
 *              match, or NULL if not applicable.  Interface names should be
 *              specified by the final component of their DBus names
 *              (Accessible, Component, etc). For Action, it is possible to
 *              specify an action name by enclosing it in parenthesis after
 *              the interface name, in which case only accessibles that
 *              implement that particular action will be returned. For
 *              instance, Action(click) will return accessibles that provide
 *              an action called "click".
 * @interfacematchtype: An #AtspiCollectionMatchType specifying how to
 *                      interpret @interfaces.
 * @roles: (element-type AtspiRole) (allow-none): A #GArray of roles to match,
 *         or NULL if not applicable.
 * @rolematchtype: An #AtspiCollectionMatchType specifying how to
 *                 interpret @roles.
 * @invert: if #TRUE, the match rule should be denied (inverted); if #FALSE,
 *          it should not. For example, if the match rule defines that a match is
 *          an object of ROLE_HEADING which has STATE_FOCUSABLE and a click action,
 *          inverting it would match all objects that are not of ROLE_HEADING,
 *          focusable and clickable at the same time.
 *
 * Creates a new #AtspiMatchRule with specified @states, @attributes,
 * @interfaces, and @roles.
 *
 * Returns: (transfer full): A new #AtspiMatchRule.
 **/
AtspiMatchRule *
atspi_match_rule_new (AtspiStateSet *states,
                      AtspiCollectionMatchType statematchtype,
                      GHashTable *attributes,
                      AtspiCollectionMatchType attributematchtype,
                      GArray *roles,
                      AtspiCollectionMatchType rolematchtype,
                      GArray *interfaces,
                      AtspiCollectionMatchType interfacematchtype,
                      gboolean invert)
{
  AtspiMatchRule *rule = g_object_new (ATSPI_TYPE_MATCH_RULE, NULL);
  int i;

  if (states)
    rule->states = g_object_ref (states);
  rule->statematchtype = statematchtype;

  if (attributes)
    {
      GHashTableIter hash_table_iter;
      gchar *key, *value;
      rule->attributes = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                (GDestroyNotify) g_free,
                                                (GDestroyNotify) g_free);
      g_hash_table_iter_init (&hash_table_iter, attributes);
      while (g_hash_table_iter_next (&hash_table_iter, (gpointer *) &key,
                                     (gpointer *) &value))
        g_hash_table_insert (rule->attributes, g_strdup (key), g_strdup (value));
    }
  rule->attributematchtype = attributematchtype;

  if (interfaces)
    {
      rule->interfaces = g_array_new (TRUE, TRUE, sizeof (gchar *));
      for (i = 0; i < interfaces->len; i++)
        {
          gchar *val = g_strdup (g_array_index (interfaces, gchar *, i));
          rule->interfaces = g_array_append_val (rule->interfaces, val);
        }
    }
  rule->interfacematchtype = interfacematchtype;

  if (roles)
    {
      for (i = 0; i < roles->len; i++)
        {
          AtspiRole role = g_array_index (roles, AtspiRole, i);
          if (role < 128)
            rule->roles[role / 32] |= (1 << (role % 32));
          else
            g_warning ("AT-SPI: unexpected role %d\n", role);
        }
    }
  rule->rolematchtype = rolematchtype;

  rule->invert = invert;

  return rule;
}

static void
append_entry (gpointer key, gpointer val, gpointer data)
{
  DBusMessageIter *iter = data;
  DBusMessageIter iter_entry;

  if (!dbus_message_iter_open_container (iter, DBUS_TYPE_DICT_ENTRY, NULL,
                                         &iter_entry))
    return;
  dbus_message_iter_append_basic (&iter_entry, DBUS_TYPE_STRING, &key);
  dbus_message_iter_append_basic (&iter_entry, DBUS_TYPE_STRING, &val);
  dbus_message_iter_close_container (iter, &iter_entry);
}

gboolean
_atspi_match_rule_marshal (AtspiMatchRule *rule, DBusMessageIter *iter)
{
  DBusMessageIter iter_struct, iter_array, iter_dict;
  dbus_int32_t states[2];
  dbus_int32_t d_statematchtype = rule->statematchtype;
  dbus_int32_t d_attributematchtype = rule->attributematchtype;
  dbus_int32_t d_interfacematchtype = rule->interfacematchtype;
  dbus_uint32_t d_rolematchtype = rule->rolematchtype;
  dbus_bool_t d_invert = rule->invert;
  gint i;
  dbus_int32_t d_role;

  if (!dbus_message_iter_open_container (iter, DBUS_TYPE_STRUCT, NULL,
                                         &iter_struct))
    return FALSE;

  /* states */
  if (rule->states)
    {
      states[0] = rule->states->states & 0xffffffff;
      states[1] = rule->states->states >> 32;
    }
  else
    {
      states[0] = states[1] = 0;
    }
  dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "i", &iter_array);
  dbus_message_iter_append_basic (&iter_array, DBUS_TYPE_INT32, &states[0]);
  dbus_message_iter_append_basic (&iter_array, DBUS_TYPE_INT32, &states[1]);
  dbus_message_iter_close_container (&iter_struct, &iter_array);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, &d_statematchtype);

  /* attributes */
  if (!dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "{ss}",
                                         &iter_dict))
    return FALSE;
  if (rule->attributes)
    g_hash_table_foreach (rule->attributes, append_entry, &iter_dict);
  dbus_message_iter_close_container (&iter_struct, &iter_dict);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, &d_attributematchtype);

  if (!dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "i",
                                         &iter_array))
    return FALSE;
  d_role = rule->roles[0];
  dbus_message_iter_append_basic (&iter_array, DBUS_TYPE_INT32, &d_role);
  d_role = rule->roles[1];
  dbus_message_iter_append_basic (&iter_array, DBUS_TYPE_INT32, &d_role);
  d_role = rule->roles[2];
  dbus_message_iter_append_basic (&iter_array, DBUS_TYPE_INT32, &d_role);
  d_role = rule->roles[3];
  dbus_message_iter_append_basic (&iter_array, DBUS_TYPE_INT32, &d_role);
  dbus_message_iter_close_container (&iter_struct, &iter_array);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32,
                                  &d_rolematchtype);

  /* interfaces */
  if (!dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "s",
                                         &iter_array))
    return FALSE;
  if (rule->interfaces)
    {
      for (i = 0; i < rule->interfaces->len; i++)
        {
          char *val = g_array_index (rule->interfaces, gchar *, i);
          dbus_message_iter_append_basic (&iter_array, DBUS_TYPE_STRING, &val);
        }
    }
  dbus_message_iter_close_container (&iter_struct, &iter_array);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, &d_interfacematchtype);

  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_BOOLEAN, &d_invert);

  dbus_message_iter_close_container (iter, &iter_struct);
  return TRUE;
}
