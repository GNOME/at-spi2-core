#include <libbonobo.h>
#include <login-helper/login-helper.h>
#include <gtk/gtkwindow.h>
#include <gdk/gdkx.h>
#include <X11/Xatom.h>

static void test_init_login_helper_vpointers (LoginHelper *helper, 
					      gpointer test_set_safe, 
					      gpointer test_get_device_reqs, 
					      gpointer test_get_raise_windows);

static gboolean test_set_safe (LoginHelper *helper, gboolean safe);

static LoginHelperDeviceReqFlags test_get_device_reqs (LoginHelper *helper);

static Window* test_get_raise_windows (LoginHelper *helper);

static void    test_post_window (void);

static void    test_set_wm_dock (void);

static GtkWidget *mainwin = NULL;

int
main (int argc, char **argv)
{
	int          ret;
	char        *obj_id, *display_name;
	BonoboObject *helper;
	
	if (!bonobo_init (&argc, argv))
	{
		g_error ("Could not initialize oaf / Bonobo");
	}
	
	obj_id = "OAFIID:GNOME_TEST:1.0";  /* just for testing, install manually */
	
	helper = BONOBO_OBJECT (g_object_new (LOGIN_HELPER_TYPE, NULL));
	
	ret = bonobo_activation_register_active_server (
		obj_id,
		bonobo_object_corba_objref (bonobo_object (helper)),
		NULL);
	
	if (ret != Bonobo_ACTIVATION_REG_SUCCESS)
	{
		switch (ret) 
		{
		case Bonobo_ACTIVATION_REG_NOT_LISTED:
			fprintf (stderr, "OAFIID not listed\n");
			break;
		default:
			fprintf (stderr, "Registration Error: %d\n", ret);
			break;
		}
	}
	else
	{
		CORBA_Environment ev;
		Bonobo_Unknown ret;
		CORBA_exception_init (&ev);
		
		ret = Bonobo_Unknown_queryInterface (
			BONOBO_OBJREF (helper),
			"IDL:Accessibility/LoginHelper:1.0", 
			&ev);
		
		if (BONOBO_EX (&ev))
		{
			fprintf (stderr, "queryInterface error: %s\n", 
				 bonobo_exception_get_text (&ev));
		}
		else
		{
			fprintf (stderr, "query returned %p: objref self = %p\n", 
				 ret, BONOBO_OBJREF (helper));
		}
	}

	/* this is a testing hack - we are changing the LoginHelperClass's vpointers here */
	
	test_init_login_helper_vpointers ((LoginHelper*)helper, test_set_safe, test_get_device_reqs, test_get_raise_windows);

	gtk_init (&argc, &argv);

	test_post_window ();

	bonobo_main ();

	return 0;
}

static void
test_init_login_helper_vpointers (LoginHelper *helper, 
				  gpointer set_safe_func, 
				  gpointer get_device_reqs_func, 
				  gpointer get_raise_windows_func)
{
	LoginHelperClass *klass = LOGIN_HELPER_GET_CLASS (helper);
	klass->set_safe = set_safe_func;
	klass->get_device_reqs = get_device_reqs_func;
	klass->get_raise_windows = get_raise_windows_func;
}

static gboolean
test_set_safe (LoginHelper *helper, gboolean safe)
{
	return TRUE;
}

static LoginHelperDeviceReqFlags
test_get_device_reqs (LoginHelper *helper)
{
	return LOGIN_HELPER_POST_WINDOWS;
}

static Window*
test_get_raise_windows (LoginHelper *helper)
{
        Window *winlist = g_new0 (Window, 2);
	winlist[0] = GDK_WINDOW_XWINDOW (mainwin->window);
	winlist[1] = None;
        return winlist;
}


void
test_set_wm_dock ()
{
  Atom atom_type[1], atom_window_type;

  gtk_widget_hide (mainwin);

  gdk_error_trap_push ();
  atom_window_type = gdk_x11_get_xatom_by_name ("_NET_WM_WINDOW_TYPE");

  atom_type[0] = gdk_x11_get_xatom_by_name ("_NET_WM_WINDOW_TYPE_DOCK");

  XChangeProperty (GDK_WINDOW_XDISPLAY (mainwin->window), 
		   GDK_WINDOW_XWINDOW (mainwin->window), 
		   atom_window_type,
		   XA_ATOM, 32, PropModeReplace,
		   (guchar *) &atom_type, 1);
  gdk_error_trap_pop ();

  gtk_widget_show (mainwin);

}

static void
test_post_window ()
{
        mainwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	gtk_window_set_screen (GTK_WINDOW (mainwin), gdk_screen_get_default ());

        /* gtk_window_set_keep_above (GTK_WINDOW (mainwin), true); optional */
	/* test_set_wm_dock (); optional */

	gtk_widget_show_all (mainwin);
}
