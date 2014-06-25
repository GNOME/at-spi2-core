/*
 * Copyright 2008-2011 Novell, Inc.
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

/* type driven marshalling */
#include <stdio.h>
#include <glib.h>

#include "config.h"
#include "dbind-any.h"

#undef DEBUG

/*  Align a value upward to a boundary, expressed as a number of bytes.
 *  E.g. align to an 8-byte boundary with argument of 8.
 *
 *   (this + boundary - 1)
 *          &
 *    ~(boundary - 1)
 */
#define ALIGN_VALUE(this, boundary) \
  (( ((gulong)(this)) + (((gulong)(boundary)) -1)) & (~(((gulong)(boundary))-1)))

#define ALIGN_ADDRESS(this, boundary) \
  ((gpointer)ALIGN_VALUE(this, boundary))

#define PTR_PLUS(ptr, offset) \
        ((gpointer) (((guchar *)(ptr)) + (offset)))

#define DBIND_POD_CASES \
         DBUS_TYPE_BYTE: \
    case DBUS_TYPE_INT16: \
    case DBUS_TYPE_UINT16: \
    case DBUS_TYPE_INT32: \
    case DBUS_TYPE_UINT32: \
    case DBUS_TYPE_BOOLEAN: \
    case DBUS_TYPE_INT64: \
    case DBUS_TYPE_UINT64: \
    case DBUS_TYPE_DOUBLE

/*---------------------------------------------------------------------------*/

static void
warn_braces ()
{
    fprintf (stderr, "Error: dbus flags structures & dicts with braces rather than "
             " an explicit type member of 'struct'\n");
}

/*---------------------------------------------------------------------------*/

static unsigned int
dbind_find_c_alignment_r (const char **type)
{
    unsigned int retval = 1;

    char t = **type;
    (*type)++;

#ifdef DEBUG
    fprintf (stderr, "\tfind align for %c (0x%x)\n", t, t);
#endif

        switch (t) {
    case DBUS_TYPE_BYTE:
        return ALIGNOF_CHAR;
    case DBUS_TYPE_BOOLEAN:
        return ALIGNOF_DBUS_BOOL_T;
    case DBUS_TYPE_INT16:
    case DBUS_TYPE_UINT16:
        return ALIGNOF_DBUS_INT16_T;
    case DBUS_TYPE_INT32:
    case DBUS_TYPE_UINT32:
        return ALIGNOF_DBUS_INT32_T;
    case DBUS_TYPE_INT64:
    case DBUS_TYPE_UINT64:
        return ALIGNOF_DBUS_INT64_T;
    case DBUS_TYPE_DOUBLE:
        return ALIGNOF_DOUBLE;
    /* ptr types */
    case DBUS_TYPE_STRING:
    case DBUS_TYPE_OBJECT_PATH:
    case DBUS_TYPE_SIGNATURE:
    case DBUS_TYPE_ARRAY:
        return ALIGNOF_DBIND_POINTER;
    case DBUS_STRUCT_BEGIN_CHAR:
      /* TODO: I think this would break with a nested struct */
#if ALIGNOF_DBIND_STRUCT > 1
                retval = MAX (retval, ALIGNOF_DBIND_STRUCT);
#endif
        while (**type != DBUS_STRUCT_END_CHAR) {
            int elem_align = dbind_find_c_alignment_r (type);
                        retval = MAX (retval, elem_align);
        }
        (*type)++;
        return retval;
    case DBUS_DICT_ENTRY_BEGIN_CHAR:
#if ALIGNOF_DBIND_STRUCT > 1
                retval = MAX (retval, ALIGNOF_DBIND_STRUCT);
#endif
        while (**type != DBUS_DICT_ENTRY_END_CHAR) {
            int elem_align = dbind_find_c_alignment_r (type);
                        retval = MAX (retval, elem_align);
        }
        (*type)++;
        return retval;
    case DBUS_TYPE_STRUCT:
    case DBUS_TYPE_DICT_ENTRY:
        warn_braces ();
        return ALIGNOF_DBIND_POINTER;
    case '\0':
        g_assert_not_reached();
        break;
    default:
                return 1;
  }
}

