/*
 * test-simple.c: A set of simple regression tests
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001 Ximian, Inc.
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

/*
 * ******** Do not copy this code as an example *********
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <cspi/spi.h>
#include <libbonobo.h>

static void validate_accessible (Accessible *accessible,
				 gboolean    has_parent,
				 gboolean    recurse_down);

#define WINDOW_MAGIC 0x123456a
#define TEST_STRING_A "A test string"
#define TEST_STRING_B "Another test string"

static int      print_tree_depth = 0;
static gboolean print_tree = FALSE;

typedef struct {
	gulong     magic;
	GtkWidget *window;
} TestWindow;

static gboolean
focus_me (GtkWidget *widget)
{
	AtkObject *aobject = atk_implementor_ref_accessible (
		ATK_IMPLEMENTOR (widget));

	/* Force a focus event - even if the WM focused
	 * us before our at-bridge's idle handler registered
	 * our interest */
	if (!GTK_WIDGET_HAS_FOCUS (widget))
		gtk_widget_grab_focus (widget);
/*	else: FIXME - gtk_widget_grab_focus should send a notify */
		atk_focus_tracker_notify (aobject);
	
	g_object_unref (G_OBJECT (aobject));

	/* Pull focus away and then back - thus sucks */
	return FALSE;
}

static TestWindow *
create_test_window (void)
{
	TestWindow *win = g_new0 (TestWindow, 1);
	GtkWidget  *widget, *vbox;

	win->magic  = WINDOW_MAGIC;
	win->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
 
	gtk_widget_show (win->window);

	vbox = gtk_vbox_new (0, 0);
	gtk_container_add (GTK_CONTAINER (win->window), vbox);
	gtk_widget_show (vbox);

	widget = gtk_entry_new ();
	gtk_entry_set_text (GTK_ENTRY (widget), TEST_STRING_A);
	gtk_container_add (GTK_CONTAINER (vbox), widget);
	gtk_widget_show (widget);

	g_idle_add ((GSourceFunc) focus_me, win->window);

	return win;
}

static void
test_window_destroy (TestWindow *win)
{
	gtk_widget_destroy (win->window);
	g_free (win);
}

static void
test_roles (void)
{
	int i;

	fprintf (stderr, "Testing roles...\n");
	for (i = -1; i < 1000; i++)
		g_assert (AccessibleRole_getName (i) != NULL);

	g_assert (!strcmp (AccessibleRole_getName (SPI_ROLE_FILE_CHOOSER), "file chooser"));
	g_assert (!strcmp (AccessibleRole_getName (SPI_ROLE_RADIO_BUTTON), "radiobutton"));
	g_assert (!strcmp (AccessibleRole_getName (SPI_ROLE_TABLE), "table"));
	g_assert (!strcmp (AccessibleRole_getName (SPI_ROLE_WINDOW), "window"));
}

static void
test_desktop (void)
{
	Accessible *desktop;
	Accessible *application;

	fprintf (stderr, "Testing desktop...\n");

	g_assert (getDesktop (-1) == NULL);
	desktop = getDesktop (0);
	g_assert (desktop != NULL);

	validate_accessible (desktop, FALSE, FALSE);

	application = Accessible_getChildAtIndex (desktop, 0);
	g_assert (application != NULL);
	AccessibleApplication_unref (application);

	Accessible_unref (desktop);
}

static void
test_application (Accessible *application)
{
	char *str;

	fprintf (stderr, "Testing application ...\n");
	g_assert (Accessible_isApplication (application));
	g_assert (Accessible_getApplication (application) ==
		  application);
	AccessibleApplication_unref (application);

	str = AccessibleApplication_getToolkitName (application);
	g_assert (str != NULL);
	g_assert (!strcmp (str, "GAIL"));
	SPI_freeString (str);

	str = AccessibleApplication_getVersion (application);
	g_assert (str != NULL);
	SPI_freeString (str);

	AccessibleApplication_getID (application);
}

static void
test_editable_text (AccessibleEditableText *etext)
{
	char *str;
	AccessibleText *text;

	fprintf (stderr, "Testing editable text ...\n");
	
	g_assert (Accessible_isText (etext));
	text = Accessible_getText (etext);

	AccessibleEditableText_setTextContents (
		etext, TEST_STRING_B);

	str = AccessibleText_getText (text, 0, -1);
	g_assert (!strcmp (str, TEST_STRING_B));

	SPI_freeString (str);

	/* FIXME: lots more editing here */

	AccessibleEditableText_setTextContents (
		etext, TEST_STRING_A);

	AccessibleText_unref (text);
}

