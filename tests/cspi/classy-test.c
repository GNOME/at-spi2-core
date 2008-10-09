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

#include <glib-object.h>

#include "classy-test.h"

static gchar *
classy_test_report(ClassyTest *test)
{
	switch (classy_test_state(test)) {
		case CLASSY_TEST_FAIL:
			return g_strdup_printf("FAIL       : %s - %s\n ", test->name, test->failm);
		case CLASSY_TEST_WIN:
			return g_strdup_printf("PASS       : %s\n", test->name);
		default:
			return g_strdup_printf("INCOMPLETE : %s\n", test->name);
	}
}

/*---------------------------------------------------------------------------*/

static void
classy_test_run(ClassyTest *test)
{
	test->tstate = CLASSY_TEST_IN_PROGRESS;
	(test->entry)(test, test->data);
}

/*---------------------------------------------------------------------------*/

G_DEFINE_TYPE (ClassyTest, classy_test, G_TYPE_OBJECT)

static void
classy_test_finalize (GObject *obj)
{
	ClassyTest *test = CLASSY_TEST(obj);

	g_free(test->name);
	if (test->failm) {
		g_free(test->failm);
		test->failm = NULL;
	}
	G_OBJECT_CLASS (classy_test_parent_class)->finalize (obj);
}

static void 
classy_test_class_init (ClassyTestClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->finalize = classy_test_finalize;

	klass->report = classy_test_report;
	klass->run = classy_test_run;

	/*Signals*/
	klass->finished = g_signal_newv("finished",
					TYPE_CLASSY_TEST,
					G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS,
					NULL,
					NULL,
					NULL,
					g_cclosure_marshal_VOID__VOID,
					G_TYPE_NONE,
					0,
					NULL);
}

static void
classy_test_init (ClassyTest *test)
{
	test->failm = NULL;
	test->tstate = CLASSY_TEST_NOT_STARTED;
}

/*---------------------------------------------------------------------------*/

ClassyTest *
classy_test_new(gchar *name,
		void (*entry) (ClassyTest*, gpointer data),
		gint istate,
		gpointer data)
{
	ClassyTest *test;

	test = g_object_new(TYPE_CLASSY_TEST, NULL);

	test->name = g_strdup(name);
	test->entry = entry;
	test->data = data;
	
	return test;
}

/*---------------------------------------------------------------------------*/

void
classy_test_pass(ClassyTest *test)
{
	test->tstate = CLASSY_TEST_WIN;
	g_signal_emit (test, CLASSY_TEST_CLASS(test)->finished, 0, NULL);
}

/*---------------------------------------------------------------------------*/

void
classy_test_fail(ClassyTest *test, gchar *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	test->failm = g_strdup_vprintf(fmt, args);
	va_end(args);
	test->tstate = CLASSY_TEST_FAIL;
	g_signal_emit (test, CLASSY_TEST_CLASS(test)->finished, 0, NULL);
}

/*---------------------------------------------------------------------------*/

gint
classy_test_state(ClassyTest *test)
{
	return test->tstate;
}

/*---------------------------------------------------------------------------*/
