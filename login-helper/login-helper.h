/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * LoginHelper
 *
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

#ifndef LOGIN_HELPER_H_
#define LOGIN_HELPER_H_

#include <login-helper/Accessibility_LoginHelper.h>
#include <X11/X.h>

G_BEGIN_DECLS

typedef struct _LoginHelper LoginHelper;
typedef struct _LoginHelperClass LoginHelperClass;

#define LOGIN_HELPER_TYPE        (login_helper_get_type ())
#define LOGIN_HELPER(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), LOGIN_HELPER_TYPE, LoginHelper))
#define LOGIN_HELPER_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), LOGIN_HELPER_TYPE, LoginHelperClass))
#define LOGIN_HELPER_GET_CLASS(o)    (G_TYPE_INSTANCE_GET_CLASS((o), LOGIN_HELPER_TYPE, LoginHelperClass))
#define IS_LOGIN_HELPER(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), LOGIN_HELPER_TYPE))
#define IS_LOGIN_HELPER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), LOGIN_HELPER_TYPE))

struct _LoginHelper {
  BonoboObject      parent;

};

typedef unsigned long LoginHelperDeviceReqFlags;

#define LOGIN_HELPER_GUI_EVENTS 1
#define LOGIN_HELPER_CORE_KEYBOARD 2
#define LOGIN_HELPER_CORE_POINTER 4
#define LOGIN_HELPER_EXT_INPUT 8
#define LOGIN_HELPER_POST_WINDOWS 16
#define LOGIN_HELPER_AUDIO_OUT 32
#define LOGIN_HELPER_AUDIO_IN 64
#define LOGIN_HELPER_NETWORK 128
#define LOGIN_HELPER_LOCALHOST 256 
#define LOGIN_HELPER_SERIAL_OUT 512
#define LOGIN_HELPER_SERIAL_IN 1024
#define LOGIN_HELPER_LAST_DEFINED 2048

struct _LoginHelperClass {
  BonoboObjectClass parent_class;

  POA_Accessibility_LoginHelper__epv epv;

  gboolean (*set_safe) (LoginHelper *helper, gboolean safe);
  LoginHelperDeviceReqFlags (*get_device_reqs) (LoginHelper *helper);
  Window*  (*get_raise_windows) (LoginHelper *helper); 
};

GType        login_helper_get_type (void) G_GNUC_CONST;
LoginHelper *login_helper_new      (void);

G_END_DECLS

#endif /* LOGIN_HELPER_H */
