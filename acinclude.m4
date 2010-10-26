#######################
# type alignment test #
#######################

AC_DEFUN([DBIND_CHECK_ALIGNOF],
	[changequote(<<, >>)dnl
	dnl The name to #define.
	define(<<AC_TYPE_NAME>>,
		translit(dbind_alignof_$1, [a-z *], [A-Z_P]))dnl
	dnl The cache variable name.
	define(<<AC_CV_NAME>>,
		translit(ac_cv_alignof_$1, [ *], [_p]))dnl
	changequote([, ])dnl
	AC_MSG_CHECKING(alignment of $1)
	AC_CACHE_VAL(AC_CV_NAME,
		[AC_TRY_RUN(
			[ #include <stdio.h>
                          #include <stdlib.h>
                          #define DBUS_API_SUBJECT_TO_CHANGE
			  #include <dbus/dbus.h>
			typedef struct {char s1;} dbind_struct;
			typedef void *dbind_pointer;
			struct test {char s1; $1 s2;};
			main()
			{
			FILE *f=fopen("conftestval", "w");
			if (!f) exit(1);
			fprintf(f, "%d\n", &(((struct test*)0)->s2));
			exit(0);
			} ],
			AC_CV_NAME=`cat conftestval`,
			AC_CV_NAME=0, AC_CV_NAME=0)
		])dnl
	AC_MSG_RESULT($AC_CV_NAME)
	if test "$AC_CV_NAME" = "0" ; then
		AC_MSG_ERROR([Failed to find alignment. Check config.log for details.])
	fi
	AC_TYPE_NAME=$AC_CV_NAME
	AC_SUBST(AC_TYPE_NAME)
	undefine([AC_TYPE_NAME])dnl
	undefine([AC_CV_NAME])dnl
])

