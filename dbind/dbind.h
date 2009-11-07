#ifndef _DBIND_H_
#define _DBIND_H_

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>
#include <dbind/dbind-any.h>

dbus_bool_t
dbind_method_call_reentrant_va (DBusConnection *cnx,
                                const char     *bus_name,
                                const char     *path,
                                const char     *interface,
                                const char     *method,
                                DBusError      *opt_error,
                                const char     *arg_types,
                                va_list         args);

dbus_bool_t
dbind_method_call_reentrant (DBusConnection *cnx,
                             const char     *bus_name,
                             const char     *path,
                             const char     *interface,
                             const char     *method,
                             DBusError      *opt_error,
                             const char     *arg_types,
                             ...);

dbus_bool_t
dbind_emit_signal_va (DBusConnection *cnx,
                      const char     *path,
                      const char     *interface,
                      const char     *signal,
                      DBusError      *opt_error,
                      const char     *arg_types,
                      va_list         args);

dbus_bool_t
dbind_emit_signal (DBusConnection *cnx,
                   const char     *path,
                   const char     *interface,
                   const char     *signal,
                   DBusError      *opt_error,
                   const char     *arg_types,
                   ...);

#endif /* _DBIND_H_ */
