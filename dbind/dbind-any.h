/*
 * Copyright 2008-2011 Novell, Inc.
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

#ifndef _DBIND_ANY_H_
#define _DBIND_ANY_H_

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

unsigned int dbind_find_c_alignment (const char *type);

void dbind_any_marshal (DBusMessageIter *iter,
                        const char **type,
                        void **val);

void dbind_any_marshal_va (DBusMessageIter *iter,
                           const char **arg_types,
                           va_list args);

void dbind_any_demarshal (DBusMessageIter *iter,
                          const char **type,
                          void **val);

void dbind_any_demarshal_va (DBusMessageIter *iter,
                             const char **arg_types,
                             va_list args);

void dbind_any_free (const char *type,
                     void *ptr_to_ptr);

void dbind_any_free_ptr (const char *type,
                         void *ptr);

#endif /* _DBIND_ANY_H_ */