static void
test_text (AccessibleText *text)
{
	char *str;

	fprintf (stderr, "Testing text ...\n");

	g_assert (AccessibleText_getCharacterCount (text) ==
		  strlen (TEST_STRING_A));

	str = AccessibleText_getText (text, 0, -1);
	g_assert (!strcmp (str, TEST_STRING_A));
	SPI_freeString (str);

	str = AccessibleText_getText (text, 0, 5);
	g_assert (!strncmp (str, TEST_STRING_A, 5));
	SPI_freeString (str);

	AccessibleText_setCaretOffset (text, 7);
	g_assert (AccessibleText_getCaretOffset (text) == 7);

	/* FIXME: lots more tests - selections etc. etc. */
}

static void
test_component (AccessibleComponent *component)
{
	long x, y, width, height;

	fprintf (stderr, "Testing component...\n");

	AccessibleComponent_getExtents (
		component, &x, &y, &width, &height, SPI_COORD_TYPE_SCREEN);

	AccessibleComponent_getPosition (
		component, &x, &y, SPI_COORD_TYPE_SCREEN);

	AccessibleComponent_getSize (component, &width, &height);

	if (width > 0 && height > 0) {
#ifdef FIXME
		Accessible *accessible, *componentb;
#endif

		g_assert (AccessibleComponent_contains (
			component, x, y, SPI_COORD_TYPE_SCREEN));

		g_assert (AccessibleComponent_contains (
			component, x + width - 1, y, SPI_COORD_TYPE_SCREEN));

		g_assert (AccessibleComponent_contains (
			component, x + width - 1, y + height - 1,
			SPI_COORD_TYPE_SCREEN));

#ifdef FIXME
		accessible = AccessibleComponent_getAccessibleAtPoint (
			component, x, y, SPI_COORD_TYPE_SCREEN);

		g_assert (Accessible_isComponent (accessible));
		componentb = Accessible_getComponent (accessible);
		g_assert (componentb == component);

		AccessibleComponent_unref (componentb);
		Accessible_unref (accessible);
#endif
	}

	AccessibleComponent_getLayer (component);
	AccessibleComponent_getMDIZOrder (component);
/*	AccessibleComponent_grabFocus (component); */
}

static void
validate_tree (Accessible *accessible,
	       gboolean    has_parent,
	       gboolean    recurse_down)
{
	Accessible  *parent;
	long         len, i;

	parent = Accessible_getParent (accessible);
	if (has_parent) {
		long        index;
		Accessible *child_at_index;

		g_assert (parent != NULL);

		index = Accessible_getIndexInParent (accessible);
		g_assert (index >= 0);

		child_at_index = Accessible_getChildAtIndex (parent, index);

		g_assert (child_at_index == accessible);

		Accessible_unref (child_at_index);
		Accessible_unref (parent);
	}

	len = Accessible_getChildCount (accessible);
	print_tree_depth++;
	for (i = 0; i < len; i++) {
		Accessible *child;

		child = Accessible_getChildAtIndex (accessible, i);
#ifdef ROPEY
		if (!child)
			fprintf (stderr, "Unusual - ChildGone at %ld\n", i);

		g_assert (Accessible_getIndexInParent (child) == i);
		g_assert (Accessible_getParent (child) == accessible);
#endif

		if (recurse_down && child)
			validate_accessible (child, has_parent, recurse_down);

		Accessible_unref (child);
	}
	print_tree_depth--;
}