/*---------------------------------------------------------------------------*/

/* gather immediate allocation information for this type */
static size_t
dbind_gather_alloc_info_r (const char **type)
{
  char t = **type;
  (*type)++;
  if (t == DBUS_TYPE_ARRAY)
    {
      switch (**type)
        {
          case DBUS_STRUCT_BEGIN_CHAR:
              while (**type != DBUS_STRUCT_END_CHAR && **type != '\0') (*type)++;
              if (**type != '\0') (*type)++;
              break;
          case DBUS_DICT_ENTRY_BEGIN_CHAR:
              while (**type != DBUS_DICT_ENTRY_END_CHAR && **type != '\0') (*type)++;
              if (**type != '\0') (*type)++;
              break;
          case '\0':
              break;
          default:
              (*type)++;
              break;
        }
    }

  switch (t) {
    case DBUS_TYPE_BYTE:
        return sizeof (char);
    case DBUS_TYPE_BOOLEAN:
        return sizeof (dbus_bool_t);
    case DBUS_TYPE_INT16:
    case DBUS_TYPE_UINT16:
        return sizeof (dbus_int16_t);
    case DBUS_TYPE_INT32:
    case DBUS_TYPE_UINT32:
        return sizeof (dbus_int32_t);
    case DBUS_TYPE_INT64:
    case DBUS_TYPE_UINT64:
        return sizeof (dbus_int64_t);
    case DBUS_TYPE_DOUBLE:
        return sizeof (double);
    /* ptr types */
    case DBUS_TYPE_STRING:
    case DBUS_TYPE_OBJECT_PATH:
    case DBUS_TYPE_SIGNATURE:
    case DBUS_TYPE_ARRAY:
        return sizeof (void *);
    case DBUS_STRUCT_BEGIN_CHAR: {
                int sum = 0, stralign;

        stralign = dbind_find_c_alignment (*type - 1);

        while (**type != DBUS_STRUCT_END_CHAR) {
                        sum = ALIGN_VALUE (sum, dbind_find_c_alignment (*type));
                        sum += dbind_gather_alloc_info_r (type);
        }
                sum = ALIGN_VALUE (sum, stralign);

        g_assert (**type == DBUS_STRUCT_END_CHAR);
        (*type)++;

                return sum;
    }
    case DBUS_DICT_ENTRY_BEGIN_CHAR: {
                int sum = 0, stralign;

        stralign = dbind_find_c_alignment (*type - 1);

        while (**type != DBUS_DICT_ENTRY_END_CHAR) {
                        sum = ALIGN_VALUE (sum, dbind_find_c_alignment (*type));
                        sum += dbind_gather_alloc_info_r (type);
        }
                sum = ALIGN_VALUE (sum, stralign);

        g_assert (**type == DBUS_DICT_ENTRY_END_CHAR);
        (*type)++;

                return sum;
    }
    case DBUS_TYPE_STRUCT:
    case DBUS_TYPE_DICT_ENTRY:
        warn_braces ();
    default:
        return 0;
  }
}

static size_t
dbind_gather_alloc_info (const char *type)
{
  return dbind_gather_alloc_info_r (&type);
}

/*---------------------------------------------------------------------------*/

