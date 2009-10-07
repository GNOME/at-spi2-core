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
#include <locale.h>
#include <gtk/gtk.h>
#include <cspi/spi.h>
#include <cspi/spi-private.h>

#include "dbus/dbus.h"

/* Known bugs */
#define WHOLE_STRING -1

static void validate_accessible (Accessible *accessible,
				 gboolean    has_parent,
				 gboolean    recurse_down);

#define WINDOW_MAGIC 0x123456a
#define TEST_STRING_A "A test string"
#define TEST_STRING_A_OBJECT "A_test_string_object"
#define TEST_STRING_B "Another test string"

static int      print_tree_depth = 0;
static gboolean print_tree = FALSE;
static gboolean do_poke = FALSE;
static gboolean key_press_received = FALSE;
static gboolean key_release_received = FALSE;

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

	return FALSE;
}

static void
test_window_add_and_show (GtkContainer *container, GtkWidget *widget)
{
	gtk_container_add (container, widget);
	gtk_widget_show (widget);
}

static GtkWidget *
create_tree (void)
{
	GtkWidget         *widget;
	GtkTreeIter        iter;
	GtkListStore      *store;
	GtkTreeViewColumn *column;

	store = gtk_list_store_new (1, G_TYPE_STRING);
	gtk_list_store_append (store, &iter);
	gtk_list_store_set (store, &iter, 0, TEST_STRING_A, -1); 
	column = gtk_tree_view_column_new_with_attributes ("String",
	        gtk_cell_renderer_text_new (), "text", 0, NULL);
	widget = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store)); 
	g_object_unref (G_OBJECT (store));
	gtk_tree_view_append_column (GTK_TREE_VIEW (widget), column);

	return widget;
}

static TestWindow *
create_test_window (void)
{
	TestWindow *win = g_new0 (TestWindow, 1);
	GtkWidget  *widget, *vbox;
	AtkObject *obj;

	win->magic  = WINDOW_MAGIC;
	win->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
 
	gtk_widget_show (win->window);

	vbox = gtk_vbox_new (0, 0);
	gtk_container_add (GTK_CONTAINER (win->window), vbox);
	gtk_widget_show (vbox);

	widget = gtk_entry_new ();
	gtk_entry_set_text (GTK_ENTRY (widget), TEST_STRING_A);
	obj = gtk_widget_get_accessible (widget);
	atk_object_set_name (obj, TEST_STRING_A_OBJECT);

	test_window_add_and_show (GTK_CONTAINER (vbox), widget);

	widget = gtk_button_new_with_label ("_Foobar");
	test_window_add_and_show (GTK_CONTAINER (vbox), widget);

	widget = gtk_hseparator_new ();
	test_window_add_and_show (GTK_CONTAINER (vbox), widget);

	widget = gtk_image_new_from_stock (GTK_STOCK_DIALOG_QUESTION,
					   GTK_ICON_SIZE_LARGE_TOOLBAR);
	test_window_add_and_show (GTK_CONTAINER (vbox), widget);

	widget = g_object_new (GTK_TYPE_HSCALE, NULL);
	gtk_range_set_range (GTK_RANGE (widget), 0.0, 100.0);
	test_window_add_and_show (GTK_CONTAINER (vbox), widget);

	widget = create_tree ();
	test_window_add_and_show (GTK_CONTAINER (vbox), widget);

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

	g_assert (!strcmp (AccessibleRole_getName (SPI_ROLE_FILE_CHOOSER), "file-chooser"));
	g_assert (!strcmp (AccessibleRole_getName (SPI_ROLE_RADIO_BUTTON), "radio-button"));
	g_assert (!strcmp (AccessibleRole_getName (SPI_ROLE_TABLE), "table"));
	g_assert (!strcmp (AccessibleRole_getName (SPI_ROLE_WINDOW), "window"));
}

