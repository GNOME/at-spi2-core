/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
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

#include "droute.h"
#include <stdlib.h>
#include <string.h>

static DRouteInterface *
find_iface (DRouteData * data, const char *name)
{
  GSList *l;

  for (l = data->interfaces; l; l = g_slist_next (l))
    {
      DRouteInterface *iface = (DRouteInterface *) l->data;
      if (iface && iface->name && !strcmp (iface->name, name))
	return iface;
    }
  return NULL;
}

static DBusHandlerResult
prop_get_all (DBusConnection * bus, DBusMessage * message, DRouteData * data)
{
  DRouteInterface *iface_def;
  DRouteProperty *prop;
  DBusError error;
  const char *iface;
  const char *path = dbus_message_get_path (message);
  DBusMessage *reply;
  DBusMessageIter iter, iter_dict, iter_dict_entry;

  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_STRING, &iface, DBUS_TYPE_INVALID))
    {
      return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
  reply = dbus_message_new_method_return (message);
  /* tbd: replace tbd with todo? */
  if (!reply)
    goto oom;
  dbus_message_iter_init_append (reply, &iter);
  if (!dbus_message_iter_open_container
      (&iter, DBUS_TYPE_ARRAY, "{sv}", &iter_dict))
    goto oom;
  iface_def = find_iface (data, iface);
  if (!iface_def)
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  if (iface_def->properties)
    for (prop = iface_def->properties; prop->name; prop++)
      {
	if (!prop->get)
	  continue;
	if (!dbus_message_iter_open_container
	    (&iter_dict, DBUS_TYPE_DICT_ENTRY, NULL, &iter_dict_entry))
	  goto oom;
	dbus_message_iter_append_basic (&iter_dict_entry, DBUS_TYPE_STRING,
					&prop->name);
	(*prop->get) (path, &iter_dict_entry, data->user_data);
	if (!dbus_message_iter_close_container (&iter_dict, &iter_dict_entry))
	  goto oom;
      }
  if (!dbus_message_iter_close_container (&iter, &iter_dict))
    goto oom;
  dbus_connection_send (bus, reply, NULL);
  dbus_message_unref (reply);
  return DBUS_HANDLER_RESULT_HANDLED;
oom:
  /* tbd: return a standard out-of-memory error */
  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static DBusHandlerResult
prop (DBusConnection * bus, DBusMessage * message, DRouteData * data)
{
  const char *mode = dbus_message_get_member (message);
  const char *iface, *member;
  const char *path = dbus_message_get_path (message);
  int set;
  DBusMessage *reply;
  DBusMessageIter iter;
  DRouteInterface *iface_def;
  DRouteProperty *prop = NULL;

  if (!strcmp (mode, "Set"))
    set = 1;
  else if (!strcmp (mode, "Get"))
    set = 0;
  else
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  reply = dbus_message_new_method_return (message);
  dbus_message_iter_init ((set ? reply : message), &iter);
  dbus_message_iter_get_basic (&iter, &iface);
  dbus_message_iter_next (&iter);
  dbus_message_iter_get_basic (&iter, &member);
  iface_def = find_iface (data, iface);
  if (!iface_def)
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  if (iface_def->properties)
    for (prop = iface_def->properties;
	 prop->name && strcmp (prop->name, member); prop++)
      if (!prop || !prop->name)
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  if (set)
    {
      if (!prop->set)
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
      (*prop->set) (path, &iter, data->user_data);
    }
  else
    {
      if (!prop->get)
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
      (*prop->get) (path, &iter, data->user_data);
    }
  dbus_connection_send (bus, reply, NULL);
  dbus_message_unref (message);
  return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult
introspect (DBusConnection * bus, DBusMessage * message, DRouteData * data)
{
  const char *path = dbus_message_get_path (message);
  GString *xml;
  DRouteInterface *iface_def;
  DRouteMethod *method;
  DRouteProperty *property;
  char *str;
  DBusMessage *reply;
  GSList *l;

  if (strcmp (dbus_message_get_member (message), "Introspect"))
    {
      return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
  xml = g_string_new (NULL);
  /* below code stolen from dbus-glib */
  g_string_append (xml, DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE);

  /* We are introspectable, though I guess that was pretty obvious */
  g_string_append_printf (xml, "  <interface name=\"%s\">\n",
			  DBUS_INTERFACE_INTROSPECTABLE);
  g_string_append (xml, "    <method name=\"Introspect\">\n");
  g_string_append_printf (xml,
			  "      <arg name=\"data\" direction=\"out\" type=\"%s\"/>\n",
			  DBUS_TYPE_STRING_AS_STRING);
  g_string_append (xml, "    </method>\n");
  g_string_append (xml, "  </interface>\n");

  /* We support get/set/getall properties */
  g_string_append_printf (xml, "  <interface name=\"%s\">\n",
			  DBUS_INTERFACE_PROPERTIES);
  g_string_append (xml, "    <method name=\"Get\">\n");
  g_string_append_printf (xml,
			  "      <arg name=\"interface\" direction=\"in\" type=\"%s\"/>\n",
			  DBUS_TYPE_STRING_AS_STRING);
  g_string_append_printf (xml,
			  "      <arg name=\"propname\" direction=\"in\" type=\"%s\"/>\n",
			  DBUS_TYPE_STRING_AS_STRING);
  g_string_append_printf (xml,
			  "      <arg name=\"value\" direction=\"out\" type=\"%s\"/>\n",
			  DBUS_TYPE_VARIANT_AS_STRING);
  g_string_append (xml, "    </method>\n");
  g_string_append (xml, "    <method name=\"Set\">\n");
  g_string_append_printf (xml,
			  "      <arg name=\"interface\" direction=\"in\" type=\"%s\"/>\n",
			  DBUS_TYPE_STRING_AS_STRING);
  g_string_append_printf (xml,
			  "      <arg name=\"propname\" direction=\"in\" type=\"%s\"/>\n",
			  DBUS_TYPE_STRING_AS_STRING);
  g_string_append_printf (xml,
			  "      <arg name=\"value\" direction=\"in\" type=\"%s\"/>\n",
			  DBUS_TYPE_VARIANT_AS_STRING);
  g_string_append (xml, "    </method>\n");
  g_string_append (xml, "    <method name=\"GetAll\">\n");
  g_string_append_printf (xml,
			  "      <arg name=\"interface\" direction=\"in\" type=\"%s\"/>\n",
			  DBUS_TYPE_STRING_AS_STRING);
  g_string_append_printf (xml,
			  "      <arg name=\"props\" direction=\"out\" type=\"%s\"/>\n",
			  DBUS_TYPE_ARRAY_AS_STRING
			  DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
			  DBUS_TYPE_STRING_AS_STRING
			  DBUS_TYPE_VARIANT_AS_STRING
			  DBUS_DICT_ENTRY_END_CHAR_AS_STRING);

  g_string_append (xml, "    </method>\n");
  g_string_append (xml, "  </interface>\n");
  g_string_append_printf (xml, "<node name=\"%s\">\n", path);
  /* end of stolen code */

  for (l = data->interfaces; l; l = g_slist_next (l))
    {
      iface_def = (DRouteInterface *) l->data;
      if (!(*iface_def->get_datum) (path, data->user_data))
	continue;
      g_string_append_printf (xml, "<interface name=\"%s\">\n",
			      iface_def->name);
      if (iface_def->methods)
	for (method = iface_def->methods; method->name; method++)
	  {
	    const char *p = method->arg_desc;
	    g_string_append_printf (xml, "<%s name=\"%s\">\n",
				    (method->type ==
				     DROUTE_METHOD ? "method" : "signal"),
				    method->name);
	    g_string_append (xml, "<arg type=\"");
	    g_string_append_len (xml, p, strcspn (p, ",:"));
	    p += strcspn (p, ",");
	    if (*p == ',')
	      p++;
	    g_string_append (xml, "\" name=\"");
	    g_string_append_len (xml, p, strcspn (p, ",:"));
	    p += strcspn (p, ",");
	    if (*p == ',')
	      p++;
	    g_string_append_printf (xml, "\" direction=\"%s\"/>\n",
				    (*p == 'o' ? "out" : "in"));
	    p += strcspn (p, ":");
	    if (*p == ':')
	      p++;
	  }
      if (iface_def->properties)
	for (property = iface_def->properties; property->name; property++)
	  {
	    if (!property->get && !property->set)
	      continue;
	    g_string_append_printf (xml,
				    "<property name=\"%s\" type=\"%s\" access=\"%s%s\"/>\n",
				    property->name, property->type,
				    (property->get ? "read" : ""),
				    (property->set ? "write" : ""));
	  }
    }
  (*data->introspect_children) (path, xml, data->user_data);
  str = g_string_free (xml, FALSE);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &str,
				DBUS_TYPE_INVALID);
    }
  g_free (str);
  dbus_connection_send (bus, reply, NULL);
  dbus_message_unref (reply);
  return DBUS_HANDLER_RESULT_HANDLED;
}

DBusHandlerResult
droute_message (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  DRouteData *data = (DRouteData *) user_data;
  DRouteInterface *iface_def;
  DRouteMethod *method;
  const char *iface = dbus_message_get_interface (message);
  const char *member = dbus_message_get_member (message);
  int type;
  DBusError error;
  DBusMessage *reply;

  dbus_error_init (&error);
  if (!member)
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  if (iface && !strcmp (iface, "org.freedesktop.dbus.Properties"))
    {
      if (!strcmp (member, "GetAll"))
	return prop_get_all (bus, message, data);
      return prop (bus, message, data);
    }
  else if (iface && !strcmp (iface, "org.freedesktop.dbus.Introspectable"))
    {
      return introspect (bus, message, data);
    }
  else
    {
      int message_type = dbus_message_get_type (message);
      switch (message_type)
	{
	case DBUS_MESSAGE_TYPE_METHOD_CALL:
	  type = DROUTE_METHOD;
	  break;
	case DBUS_MESSAGE_TYPE_SIGNAL:
	  type = DROUTE_SIGNAL;
	  break;
	default:
	  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}
    }
  if (iface)
    {
      iface_def = find_iface (data, iface);
      if (!iface_def)
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
      if (iface_def->methods)
	for (method = iface_def->methods; method->func; method++)
	  {
	    if (method->type == type && !strcmp (method->name, member))
	      {
		reply =
		  (*method->func) (bus, message,
				   (method->wants_droute_data ? data : data->
				    user_data));
		if (reply)
		  {
		    dbus_connection_send (bus, reply, NULL);
		    dbus_message_unref (reply);
		  }
		return DBUS_HANDLER_RESULT_HANDLED;
	      }
	  }
    }
  else
    {
      GSList *l;
      if (type == DBUS_MESSAGE_TYPE_SIGNAL)
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
      for (l = data->interfaces; l; l = g_slist_next (l))
	{
	  iface_def = (DRouteInterface *) l->data;
	  if (iface_def->methods)
	    for (method = iface_def->methods; method->func; method++)
	      {
		if (method->type == type && !strcmp (method->name, member))
		  {
		    reply = (*method->func) (bus, message, data->user_data);
		    if (reply)
		      {
			dbus_connection_send (bus, reply, NULL);
			dbus_message_unref (reply);
		      }
		    return DBUS_HANDLER_RESULT_HANDLED;
		  }
	      }
	}
    }
  return DBUS_HANDLER_RESULT_HANDLED;
}

dbus_bool_t
droute_return_v_int32 (DBusMessageIter * iter, dbus_int32_t val)
{
  DBusMessageIter sub;

  if (!dbus_message_iter_open_container
      (iter, DBUS_TYPE_VARIANT, DBUS_TYPE_INT32_AS_STRING, &sub))
    {
      return FALSE;
    }
  dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &val);
  dbus_message_iter_close_container (iter, &sub);
  return TRUE;
}