static void
dbind_any_free_r (const char **type, void **data)
{
#ifdef DEBUG
    fprintf (stderr, "any free '%c' to %p\n", **type, *data);
#endif

    switch (**type) {
    case DBIND_POD_CASES:
        *data = ((guchar *)*data) + dbind_gather_alloc_info (*type);
        (*type)++;
        break;
    case DBUS_TYPE_STRING:
    case DBUS_TYPE_OBJECT_PATH:
    case DBUS_TYPE_SIGNATURE:
#ifdef DEBUG
        fprintf (stderr, "string free %p\n", **(void ***)data);
#endif
        g_free (**(void ***)data);
        *data = ((guchar *)*data) + dbind_gather_alloc_info (*type);
        (*type)++;
        break;
    case DBUS_TYPE_ARRAY: {
        int i;
        GArray *vals = **(void ***)data;
        size_t elem_size, elem_align;
        const char *saved_child_type;

        (*type)++;
        saved_child_type = *type;

        elem_size = dbind_gather_alloc_info (*type);
        elem_align = dbind_find_c_alignment_r (type); 

        for (i = 0; i < vals->len; i++) {
            void *ptr = vals->data + elem_size * i;
            *type = saved_child_type; /* rewind type info */
            ptr = ALIGN_ADDRESS (ptr, elem_align);
            dbind_any_free_r (type, &ptr);
        }
        g_array_free (vals, TRUE);
        break;
    }
    case DBUS_STRUCT_BEGIN_CHAR: {
                gconstpointer data0 = *data;
                int offset = 0, stralign;

        stralign = dbind_find_c_alignment (*type);
        (*type)++;

        offset = 0 ;
        while (**type != DBUS_STRUCT_END_CHAR) {
            const char *subt = *type;
                        offset = ALIGN_VALUE (offset, dbind_find_c_alignment (*type));
                        *data = PTR_PLUS (data0, offset);
            dbind_any_free_r (type, data);
            offset += dbind_gather_alloc_info (subt);
        }

                offset = ALIGN_VALUE (offset, stralign);
                *data = PTR_PLUS (data0, offset);

        g_assert (**type == DBUS_STRUCT_END_CHAR);
        (*type)++;

        break;
    }
    case DBUS_DICT_ENTRY_BEGIN_CHAR: {
                gconstpointer data0 = *data;
                int offset = 0, stralign;

        stralign = dbind_find_c_alignment (*type);
        (*type)++;

        offset = 0 ;
        while (**type != DBUS_DICT_ENTRY_END_CHAR) {
            const char *subt = *type;
                        offset = ALIGN_VALUE (offset, dbind_find_c_alignment (*type));
                        *data = PTR_PLUS (data0, offset);
            dbind_any_free_r (type, data);
            offset += dbind_gather_alloc_info (subt);
        }

                offset = ALIGN_VALUE (offset, stralign);
                *data = PTR_PLUS (data0, offset);

        g_assert (**type == DBUS_DICT_ENTRY_END_CHAR);
        (*type)++;

        break;
    }
    case DBUS_TYPE_STRUCT:
    case DBUS_TYPE_DICT_ENTRY:
        warn_braces ();
        break;
    }
}

/*---------------------------------------------------------------------------*/