static void
test_action (AccessibleAction *action)
{
	gint n_actions, i;
	gchar *s, *sd;
	g_assert ((n_actions = AccessibleAction_getNActions (action)) >= 0);

	fprintf (stderr, "Testing actions...");
	for (i = 0; i < n_actions; ++i)
	{
		s = AccessibleAction_getName (action, i);
		g_assert (s);
		sd = AccessibleAction_getDescription (action, i);
		g_assert (sd);
		fprintf (stderr, "%d: %s (%s);  ", i, s, sd);
		SPI_freeString (s);
		SPI_freeString (sd);
		g_assert (AccessibleAction_doAction (action, i));
	}
	fprintf (stderr, "\n");
}

static void
test_desktop (void)
{
	Accessible  *desktop;
	Accessible  *application;
	Accessible **list;

	fprintf (stderr, "Testing desktop...\n");

	g_assert (SPI_getDesktop (-1) == NULL);
	desktop = SPI_getDesktop (0);
	g_assert (desktop != NULL);

	g_assert ((SPI_getDesktopList (&list)) > 0);
	g_assert (list[0] == desktop);
	SPI_freeDesktopList (list);

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

	str = AccessibleApplication_getLocale (application, LC_MESSAGES);
	g_assert (!strcmp (str, setlocale (LC_MESSAGES, NULL)));
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

	str = AccessibleText_getText (text, 0, WHOLE_STRING);
	g_assert (!strcmp (str, TEST_STRING_B));

	SPI_freeString (str);

	/* FIXME: lots more editing here */

	AccessibleEditableText_setTextContents (
		etext, TEST_STRING_A);

	AccessibleText_unref (text);
}

static void
test_table (AccessibleTable *table)
{
	Accessible *header;
	gint index;
	gint rows, columns;

	fprintf (stderr, "Testing table ...\n");

	rows = AccessibleTable_getNRows (table);
	g_assert (rows > 0);

	columns = AccessibleTable_getNColumns (table);
	g_assert (columns > 0);

	index = AccessibleTable_getIndexAt (table, rows - 1, columns - 1);

	g_assert (AccessibleTable_getRowAtIndex (table, index) == rows - 1);

	g_assert (AccessibleTable_getColumnAtIndex (table, index) == columns - 1);

	g_assert ((header = AccessibleTable_getColumnHeader (table, 0)));
	Accessible_unref (header);

	AccessibleTable_isSelected (table, 0, 0);
	
	/* FIXME: lots more tests */
}

