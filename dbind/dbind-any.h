#ifndef _DBIND_ANY_H_
#define _DBIND_ANY_H_

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

unsigned int dbind_find_c_alignment (char *type);

void   dbind_any_marshal       (DBusMessageIter *iter,
                                char           **type,
                                void           **val);

void   dbind_any_marshal_va    (DBusMessageIter *iter,
                                char           **arg_types,
                                va_list          args);

void   dbind_any_demarshal     (DBusMessageIter *iter,
                                char           **type,
                                void           **val);

void   dbind_any_demarshal_va  (DBusMessageIter *iter,
                                char           **arg_types,
                                va_list          args);

void   dbind_any_free          (char            *type,
                                void            *ptr_to_ptr);

void   dbind_any_free_ptr      (char            *type,
                                void            *ptr);

#endif /* _DBIND_ANY_H_ */