void
dbind_any_marshal (DBusMessageIter *iter,
                   const char           **type,
                   void           **data)
{
    size_t len;

#ifdef DEBUG
    fprintf (stderr, "any marshal '%c' to %p\n", **type, *data);
#endif

    switch (**type) {
    case DBIND_POD_CASES:
    case DBUS_TYPE_STRING:
    case DBUS_TYPE_OBJECT_PATH:
    case DBUS_TYPE_SIGNATURE:
        len = dbind_gather_alloc_info (*type);
        dbus_message_iter_append_basic (iter, **type, *data);
        *data = ((guchar *)*data) + len;
        (*type)++;
        break;
    case DBUS_TYPE_ARRAY: {
        int i;
        GArray *vals = **(void ***)data;
        size_t elem_size, elem_align;
        DBusMessageIter sub;
        const char *saved_child_type;
        char *child_type_string;

        (*type)++;
        saved_child_type = *type;

        elem_size = dbind_gather_alloc_info (*type);
        elem_align = dbind_find_c_alignment_r (type); 

        /* wow this part of the API sucks too ... */
        child_type_string = g_strndup (saved_child_type, *type - saved_child_type);
        /* fprintf (stderr, "array child type '%s'\n", child_type_string); */
        dbus_message_iter_open_container (iter, DBUS_TYPE_ARRAY,
                                          child_type_string, &sub);
        for (i = 0; i < vals->len; i++) {
            void *ptr = vals->data + elem_size * i;
            *type = saved_child_type; /* rewind type info */
            ptr = ALIGN_ADDRESS (ptr, elem_align);
            dbind_any_marshal (&sub, type, &ptr);
        }

        dbus_message_iter_close_container (iter, &sub);
        g_free (child_type_string);
        break;
    }
    case DBUS_STRUCT_BEGIN_CHAR: {
                gconstpointer data0 = *data;
                int offset = 0, stralign;
        DBusMessageIter sub;

        stralign = dbind_find_c_alignment (*type);

        (*type)++;

        dbus_message_iter_open_container (iter, DBUS_TYPE_STRUCT, NULL, &sub);

        offset = 0 ;
        while (**type != DBUS_STRUCT_END_CHAR) {
            const char *subt = *type;
                        offset = ALIGN_VALUE (offset, dbind_find_c_alignment (*type));
                        *data = PTR_PLUS (data0, offset);
            dbind_any_marshal (&sub, type, data);
            offset += dbind_gather_alloc_info (subt);
        }

                offset = ALIGN_VALUE (offset, stralign);
                *data = PTR_PLUS (data0, offset);

        dbus_message_iter_close_container (iter, &sub);

        g_assert (**type == DBUS_STRUCT_END_CHAR);
        (*type)++;

        break;
    }
    case DBUS_DICT_ENTRY_BEGIN_CHAR: {
                gconstpointer data0 = *data;
                int offset = 0, stralign;
        DBusMessageIter sub;

        stralign = dbind_find_c_alignment (*type);

        (*type)++;

        dbus_message_iter_open_container (iter, DBUS_TYPE_DICT_ENTRY, NULL, &sub);

        offset = 0 ;
        while (**type != DBUS_DICT_ENTRY_END_CHAR) {
            const char *subt = *type;
                        offset = ALIGN_VALUE (offset, dbind_find_c_alignment (*type));
                        *data = PTR_PLUS (data0, offset);
            dbind_any_marshal (&sub, type, data);
            offset += dbind_gather_alloc_info (subt);
        }

                offset = ALIGN_VALUE (offset, stralign);
                *data = PTR_PLUS (data0, offset);

        dbus_message_iter_close_container (iter, &sub);

        g_assert (**type == DBUS_DICT_ENTRY_END_CHAR);
        (*type)++;

        break;
    }
    case DBUS_TYPE_STRUCT:
    case DBUS_TYPE_DICT_ENTRY:
        warn_braces ();
        break;
    }
}

/*---------------------------------------------------------------------------*/

void
dbind_any_marshal_va (DBusMessageIter *iter,
                      const char           **arg_types,
                      va_list          args)
{
    const char *p = *arg_types;

    /* Guard against null arg types 
       Fix for - http://bugs.freedesktop.org/show_bug.cgi?id=23027
     */
    if (p == NULL)
        p = "";

    {
        /* special case base-types since we need to walk the stack worse-luck */
        for (;*p != '\0' && *p != '=';) {
            int intarg;
            void *ptrarg;
            double doublearg;
            dbus_int64_t int64arg;
            void *arg = NULL;

            switch (*p) {
            case DBUS_TYPE_BYTE:
            case DBUS_TYPE_BOOLEAN:
            case DBUS_TYPE_INT16:
            case DBUS_TYPE_UINT16:
            case DBUS_TYPE_INT32:
            case DBUS_TYPE_UINT32:
                intarg = va_arg (args, int);
                arg = &intarg;
                break;
            case DBUS_TYPE_INT64:
            case DBUS_TYPE_UINT64:
                int64arg = va_arg (args, dbus_int64_t);
                arg = &int64arg;
                break;
            case DBUS_TYPE_DOUBLE:
                doublearg = va_arg (args, double);
                arg = &doublearg;
                break;
            /* ptr types */
            case DBUS_TYPE_STRING:
            case DBUS_TYPE_OBJECT_PATH:
            case DBUS_TYPE_SIGNATURE:
            case DBUS_TYPE_ARRAY:
            case DBUS_TYPE_DICT_ENTRY:
                ptrarg = va_arg (args, void *);
                arg = &ptrarg;
                break;
            case DBUS_STRUCT_BEGIN_CHAR:
                ptrarg = va_arg (args, void *);
                arg = ptrarg;
                break;
            case DBUS_DICT_ENTRY_BEGIN_CHAR:
                ptrarg = va_arg (args, void *);
                arg = ptrarg;
                break;

            case DBUS_TYPE_VARIANT:
                fprintf (stderr, "No variant support yet - very toolkit specific\n");
                ptrarg = va_arg (args, void *);
                arg = &ptrarg;
                break;
            default:
                fprintf (stderr, "Unknown / invalid arg type %c\n", *p);
                break;
            }
            if (arg != NULL)
                dbind_any_marshal (iter, &p, &arg);
            }
        if (*arg_types)
          *arg_types = p;
    }
}

