#include <droute/droute.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "atspi/atspi.h"

#define TEST_OBJECT_PATH "/test/object"
#define TEST_INTERFACE_ONE "test.interface.One"
#define TEST_INTERFACE_TWO "test.interface.Two"

#define OBJECT_ONE "ObjectOne";
#define OBJECT_TWO "ObjectTwo";

#define STRING_ONE "StringOne"
#define STRING_TWO "StringTwo"

#define INT_ONE 0
#define INT_TWO 456

#define NONE_REPLY_STRING "NoneMethod"

const gchar *test_interface_One =
    "<interface name=\"test.interface.One\">"
    "  <method name=\"null\"/>"
    "  <method name=\"getInt\">"
    "    <arg direction=\"out\" type=\"o\"/>"
    "  </method>"
    "  <method name=\"setInt\">"
    "    <arg direction=\"in\" type=\"o\"/>"
    "  </method>"
    "  <method name=\"getString\">"
    "    <arg direction=\"out\" type=\"s\"/>"
    "  </method>"
    "  <method name=\"setString\">"
    "    <arg direction=\"in\" type=\"s\"/>"
    "  </method>"
    "</interface>";

const gchar *test_interface_Two =
    "<interface name=\"test.interface.One\">"
    "  <method name=\"null\"/>"
    "  <method name=\"getInt\">"
    "    <arg direction=\"out\" type=\"o\"/>"
    "  </method>"
    "  <method name=\"setInt\">"
    "    <arg direction=\"in\" type=\"o\"/>"
    "  </method>"
    "  <method name=\"getString\">"
    "    <arg direction=\"out\" type=\"s\"/>"
    "  </method>"
    "  <method name=\"setString\">"
    "    <arg direction=\"in\" type=\"s\"/>"
    "  </method>"
    "</interface>";

typedef struct _AnObject
{
  gchar *astring;
  guint *anint;
} AnObject;

static DBusConnection *bus;
static GMainLoop *main_loop;
static gboolean success = TRUE;

static DBusMessage *
impl_null (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  DBusMessage *reply;

  reply = dbus_message_new_method_return (message);
  return reply;
}

static DBusMessage *
impl_getInt (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AnObject *object = (AnObject *) user_data;
  DBusMessage *reply;
  DBusError error;

  dbus_error_init (&error);

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_INT32, &(object->anint), DBUS_TYPE_INVALID);
  return reply;
}

static DBusMessage *
impl_setInt (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AnObject *object = (AnObject *) user_data;
  DBusMessage *reply;
  DBusError error;

  dbus_error_init (&error);

  dbus_message_get_args (message, &error, DBUS_TYPE_INT32, &(object->anint), DBUS_TYPE_INVALID);

  reply = dbus_message_new_method_return (message);
  return reply;
}

static DBusMessage *
impl_getString (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AnObject *object = (AnObject *) user_data;
  DBusMessage *reply;
  DBusError error;

  dbus_error_init (&error);

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_STRING, &(object->astring), DBUS_TYPE_INVALID);
  return reply;
}

static DBusMessage *
impl_setString (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AnObject *object = (AnObject *) user_data;
  DBusMessage *reply;
  DBusError error;

  dbus_error_init (&error);

  g_free (object->astring);
  dbus_message_get_args (message, &error, DBUS_TYPE_STRING, &(object->astring), DBUS_TYPE_INVALID);

  reply = dbus_message_new_method_return (message);
  return reply;
}

static DBusMessage *
impl_getInterfaceOne (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  DBusMessage *reply;
  DBusError error;
  gchar *itf = TEST_INTERFACE_ONE;

  dbus_error_init (&error);

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_STRING, &itf, DBUS_TYPE_INVALID);
  return reply;
}

static DBusMessage *
impl_getInterfaceTwo (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  DBusMessage *reply;
  DBusError error;
  gchar *itf = TEST_INTERFACE_TWO;

  dbus_error_init (&error);

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_STRING, &itf, DBUS_TYPE_INVALID);
  return reply;
}