dbus_bool_t
droute_return_v_double (DBusMessageIter * iter, double val)
{
  DBusMessageIter sub;

  if (!dbus_message_iter_open_container
      (iter, DBUS_TYPE_VARIANT, DBUS_TYPE_DOUBLE_AS_STRING, &sub))
    {
      return FALSE;
    }
  dbus_message_iter_append_basic (&sub, DBUS_TYPE_DOUBLE, &val);
  dbus_message_iter_close_container (iter, &sub);
  return TRUE;
}

/* Return a string in a variant
 * will return an empty string if passed a NULL value */
dbus_bool_t
droute_return_v_string (DBusMessageIter * iter, const char *val)
{
  DBusMessageIter sub;

  if (!val)
    val = "";
  if (!dbus_message_iter_open_container
      (iter, DBUS_TYPE_VARIANT, DBUS_TYPE_STRING_AS_STRING, &sub))
    {
      return FALSE;
    }
  dbus_message_iter_append_basic (&sub, DBUS_TYPE_STRING, &val);
  dbus_message_iter_close_container (iter, &sub);
  return TRUE;
}

dbus_int32_t
droute_get_v_int32 (DBusMessageIter * iter)
{
  DBusMessageIter sub;
  dbus_int32_t rv;

  // TODO- ensure we have the correct type
  dbus_message_iter_recurse (iter, &sub);
  dbus_message_iter_get_basic (&sub, &rv);
  return rv;
}