/*---------------------------------------------------------------------------*/

void
dbind_any_demarshal (DBusMessageIter *iter,
                     const char           **type,
                     void           **data)
{
    size_t len;

#ifdef DEBUG
    fprintf (stderr, "any demarshal '%c' to %p\n", **type, *data);
#endif

    switch (**type) {
    case DBIND_POD_CASES:
        len = dbind_gather_alloc_info (*type);
        dbus_message_iter_get_basic (iter, *data);
        *data = ((guchar *)*data) + len;
        (*type)++;
        break;
    case DBUS_TYPE_STRING:
    case DBUS_TYPE_OBJECT_PATH:
    case DBUS_TYPE_SIGNATURE:
        len = dbind_gather_alloc_info (*type);
        dbus_message_iter_get_basic (iter, *data);
#ifdef DEBUG
        fprintf (stderr, "dup string '%s' (%p)\n", **(void ***)data, **(void ***)data);
#endif
        **(void ***)data = g_strdup (**(void ***)data);
        *data = ((guchar *)*data) + len;
        (*type)++;
        break;
    case DBUS_TYPE_ARRAY: {
        GArray *vals;
        DBusMessageIter child;
        size_t elem_size, elem_align;
        const char *stored_child_type;
        int i;

        (*type)++;
        stored_child_type = *type;

        elem_size = dbind_gather_alloc_info (*type);
        elem_align = dbind_find_c_alignment_r (type);
        vals = g_array_new (FALSE, FALSE, elem_size);
        (**(void ***)data) = vals;
        *data = ((guchar *)*data) + sizeof (void *);

        i = 0;
        dbus_message_iter_recurse (iter, &child);
        while (dbus_message_iter_get_arg_type (&child) != DBUS_TYPE_INVALID) {
            void *ptr;
            const char *subt = stored_child_type;
            g_array_set_size (vals, i + 1);
            ptr = vals->data + elem_size * i;
            ptr = ALIGN_ADDRESS (ptr, elem_align);
            dbind_any_demarshal (&child, &subt, &ptr);
            i++;
        };
        break;
    }
    case DBUS_STRUCT_BEGIN_CHAR: {
                gconstpointer data0 = *data;
                int offset = 0, stralign;
        DBusMessageIter child;

        stralign = dbind_find_c_alignment (*type);

        (*type)++;

        dbus_message_iter_recurse (iter, &child);

        while (**type != DBUS_STRUCT_END_CHAR) {
            const char *subt = *type;
                        offset = ALIGN_VALUE (offset, dbind_find_c_alignment (*type));
                        *data = PTR_PLUS (data0, offset);
            dbind_any_demarshal (&child, type, data);
            offset += dbind_gather_alloc_info (subt);
        }

                offset = ALIGN_VALUE (offset, stralign);
                *data = PTR_PLUS (data0, offset);

        g_assert (**type == DBUS_STRUCT_END_CHAR);
        (*type)++;

        break;
    }
    case DBUS_DICT_ENTRY_BEGIN_CHAR: {
                gconstpointer data0 = *data;
                int offset = 0, stralign;
        DBusMessageIter child;

        stralign = dbind_find_c_alignment (*type);

        (*type)++;

        dbus_message_iter_recurse (iter, &child);

        while (**type != DBUS_DICT_ENTRY_END_CHAR) {
            const char *subt = *type;
                        offset = ALIGN_VALUE (offset, dbind_find_c_alignment (*type));
                        *data = PTR_PLUS (data0, offset);
            dbind_any_demarshal (&child, type, data);
            offset += dbind_gather_alloc_info (subt);
        }

                offset = ALIGN_VALUE (offset, stralign);
                *data = PTR_PLUS (data0, offset);

        g_assert (**type == DBUS_DICT_ENTRY_END_CHAR);
        (*type)++;

        break;
    case DBUS_TYPE_VARIANT:
        /* skip; unimplemented for now */
        (*type)++;
        break;
    }
    case DBUS_TYPE_STRUCT:
    case DBUS_TYPE_DICT_ENTRY:
        warn_braces ();
        break;
    }
    dbus_message_iter_next (iter);
}

