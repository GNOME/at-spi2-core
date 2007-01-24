/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * LoginHelper interface 
 * Copyright 2004 Sun Microsystems Inc.,
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

/* login-helper.c: minimal implementation of Accessibility_LoginHelper.idl */
#include <config.h>
#include <libbonobo.h>
#include "login-helper.h"

/* Our parent Gtk object type */
#define PARENT_TYPE BONOBO_TYPE_OBJECT

/* A pointer to our parent object class */
static BonoboObjectClass *bonobo_object_parent_class;

static void
login_helper_finalize (GObject *object)
{
  (G_OBJECT_CLASS (bonobo_object_parent_class))->finalize (object);
}

static gboolean
login_helper_set_safe (LoginHelper *helper, gboolean safe)
{
  LoginHelperClass *klass = LOGIN_HELPER_GET_CLASS (helper);
  
  if (klass->set_safe)
    return (* klass->set_safe)(helper, safe);
  else
    return FALSE;
}

static CORBA_boolean
impl_login_helper_set_safe (PortableServer_Servant servant,
			    const CORBA_boolean safe,
			    CORBA_Environment *ev)
{
  LoginHelper *helper = LOGIN_HELPER (bonobo_object_from_servant (servant));

  return login_helper_set_safe (helper, safe);
}

static LoginHelperDeviceReqFlags
login_helper_get_device_reqs (LoginHelper *helper)
{
  LoginHelperClass *klass = LOGIN_HELPER_GET_CLASS (helper);
  
  if (klass->get_device_reqs)
    return  (* klass->get_device_reqs)(helper);
  else
    return 0;
}

static Accessibility_LoginHelper_DeviceReqList*
impl_login_helper_get_device_reqs (PortableServer_Servant servant,
				   CORBA_Environment *ev)
{
  LoginHelper *helper = LOGIN_HELPER (bonobo_object_from_servant (servant));
  LoginHelperDeviceReqFlags flags = 0;
  gint n_reqs = 0, i = 1, j = 1;
  Accessibility_LoginHelper_DeviceReqList *list;

  flags = login_helper_get_device_reqs (helper);

  while (i <= LOGIN_HELPER_LAST_DEFINED) 
  {
      if (flags & i) n_reqs++;
      i <<= 1;
  }
  
  list = Accessibility_LoginHelper_DeviceReqList__alloc ();
  list->_length = n_reqs; 
  list->_buffer = Accessibility_LoginHelper_DeviceReqList_allocbuf (list->_length);

  for (i = 0, j = 1; i < n_reqs; i++) 
  {
      Accessibility_LoginHelper_DeviceReq req;
      while ((flags & j) == 0) {
	  j <<= 1;
      }
      switch (j)
      {
	  case LOGIN_HELPER_GUI_EVENTS:
	      req = Accessibility_LoginHelper_GUI_EVENTS;
	      break;
	  case LOGIN_HELPER_CORE_KEYBOARD:
	      req = Accessibility_LoginHelper_CORE_KEYBOARD;
	      break;
	  case LOGIN_HELPER_CORE_POINTER:
	      req = Accessibility_LoginHelper_CORE_POINTER;
	      break;
	  case LOGIN_HELPER_EXT_INPUT:
	      req = Accessibility_LoginHelper_EXT_INPUT;
	      break;
	  case LOGIN_HELPER_POST_WINDOWS:
	      req = Accessibility_LoginHelper_POST_WINDOWS;
	      break;
	  case LOGIN_HELPER_AUDIO_OUT:
	      req = Accessibility_LoginHelper_AUDIO_OUT;
	      break;
	  case LOGIN_HELPER_AUDIO_IN:
	      req = Accessibility_LoginHelper_AUDIO_IN;
	      break;
	  case LOGIN_HELPER_NETWORK:
	      req = Accessibility_LoginHelper_NETWORK;
	      break;
	  case LOGIN_HELPER_LOCALHOST:
	      req = Accessibility_LoginHelper_LOCALHOST;
	      break;
	  case LOGIN_HELPER_SERIAL_OUT:
	      req = Accessibility_LoginHelper_SERIAL_OUT;
	      break;
	  case LOGIN_HELPER_SERIAL_IN:
	      req = Accessibility_LoginHelper_SERIAL_IN;
	      break;
	  default:
	      req = 0;
	      break;
      }
      list->_buffer[i] = req;
      j <<= 1;
  }  

  return list;
}

static Window*
login_helper_get_raise_windows (LoginHelper *helper)
{
  LoginHelperClass *klass = LOGIN_HELPER_GET_CLASS (helper);
  
  if (klass->get_raise_windows)
    return (* klass->get_raise_windows)(helper);
  else
    return NULL;
}

static Accessibility_LoginHelper_WindowList*
impl_login_helper_get_raise_windows (PortableServer_Servant servant,
				     CORBA_Environment *ev)
{
  LoginHelper *helper = LOGIN_HELPER (bonobo_object_from_servant (servant));
  unsigned long *wids;
  Accessibility_LoginHelper_WindowList *windows;

  wids = login_helper_get_raise_windows (helper);

  if (wids && *wids)
    {
      gint i, count = 0;
      while (wids[count])
	{
	  ++count;
	}
      windows = Accessibility_LoginHelper_WindowList__alloc ();
      windows->_length = count;
      windows->_buffer = Accessibility_LoginHelper_WindowList_allocbuf (windows->_length);
      for (i = 0; i < count; ++i)
	{
          Accessibility_LoginHelper_WindowInfo winfo;
	  winfo.winID = wids[i];
	  windows->_buffer [i] = winfo;
	}
      g_free (wids);
      return windows;
    }
  else
    {
      return NULL;
    }
}

static void
login_helper_class_init (LoginHelperClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_LoginHelper__epv *epv = &klass->epv;

  bonobo_object_parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = login_helper_finalize;
  
  epv->getRaiseWindows = impl_login_helper_get_raise_windows;
  epv->getDeviceReqs = impl_login_helper_get_device_reqs;
  epv->setSafe = impl_login_helper_set_safe;
}

static void
login_helper_init (GObject *object)
{
}

BONOBO_TYPE_FUNC_FULL (LoginHelper,
		       Accessibility_LoginHelper,
		       PARENT_TYPE, login_helper)

