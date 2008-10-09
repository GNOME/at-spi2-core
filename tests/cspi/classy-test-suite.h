/* 
 * Classy Test - Terrible framework for testing asyncronous interface
 *
 * Copyright (C) 2008 Codethink Ltd
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#ifndef CLASSY_TEST_SUITE_H
#define CLASSY_TEST_SUITE_H

#include <glib-object.h>

G_BEGIN_DECLS

GType classy_test_suite_get_type(void);

#define TYPE_CLASSY_TEST_SUITE (classy_test_suite_get_type())

#define CLASSY_TEST_SUITE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),	\
				TYPE_CLASSY_TEST_SUITE,			\
				ClassyTestSuite))

#define CLASSY_TEST_SUITE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),	\
					    TYPE_CLASSY_TEST_SUITE,		\
					    ClassyTestSuiteClass))

#define IS_CLASSY_TEST_SUITE(obj)	   (G_TYPE_CHECK_INSTANCE_TYPE((obj),	\
					    TYPE_CLASSY_TEST_SUITE))

#define IS_CLASSY_TEST_SUITE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),	\
					    TYPE_CLASSY_TEST_SUITE))

#define CLASSY_TEST_SUITE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),	\
					    TYPE_CLASSY_TEST_SUITE,		\
					    ClassyTestSuiteClass))

typedef struct _ClassyTestSuite ClassyTestSuite;
typedef struct _ClassyTestSuiteClass ClassyTestSuiteClass;

struct _ClassyTestSuite {
	ClassyTest parent;

	GArray *cases;
	gint current;
	gboolean cont;

	gpointer data;
};

struct _ClassyTestSuiteClass {
	ClassyTestClass parent;
};

ClassyTestSuite *
classy_test_suite_new(gchar *name, gint dsize, gboolean cont);

void
classy_test_suite_add(ClassyTestSuite *suite, ClassyTest *test);

/*---------------------------------------------------------------------------*/

G_END_DECLS
#endif /* CLASSY_TEST_SUITE_H */