static const char *
pass_complex_arg (const char *p, char begin, char end)
{
  int level = 1;

  p++;
  while (*p && level > 0)
  {
    if (*p == begin)
      level++;
    else if (*p == end)
      level--;
    p++;
  }
  if (*p == end)
    p++;
  return p;
}

static const char *
pass_arg (const char *p)
{
  switch (*p)
  {
  case '(':
    return pass_complex_arg (p, '(', ')');
  case '{':
    return pass_complex_arg (p, '{', '}');
  case 'a':
    return pass_arg (p+1);
  default:
    return p + 1;
  }
}

/*---------------------------------------------------------------------------*/

void
dbind_any_demarshal_va (DBusMessageIter *iter,
                        const char           **arg_types,
                        va_list          args)
{
    const char *p = *arg_types;

        /* Pass in args */
    for (;*p != '\0' && *p != '=';) {
        int intarg;
        void *ptrarg;
        double doublearg;
        dbus_int64_t int64arg;
        void *arg = NULL;

        switch (*p) {
        case DBUS_TYPE_BYTE:
        case DBUS_TYPE_BOOLEAN:
        case DBUS_TYPE_INT16:
        case DBUS_TYPE_UINT16:
        case DBUS_TYPE_INT32:
        case DBUS_TYPE_UINT32:
            intarg = va_arg (args, int);
            break;
        case DBUS_TYPE_INT64:
        case DBUS_TYPE_UINT64:
            int64arg = va_arg (args, dbus_int64_t);
            break;
        case DBUS_TYPE_DOUBLE:
            doublearg = va_arg (args, double);
            break;
        /* ptr types */
        case DBUS_TYPE_STRING:
        case DBUS_TYPE_OBJECT_PATH:
        case DBUS_TYPE_SIGNATURE:
        case DBUS_TYPE_ARRAY:
        case DBUS_TYPE_DICT_ENTRY:
            ptrarg = va_arg (args, void *);
            break;
        case DBUS_STRUCT_BEGIN_CHAR:
            ptrarg = va_arg (args, void *);
            break;
        case DBUS_DICT_ENTRY_BEGIN_CHAR:
            ptrarg = va_arg (args, void *);
            break;

        case DBUS_TYPE_VARIANT:
            fprintf (stderr, "No variant support yet - very toolkit specific\n");
            ptrarg = va_arg (args, void *);
            break;
        default:
            fprintf (stderr, "Unknown / invalid arg type %c\n", *p);
            break;
        }
      p = pass_arg (p);
    }

    if (p [0] == '=' && p[1] == '>')
      p += 2;

    for (;*p != '\0';) {
        void *arg = va_arg (args, void *);
        dbind_any_demarshal (iter, &p, &arg);
    }
}

/*---------------------------------------------------------------------------*/

/* nice deep free ... */
void
dbind_any_free (const char *type,
                void *ptr)
{
    dbind_any_free_r (&type, &ptr);
}

/* should this be the default normalization ? */
void
dbind_any_free_ptr (const char *type, void *ptr)
{
    dbind_any_free (type, &ptr);
}

/*---------------------------------------------------------------------------*/

unsigned int
dbind_find_c_alignment (const char *type)
{
    return dbind_find_c_alignment_r (&type);
}

/*END------------------------------------------------------------------------*/