static void
validate_accessible (Accessible *accessible,
		     gboolean    has_parent,
		     gboolean    recurse_down)
{
	Accessible *tmp;
	char       *name, *descr;
	const char *role;

	name = Accessible_getName (accessible);
	g_assert (name != NULL);
	
	descr = Accessible_getDescription (accessible);
	g_assert (descr != NULL);

	role = Accessible_getRole (accessible);
	g_assert (role != NULL);

	if (print_tree) {
		int i;

		for (i = 0; i < print_tree_depth; i++)
			fputc (' ', stderr);
		fputs ("|-> [ ", stderr);
	}

	if (Accessible_isAction (accessible)) {
		tmp = Accessible_getAction (accessible);
		g_assert (tmp != NULL);
		if (print_tree)
			fprintf (stderr, "At");
		AccessibleAction_unref (tmp);
	}

	if (Accessible_isApplication (accessible)) {
		tmp = Accessible_getApplication (accessible);
		if (print_tree)
			fprintf (stderr, "Ap");
		else
			test_application (tmp);
		AccessibleApplication_unref (tmp);
	}

	if (Accessible_isComponent (accessible)) {
		tmp = Accessible_getComponent (accessible);
		g_assert (tmp != NULL);
		if (print_tree)
			fprintf (stderr, "Co");
		else
			test_component (tmp);
		AccessibleComponent_unref (tmp);
	}

	if (Accessible_isEditableText (accessible)) {
		tmp = Accessible_getEditableText (accessible);
		g_assert (tmp != NULL);
		if (print_tree)
			fprintf (stderr, "Et");
		else
			test_editable_text (tmp);
		AccessibleEditableText_unref (tmp);
	}

	if (Accessible_isHypertext (accessible)) {
		tmp = Accessible_getHypertext (accessible);
		g_assert (tmp != NULL);
		if (print_tree)
			fprintf (stderr, "Ht");
		AccessibleHypertext_unref (tmp);
	}

	if (Accessible_isImage (accessible)) {
		tmp = Accessible_getImage (accessible);
		g_assert (tmp != NULL);
		if (print_tree)
			fprintf (stderr, "Im");
		AccessibleImage_unref (accessible);
	}

	if (Accessible_isSelection (accessible)) {
		tmp = Accessible_getSelection (accessible);
		g_assert (tmp != NULL);
		if (print_tree)
			fprintf (stderr, "Se");
		AccessibleSelection_unref (tmp);
	}

	if (Accessible_isTable (accessible)) {
		tmp = Accessible_getTable (accessible);
		g_assert (tmp != NULL);
		if (print_tree)
			fprintf (stderr, "Ta");
		AccessibleTable_unref (tmp);
	}

	if (Accessible_isText (accessible)) {
		tmp = Accessible_getText (accessible);
		g_assert (tmp != NULL);
		if (print_tree)
			fprintf (stderr, "Te");
		else
			test_text (tmp);
		AccessibleText_unref (tmp);
	}

	if (print_tree)
		fprintf (stderr, " ] '%s' (%s) - %s:\n", name, descr, role);

	SPI_freeString (name);
	SPI_freeString (descr);

	validate_tree (accessible, has_parent, recurse_down);
}

static void
test_misc (void)
{
	fprintf (stderr, "Testing misc bits ...\n");

	g_assert (!Accessible_isComponent (NULL));
	g_assert (Accessible_getComponent (NULL) == NULL);
	SPI_freeString (NULL);
}

static void
global_listener_cb (AccessibleEvent     *event,
		    void                *user_data)
{
	TestWindow *win = user_data;
	Accessible *desktop;
	AccessibleApplication *application;

	g_assert (win->magic == WINDOW_MAGIC);
	g_assert (!strcmp (event->type, "focus:"));

	fprintf (stderr, "Fielded focus event ...\n");

	desktop = getDesktop (0);
	application = Accessible_getChildAtIndex (desktop, 0);
	g_assert (application != NULL);
	Accessible_unref (desktop);

	test_application (application);
	
	AccessibleApplication_unref (application);

	print_tree = TRUE;
	validate_accessible (event->source, TRUE, TRUE);
	print_tree = FALSE;
	validate_accessible (event->source, TRUE, TRUE);

	gtk_main_quit ();
}

int
main (int argc, char **argv)
{
	int leaked;
	TestWindow *win;
	AccessibleEventListener *global_listener;

	setenv ("GTK_MODULES", "gail:at-bridge", FALSE);

	gtk_init (&argc, &argv);

	g_assert (!SPI_init ());
	g_assert (SPI_init ());
	g_assert (getDesktopCount () == 1);

	test_roles ();
	test_misc ();
	test_desktop ();

	win = create_test_window ();

	global_listener = createAccessibleEventListener (global_listener_cb, win);
	g_assert (registerGlobalEventListener (global_listener, "focus:"));

	fprintf (stderr, "Waiting for focus event ...\n");
	gtk_main ();

	g_assert (deregisterGlobalEventListenerAll (global_listener));
	AccessibleEventListener_unref (global_listener);

	test_window_destroy (win);

	if ((leaked = SPI_exit ()))
		g_error ("Leaked %d SPI handles", leaked);

	g_assert (!SPI_exit ());

	fprintf (stderr, "All tests passed\n");

	if (g_getenv ("_MEMPROF_SOCKET")) {
		fprintf (stderr, "Waiting for memprof\n");
		gtk_main ();
	}

	setenv ("AT_BRIDGE_SHUTDOWN", "1", TRUE);

	return 0;
}
