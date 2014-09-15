#######################
# type alignment test #
#######################

AC_DEFUN([DBIND_CHECK_ALIGNOF],[
AC_CHECK_ALIGNOF($1,[
#include <stdio.h>
#include <stdlib.h>
#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>
typedef struct {char s1;} dbind_struct;
typedef void *dbind_pointer;
])

dnl Note that we substitute for names like @DBIND_ALIGNOF_DBIND_STRUCT@, but
dnl we #define names like ALIGNOF_DBIND_STRUCT in config.h!
dnl
AC_SUBST(translit(dbind_alignof_$1, [a-z *], [A-Z_P]),[$ac_cv_alignof_$1])

])