const char *
droute_get_v_string (DBusMessageIter * iter)
{
  DBusMessageIter sub;
  char *rv;

  // TODO- ensure we have the correct type
  dbus_message_iter_recurse (iter, &sub);
  dbus_message_iter_get_basic (&sub, &rv);
  return rv;
}

dbus_bool_t
droute_return_v_object (DBusMessageIter * iter, const char *path)
{
  DBusMessageIter sub;

  if (!dbus_message_iter_open_container
      (iter, DBUS_TYPE_VARIANT, DBUS_TYPE_OBJECT_PATH_AS_STRING, &sub))
    {
      return FALSE;
    }
  dbus_message_iter_append_basic (&sub, DBUS_TYPE_OBJECT_PATH, &path);
  dbus_message_iter_close_container (iter, &sub);
  return TRUE;
}

dbus_bool_t
droute_add_interface (DRouteData * data, const char *name,
		      DRouteMethod * methods, DRouteProperty * properties,
		      DRouteGetDatumFunction get_datum,
		      DRouteFreeDatumFunction free_datum)
{
  DRouteInterface *iface =
    (DRouteInterface *) malloc (sizeof (DRouteInterface));
  GSList *new_list;

  if (!iface)
    return FALSE;
  iface->name = strdup (name);
  if (!iface->name)
    {
      free (iface);
      return FALSE;
    }
  iface->methods = methods;
  iface->properties = properties;
  iface->get_datum = get_datum;
  iface->free_datum = free_datum;
  new_list = g_slist_append (data->interfaces, iface);
  if (!new_list)
    {
      free (iface->name);
      free (iface);
      return FALSE;
    }
  data->interfaces = new_list;
  return TRUE;
}
