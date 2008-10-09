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
#ifndef CLASSY_TEST_H
#define CLASSY_TEST_H

#include <glib-object.h>

G_BEGIN_DECLS

GType classy_test_get_type(void);

#define TYPE_CLASSY_TEST (classy_test_get_type())

#define CLASSY_TEST(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),	\
			  TYPE_CLASSY_TEST,			\
			  ClassyTest))

#define CLASSY_TEST_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),	\
				      TYPE_CLASSY_TEST,			\
				      ClassyTestClass))

#define IS_CLASSY_TEST(obj)	     (G_TYPE_CHECK_INSTANCE_TYPE((obj),	\
				     TYPE_CLASSY_TEST))

#define IS_CLASSY_TEST_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),	\
				      TYPE_CLASSY_TEST))

#define CLASSY_TEST_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),	\
				      TYPE_CLASSY_TEST,			\
				      ClassyTestClass))

typedef struct _ClassyTest ClassyTest;
typedef struct _ClassyTestClass ClassyTestClass;

enum {
	CLASSY_TEST_NOT_STARTED,
	CLASSY_TEST_IN_PROGRESS,
	CLASSY_TEST_FAIL,
	CLASSY_TEST_WIN
};

struct _ClassyTest {
	GObject parent;

	gchar *name;
	gchar *failm;
	gint tstate;

	void (*entry) (ClassyTest *tc, gpointer data);

	gpointer data;
};

struct _ClassyTestClass {
	GObjectClass parent;

	/*Virtuals*/
	gchar *(*report) (ClassyTest *test);
	void   (*run)    (ClassyTest *test);

	/*Signals*/
	gint finished;
};

ClassyTest *
classy_test_new(gchar *name,
		void (*entry) (ClassyTest*, gpointer data),
		gint istate,
		gpointer data);

void
classy_test_pass(ClassyTest *test);

void
classy_test_fail(ClassyTest *test, gchar *fmt, ...);

gint
classy_test_state(ClassyTest *test);

G_END_DECLS
#endif /* CLASSY_TEST_H */
