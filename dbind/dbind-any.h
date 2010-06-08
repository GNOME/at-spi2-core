#ifndef _DBIND_ANY_H_
#define _DBIND_ANY_H_

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

unsigned int dbind_find_c_alignment (const char *type);

void   dbind_any_marshal       (DBusMessageIter *iter,
                                const char           **type,
                                void           **val);

void   dbind_any_marshal_va    (DBusMessageIter *iter,
                                const const char           **arg_types,
                                va_list          args);

void   dbind_any_demarshal     (DBusMessageIter *iter,
                                const char           **type,
                                void           **val);

void   dbind_any_demarshal_va  (DBusMessageIter *iter,
                                const char           **arg_types,
                                va_list          args);

void   dbind_any_free          (const char      *type,
                                void            *ptr_to_ptr);

void   dbind_any_free_ptr      (const char      *type,
                                void            *ptr);

#endif /* _DBIND_ANY_H_ */
