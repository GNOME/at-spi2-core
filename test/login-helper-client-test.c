/* 
 * test utility for LoginHelper client API and services.
 *
 * Copyright Sun Microsystems 2004
 *
 */

#include <X11/X.h>
#include <libbonobo.h>
#include <login-helper/Accessibility_LoginHelper.h>

gint main (int argc, char **argv)
{
    Bonobo_ServerInfoList *server_list;
    Accessibility_LoginHelper *helper_list = NULL;
    CORBA_boolean safe;
    CORBA_Environment ev;
    Accessibility_LoginHelper_WindowList *windows;
    int i;

    CORBA_exception_init (&ev);
    if (!bonobo_init (&argc, argv))
    {
	g_error ("Can't initialize Bonobo");
    }

    /* bonobo-activation query lists existing instances */
    server_list = bonobo_activation_query (
	"(repo_ids.has('IDL:Accessibility/LoginHelper:1.0')) AND _active", 
	NULL, &ev);

    if (BONOBO_EX (&ev)) 
    {
	bonobo_debug_shutdown ();
	g_error ("LoginHelper query failed : %s",
		 bonobo_exception_get_text (&ev));
	/* not reached (below) because g_error exits */
	CORBA_exception_free (&ev);
    }

    g_message ("%d LoginHelpers are running.", 
	       server_list ? server_list->_length : 0);

    helper_list = g_new0 (Accessibility_LoginHelper, server_list->_length);

    /* for each instance... */
    for (i = 0; i < server_list->_length; i++)
    {
	Accessibility_LoginHelper helper;
	Bonobo_Unknown server;
	Bonobo_ServerInfo info = server_list->_buffer[i];

	server = bonobo_activation_activate_from_id (
	    info.iid, Bonobo_ACTIVATION_FLAG_EXISTING_ONLY, NULL, &ev);

	if (BONOBO_EX (&ev))
	{
	    g_warning ("Error activating server %d: %s", i, bonobo_exception_get_text (&ev));
	    CORBA_exception_free (&ev);
	    continue;
	}
	else if (server == CORBA_OBJECT_NIL)
	{
	    g_warning ("Activated server %d is NIL!", i);
	    continue;
	}

	bonobo_activate ();

	helper = Bonobo_Unknown_queryInterface (
	    server, 
	    "IDL:Accessibility/LoginHelper:1.0",
	    &ev);

	if (BONOBO_EX (&ev))
	{
	    g_warning ("Error performing interface query: %s", bonobo_exception_get_text (&ev));
	    CORBA_exception_free (&ev);
	    continue;
	}
	else if (helper == CORBA_OBJECT_NIL)
	{
	    g_warning ("Activated an object which advertised LoginHelper but does not implement it!");
	    continue;
	}
	helper_list[i] = helper;
	bonobo_object_release_unref (server, &ev);

	if (helper && !BONOBO_EX (&ev))
	{
	    /* ask the helper to go into safe mode */
	    safe = Accessibility_LoginHelper_setSafe (helper, TRUE, &ev);
	    if (BONOBO_EX (&ev))
	    {
		g_warning ("setSafe(TRUE) failed: %s", 
			   bonobo_exception_get_text (&ev));
		CORBA_exception_free (&ev);
	    }
	    /* get the raise window list (if the program went into safe mode) */
	    if (safe) 
	    {
		int j;
		gboolean needs_windows_raised = FALSE;
		Accessibility_LoginHelper_DeviceReqList *list;

		g_message ("safe");

		/* does this helper need to have windows raised? */
		list = Accessibility_LoginHelper_getDeviceReqs (helper, &ev);

		if (BONOBO_EX (&ev)) {
		    g_warning ("Bonobo exception getting Device Requirements: %s",
			     bonobo_exception_get_text (&ev));
		    CORBA_exception_free (&ev);
		}
		else
		{
		    g_message ("LoginHelper device requirements: ");
		    if (list->_length == 0) 
			g_message ("\tNone.");

		    for (j = 0; j < list->_length; j++) 
		    {
			switch (list->_buffer[j])
			{
			    case Accessibility_LoginHelper_GUI_EVENTS:
				g_message ("\tNeeds access to the GUI event subsystem (e.g. Xserver)");
				break;
			    case Accessibility_LoginHelper_EXT_INPUT:
				g_message ("\tReads XInput extended input devices");
				break;
			    case Accessibility_LoginHelper_POST_WINDOWS:
				g_message ("\tPosts windows");
				needs_windows_raised = TRUE;
				break;
			    case Accessibility_LoginHelper_AUDIO_OUT:
				g_message ("\tWrites to audio device");
				break;
			    case Accessibility_LoginHelper_AUDIO_IN:
				g_message ("\tReads from audio device");
				break;
			    case Accessibility_LoginHelper_LOCALHOST:
				g_message ("\tNeeds LOCALHOST network connection");
				break;
			    case Accessibility_LoginHelper_SERIAL_OUT:
				g_message ("\tNeeds to write to one or more serial ports");
				break;
			    default:
				break;
			}
		    }
		    CORBA_free (list);
		}
		if (needs_windows_raised) 
		{
		    /* don't raise in this test, but do something with each wid */
		    windows = Accessibility_LoginHelper_getRaiseWindows (helper, &ev);
		    if (BONOBO_EX (&ev))
		    {
			g_warning ("getRaiseWindows failed: %s", 
				   bonobo_exception_get_text (&ev));
			CORBA_exception_free (&ev);
		    }
		    g_message ("%d windows need raising", windows->_length);
		    for (j = 0; j < windows->_length; j++)	
		    {
			Window wid;
			wid = windows->_buffer[j].winID;
			g_message ("Window ID = %ld", (long) wid);
		    }    
		}
	    }
	    else
	    {
		g_warning ("LoginHelper %d did not go into safe mode", i);
	    }
	}
	else
	{
	    if (BONOBO_EX (&ev))
	    {
		g_warning ("Error activating %s: %s", 
			   info.iid, bonobo_exception_get_text (&ev));
		CORBA_exception_free (&ev);
	    }
	    else
	    {
		g_warning ("no active instance of %s found", info.iid);
	    }
	}
    }
    
    /* 
     * In an actual login/authentication setting, login/authentication 
     * would take place here, and code below would be executed after 
     * authentication was complete.
     */
    
    /* now tell the apps they can resume their normal activities */
    for (i = 0; i < server_list->_length; i++)
    {
	Accessibility_LoginHelper helper = helper_list[i];
	/* really no need to check the return value this time */
	Accessibility_LoginHelper_setSafe (helper, FALSE, &ev);
	if (BONOBO_EX (&ev))
	{
	    g_warning ("setSafe(FALSE) failed: %s", 
		       bonobo_exception_get_text (&ev));
	    CORBA_exception_free (&ev);
	}
	CORBA_Object_release (helper, &ev);
    }	
    CORBA_free (server_list);

    return bonobo_debug_shutdown (); /* see if we leaked stuff */
}