static void
test_text (AccessibleText *text)
{
	char *str;

	fprintf (stderr, "Testing text ...\n");

	g_assert (AccessibleText_getCharacterCount (text) ==
		  strlen (TEST_STRING_A));

	str = AccessibleText_getText (text, 0, WHOLE_STRING);
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
test_value (AccessibleValue *value)
{
	float original_value;
	
	fprintf (stderr, "Testing value ...\n");

	original_value = AccessibleValue_getCurrentValue (value);
	
	g_assert (AccessibleValue_getCurrentValue (value) <=
		  AccessibleValue_getMaximumValue (value));

	g_assert (AccessibleValue_getCurrentValue (value) >=
		  AccessibleValue_getMinimumValue (value));

	AccessibleValue_setCurrentValue (value, 
		  AccessibleValue_getMinimumValue (value));
	
	g_assert (AccessibleValue_getCurrentValue (value) ==
		  AccessibleValue_getMinimumValue (value));

	AccessibleValue_setCurrentValue (value, 
		  AccessibleValue_getMaximumValue (value));
	
	g_assert (AccessibleValue_getCurrentValue (value) ==
		  AccessibleValue_getMaximumValue (value));

	AccessibleValue_setCurrentValue (value, original_value);
	
	g_assert (AccessibleValue_getCurrentValue (value) == original_value);
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
test_image (AccessibleImage *image)
{
	char *desc;
	long int x = -1, y = -1, width = -1, height = -1;

	desc = AccessibleImage_getImageDescription (image);
	g_assert (desc != NULL);
	SPI_freeString (desc);

	AccessibleImage_getImagePosition (image, &x, &y,
					  SPI_COORD_TYPE_SCREEN);
	AccessibleImage_getImageSize     (image, &width, &height);
	AccessibleImage_getImageExtents  (image, &x, &y, &width, &height,
					  SPI_COORD_TYPE_WINDOW);
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
	Accessible          *tmp;
	char                *name, *descr;
	AccessibleRole       role;
	AccessibleRelation **relations;
	char                *role_name;
	GString             *item_str = g_string_new ("");
	int                  i;

	name = Accessible_getName (accessible);
	g_assert (name != NULL);
	
	descr = Accessible_getDescription (accessible);
	g_assert (descr != NULL);

	role = Accessible_getRole (accessible);
	g_assert (role != SPI_ROLE_INVALID);
	role_name = Accessible_getRoleName (accessible);
	g_assert (role_name != NULL);

	relations = Accessible_getRelationSet (accessible);
	g_assert (relations != NULL);

	for (i = 0; relations [i]; i++) {
		AccessibleRelationType type;
		int                    targets;

		fprintf (stderr, "relation %d\n", i);

		type = AccessibleRelation_getRelationType (relations [i]);
		g_assert (type != SPI_RELATION_NULL);

		targets = AccessibleRelation_getNTargets (relations [i]);
		g_assert (targets != -1);

		AccessibleRelation_unref (relations [i]);
		relations [i] = NULL;
	}
	free (relations);

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
		else
			test_action (tmp);
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
		if (print_tree) {
			char *desc;

			fprintf (stderr, "Im");

			desc = AccessibleImage_getImageDescription (tmp);
			g_string_append_printf (
				item_str, " image descr: '%s'", desc);
			SPI_freeString (desc);
		} else
			test_image (tmp);

		AccessibleImage_unref (tmp);
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
		else
			test_table (tmp);
		AccessibleTable_unref (tmp);
	}

	if (Accessible_isText (accessible)) {
		tmp = Accessible_getText (accessible);
		g_assert (tmp != NULL);
		if (print_tree)
			fprintf (stderr, "Te");
		else {
			if (strcmp (name, TEST_STRING_A_OBJECT) == 0)	
				test_text (tmp);
		}
		AccessibleText_unref (tmp);
	}

	if (Accessible_isValue (accessible)) {
		tmp = Accessible_getValue (accessible);
		g_assert (tmp != NULL);
		if (print_tree)
			fprintf (stderr, "Va");
		else
			test_value (tmp); 
		AccessibleValue_unref (tmp);
	}

	if (print_tree)
		fprintf (stderr, " ] '%s' (%s) - %s: %s\n",
			 name, descr, role_name, item_str->str);

	SPI_freeString (name);
	SPI_freeString (descr);
	SPI_freeString (role_name);
	g_string_free (item_str, TRUE);

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
global_listener_cb (const AccessibleEvent *event,
		    void                  *user_data)
{
	TestWindow *win = user_data;
	Accessible *desktop;
	AccessibleApplication *application;

	g_assert (win->magic == WINDOW_MAGIC);
	g_assert (!strcmp (event->type, "focus:"));

	fprintf (stderr, "Fielded focus event ...\n");

	if (!do_poke) {
		desktop = SPI_getDesktop (0);
		application = Accessible_getChildAtIndex (desktop, 0);
		g_assert (application != NULL);
		Accessible_unref (desktop);
		
		test_application (application);
		
		AccessibleApplication_unref (application);
		
		print_tree = FALSE;

		validate_accessible (event->source, TRUE, TRUE);

		fprintf (stderr, "quitting mainloop.\n");
		gtk_main_quit ();
	}

	print_tree = TRUE;
	validate_accessible (event->source, TRUE, TRUE);
}

static SPIBoolean
key_listener_cb (const AccessibleKeystroke *stroke,
		 void                      *user_data)
{
	AccessibleKeystroke *s = user_data;
	
	*s = *stroke;
	if (stroke->keystring) s->keystring = g_strdup (stroke->keystring);
	
	if (s->type == SPI_KEY_PRESSED)
		key_press_received = TRUE;
	else if (s->type == SPI_KEY_RELEASED)
		key_release_received = TRUE;

	return TRUE;
}


static void
test_keylisteners (void)
{
	int i;
	AccessibleKeystroke stroke;
	AccessibleKeystrokeListener *key_listener;
	AccessibleKeySet *test_keyset;

	fprintf (stderr, "Testing keyboard listeners ...\n");

	key_listener = SPI_createAccessibleKeystrokeListener (
		key_listener_cb, &stroke);

	test_keyset = SPI_createAccessibleKeySet (1, "=", NULL, NULL);

	g_assert (SPI_registerAccessibleKeystrokeListener (
		key_listener,
		test_keyset,
		0,
		SPI_KEY_PRESSED | SPI_KEY_RELEASED,
		SPI_KEYLISTENER_CANCONSUME | SPI_KEYLISTENER_ALL_WINDOWS));

	for (i = 0; i < 3; i++) {
		memset (&stroke, 0, sizeof (AccessibleKeystroke));
		g_assert (SPI_generateKeyboardEvent ('=', NULL, SPI_KEY_SYM));
		while (!(key_press_received))
			g_main_context_iteration (NULL, TRUE);
		fprintf (stderr, "p [%s]", stroke.keystring);
	        g_assert (!strcmp (stroke.keystring, "="));
		while (!(key_release_received))
			g_main_context_iteration (NULL, TRUE);
		fprintf (stderr, "r [%s]", stroke.keystring);
		key_press_received = FALSE;
		key_release_received = FALSE;
	}
	g_assert (SPI_deregisterAccessibleKeystrokeListener (key_listener, 0));
	SPI_freeAccessibleKeySet (test_keyset);

	fprintf (stderr, "\n");

	AccessibleKeystrokeListener_unref (key_listener);

	g_assert (SPI_generateMouseEvent (100, 100, "rel"));
        g_assert (SPI_generateMouseEvent (-50, -50, "rel"));		  
        g_assert (SPI_generateMouseEvent (-50, -50, "rel"));		  
        g_assert (SPI_generateMouseEvent (-1, -1, "b1c")); 
}

int
main (int argc, char **argv)
{
	int leaked, i;
	TestWindow *win;
	const char *modules;
	AccessibleEventListener *global_listener;

	modules = g_getenv ("GTK_MODULES");
	if (!modules || modules [0] == '\0')
		putenv ("GTK_MODULES=gail:atk-bridge");
	modules = NULL;

	for (i = 1; i < argc; i++) {
		if (!g_strcasecmp (argv [i], "--poke"))
			do_poke = TRUE;
	}

	gtk_init (&argc, &argv);

	g_assert (!SPI_init ());
	g_assert (SPI_init ());
	g_assert (SPI_getDesktopCount () == 1);

	test_roles ();
	test_misc ();
	test_desktop ();
	test_keylisteners (); 

	win = create_test_window ();

	global_listener = SPI_createAccessibleEventListener (global_listener_cb, win);

	g_assert (SPI_registerGlobalEventListener (global_listener, "focus:"));

	fprintf (stderr, "Waiting for focus event ...\n");
	gtk_main ();

	g_assert (SPI_deregisterGlobalEventListenerAll (global_listener));
	AccessibleEventListener_unref (global_listener);

	test_window_destroy (win);

	/* Wait for any pending events from the registry */
	g_usleep (500*1000);
	for (i = 0; i < 100; i++)
		dbus_connection_read_write_dispatch (SPI_bus(), 5);

	if ((leaked = SPI_exit ()))
		g_error ("Leaked %d SPI handles", leaked);

	g_assert (!SPI_exit ());

	fprintf (stderr, "All tests passed\n");

	if (g_getenv ("_MEMPROF_SOCKET")) {
		fprintf (stderr, "Waiting for memprof\n");
		gtk_main ();
	}

	putenv ("AT_BRIDGE_SHUTDOWN=1");

	return 0;
}