static DRouteMethod test_methods_one[] = {
  { impl_null, "null" },
  { impl_getInt, "getInt" },
  { impl_setInt, "setInt" },
  { impl_getString, "getString" },
  { impl_setString, "setString" },
  { impl_getInterfaceOne, "getInterfaceOne" },
  { NULL, NULL }
};

static DRouteMethod test_methods_two[] = {
  { impl_null, "null" },
  { impl_getInt, "getInt" },
  { impl_setInt, "setInt" },
  { impl_getString, "getString" },
  { impl_setString, "setString" },
  { impl_getInterfaceTwo, "getInterfaceTwo" },
  { NULL, NULL }
};

static DRouteProperty test_properties[] = {
  { NULL, NULL, NULL }
};

static void
set_reply (DBusPendingCall *pending, void *user_data)
{
  void **replyptr = (void **) user_data;

  *replyptr = dbus_pending_call_steal_reply (pending);
  dbus_pending_call_unref (pending);
}

static DBusMessage *
send_and_allow_reentry (DBusConnection *bus, DBusMessage *message, DBusError *error)
{
  DBusPendingCall *pending;
  DBusMessage *reply = NULL;

  if (!dbus_connection_send_with_reply (bus, message, &pending, -1))
    {
      return NULL;
    }
  dbus_pending_call_set_notify (pending, set_reply, (void *) &reply, NULL);
  while (!reply)
    {
      if (!dbus_connection_read_write_dispatch (bus, -1))
        return NULL;
    }
  return reply;
}

gboolean
do_tests_func (gpointer data)
{
  DBusError error;
  const gchar *bus_name;
  DBusMessage *message, *reply;

  gchar *expected_string;
  gchar *result_string;

  dbus_error_init (&error);
  bus_name = dbus_bus_get_unique_name (bus);

  /* --------------------------------------------------------*/

  message = dbus_message_new_method_call (bus_name,
                                          TEST_OBJECT_PATH,
                                          TEST_INTERFACE_ONE,
                                          "null");
  reply = send_and_allow_reentry (bus, message, NULL);
  dbus_message_unref (message);
  if (reply)
    dbus_message_unref (reply);

  /* --------------------------------------------------------*/

  expected_string = TEST_INTERFACE_ONE;
  result_string = NULL;
  message = dbus_message_new_method_call (bus_name,
                                          TEST_OBJECT_PATH,
                                          TEST_INTERFACE_ONE,
                                          "getInterfaceOne");
  reply = send_and_allow_reentry (bus, message, NULL);
  dbus_message_unref (message);
  dbus_message_get_args (reply, NULL, DBUS_TYPE_STRING, &result_string,
                         DBUS_TYPE_INVALID);
  dbus_message_unref (reply);
  if (g_strcmp0 (expected_string, result_string))
    {
      g_print ("Failed: reply to getInterfaceOne was %s; expected %s\n",
               result_string, expected_string);
      exit (1);
    }

  /* --------------------------------------------------------*/

  g_main_loop_quit (main_loop);
  return FALSE;
}

int
main (int argc, char **argv)
{
  DRouteContext *cnx;
  DRoutePath *path;
  AnObject *object;
  DBusError error;

  /* Setup some server object */

  object = g_new0 (AnObject, 1);
  object->astring = g_strdup (STRING_ONE);
  object->anint = INT_ONE;

  dbus_error_init (&error);
  main_loop = g_main_loop_new (NULL, FALSE);
  bus = dbus_bus_get (DBUS_BUS_SESSION, &error);
  atspi_dbus_connection_setup_with_g_main (bus, g_main_context_default ());

  cnx = droute_new ();
  path = droute_add_one (cnx, TEST_OBJECT_PATH, object);

  droute_path_add_interface (path,
                             TEST_INTERFACE_ONE,
                             test_interface_One,
                             test_methods_one,
                             test_properties);

  droute_path_add_interface (path,
                             TEST_INTERFACE_TWO,
                             test_interface_Two,
                             test_methods_two,
                             test_properties);

  droute_path_register (path, bus);

  g_idle_add (do_tests_func, NULL);
  g_main_loop_run (main_loop);

  droute_free (cnx);
  g_free (object->astring);
  g_free (object);

  if (success)
    return 0;
  else
    return 1;
}
