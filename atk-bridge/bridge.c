/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <libbonobo.h>
#include <orbit/orbit.h>
#include <atk/atk.h>
#include <atk/atkobject.h>
#include <atk/atknoopobject.h>
#include <libspi/Accessibility.h>
#include "accessible.h"
#include "application.h"

#include <bonobo-activation/bonobo-activation-register.h>

#undef SPI_BRIDGE_DEBUG

#define DBG(a,b) if(_dbg>=(a))b

static int _dbg = 0;
static char *spi_nil_string = "";

static CORBA_Environment ev;
static Accessibility_Registry registry = NULL;
static SpiApplication *this_app = NULL;
static gboolean registry_died = FALSE;
static guint toplevel_handler;

/* NOT YET USED
   static GQuark atk_quark_property_changed_name;
   static GQuark atk_quark_property_changed_description;
   static GQuark atk_quark_property_changed_parent;
   static GQuark atk_quark_property_changed_role;
   static GQuark atk_quark_property_changed_table_caption;
   static GQuark atk_quark_property_changed_table_column_description;
   static GQuark atk_quark_property_changed_table_row_description;
   static guint atk_signal_property_changed;
*/

static guint atk_signal_text_changed;
static guint atk_signal_child_changed;

/* NOT YET USED
   static guint atk_signal_text_selection_changed;
   static guint atk_signal_active_descendant_changed;
   static guint atk_signal_row_reordered;
   static guint atk_signal_row_inserted;
   static guint atk_signal_row_deleted;
   static guint atk_signal_column_reordered;
   static guint atk_signal_column_inserted;
   static guint atk_signal_column_deleted;
*/

static Accessibility_Registry spi_atk_bridge_get_registry (void);
static void     spi_atk_bridge_do_registration         (void);
static void     spi_atk_bridge_toplevel_added          (AtkObject             *object,
                                                        guint                 index,
                                                        AtkObject             *child);

static void     spi_atk_bridge_exit_func               (void);
static void     spi_atk_register_event_listeners       (void);
static void     spi_atk_bridge_focus_tracker           (AtkObject             *object);
static void     spi_atk_bridge_register_application    (Accessibility_Registry registry);
static gboolean spi_atk_bridge_property_event_listener (GSignalInvocationHint *signal_hint,
							guint                  n_param_values,
							const GValue          *param_values,
							gpointer               data);
static gboolean
spi_atk_bridge_window_event_listener (GSignalInvocationHint *signal_hint,
				guint n_param_values,
				const GValue *param_values,
				gpointer data);
static gboolean
spi_atk_bridge_state_event_listener (GSignalInvocationHint *signal_hint,
				     guint n_param_values,
				     const GValue *param_values,
				     gpointer data);
static gboolean spi_atk_bridge_signal_listener         (GSignalInvocationHint *signal_hint,
							guint                  n_param_values,
							const GValue          *param_values,
							gpointer               data);
static gint     spi_atk_bridge_key_listener            (AtkKeyEventStruct     *event,
							gpointer               data);

/* For automatic libgnome init */
extern void gnome_accessibility_module_init     (void);
extern void gnome_accessibility_module_shutdown (void);

static int     atk_bridge_initialized = FALSE;
static guint   atk_bridge_focus_tracker_id = 0;
static guint   atk_bridge_key_event_listener_id = 0;
static GArray *listener_ids = NULL;

/*
 *   These exported symbols are hooked by gnome-program
 * to provide automatic module initialization and shutdown.
 */
extern void gnome_accessibility_module_init     (void);
extern void gnome_accessibility_module_shutdown (void);

static void
spi_atk_bridge_init_event_type_consts ()
{
  atk_signal_child_changed = g_signal_lookup ("child_changed", 
					      ATK_TYPE_OBJECT);
  atk_signal_text_changed = g_signal_lookup ("text_changed", 
					     ATK_TYPE_TEXT);
}

static int
atk_bridge_init (gint *argc, gchar **argv[])
{
  const char *debug_env_string = g_getenv ("AT_SPI_DEBUG");

  if (atk_bridge_initialized)
    {
      return 0;
    }
  atk_bridge_initialized = TRUE;

  if (debug_env_string)
    _dbg = (int) g_ascii_strtod (debug_env_string, NULL);

  if (!bonobo_init (argc, argv ? *argv : NULL))
    {
      g_error ("Could not initialize Bonobo");
    }

  /*
   * We only want to enable the bridge for top level
   * applications, we detect bonobo components by seeing
   * if they were activated with the intention of extracting
   * an impl. by IID - very solid.
   */
  if (bonobo_activation_iid_get ())
    {
      DBG (1, g_message ("Found Bonobo component\n"));
      toplevel_handler = g_signal_connect (atk_get_root (), 
                                           "children-changed::add",
                                           (GCallback) spi_atk_bridge_toplevel_added, 
                                           NULL);
    }
  else
    {
      spi_atk_bridge_do_registration ();
    }
 
  spi_atk_bridge_init_event_type_consts ();

  return 0;
}


static void
spi_atk_bridge_do_registration (void)
{
  CORBA_Environment ev;

  CORBA_exception_init(&ev);

  if (spi_atk_bridge_get_registry () == CORBA_OBJECT_NIL)
    {
      g_error ("Could not locate registry");
    }

  bonobo_activate ();

  /* Create the accessible application server object */

  this_app = spi_application_new (atk_get_root ());

  DBG (1, g_message ("About to register application\n"));

  spi_atk_bridge_register_application (spi_atk_bridge_get_registry ());
  
  g_atexit (spi_atk_bridge_exit_func);

  DBG (1, g_message ("Application registered & listening\n"));

}

static void
spi_atk_bridge_toplevel_added (AtkObject *object,
                               guint     index,
                               AtkObject *child)
{
  g_signal_handler_disconnect (object, toplevel_handler);
  spi_atk_bridge_do_registration ();
}

static void
spi_atk_bridge_register_application (Accessibility_Registry registry)
{
  Accessibility_Registry_registerApplication (spi_atk_bridge_get_registry (),
                                              BONOBO_OBJREF (this_app),
                                              &ev);
  spi_atk_register_event_listeners ();
}

static Accessibility_Registry
spi_atk_bridge_get_registry ()
{
  CORBA_Environment ev;

  if (registry_died || (registry == NULL)) {
	  CORBA_exception_init (&ev);
	  if (registry_died) 
	    DBG (1, g_warning ("registry died! restarting..."));
	  registry = bonobo_activation_activate_from_id (
		  "OAFIID:Accessibility_Registry:1.0", 0, NULL, &ev);
	  
	  if (ev._major != CORBA_NO_EXCEPTION)
	  {
		  g_error ("Accessibility app error: exception during "
			   "registry activation from id: %s\n",
			   CORBA_exception_id (&ev));
		  CORBA_exception_free (&ev);
	  }
	  
	  if (registry_died && registry) {
		  registry_died = FALSE;
		  spi_atk_bridge_register_application (registry);
	  }
  }
  return registry;
}

int
gtk_module_init (gint *argc, gchar **argv[])
{
	return atk_bridge_init (argc, argv);
}

static void
add_signal_listener (const char *signal_name)
{
  guint id;

  id = atk_add_global_event_listener (
    spi_atk_bridge_signal_listener, signal_name);

  g_array_append_val (listener_ids, id);
}

static void
spi_atk_register_event_listeners (void)
{
  /*
   * kludge to make sure the Atk interface types are registered, otherwise
   * the AtkText signal handlers below won't get registered
   */
  guint      id;
  GObject   *ao = g_object_new (ATK_TYPE_OBJECT, NULL);
  AtkObject *bo = atk_no_op_object_new (ao);
  
  /* Register for focus event notifications, and register app with central registry  */

  listener_ids = g_array_sized_new (FALSE, TRUE, sizeof (guint), 16);

  atk_bridge_focus_tracker_id = atk_add_focus_tracker (spi_atk_bridge_focus_tracker);

  id = atk_add_global_event_listener (spi_atk_bridge_property_event_listener,
				      "Gtk:AtkObject:property-change");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_window_event_listener,
				      "window:create");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_window_event_listener,
				      "window:destroy");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_window_event_listener,
				      "window:minimize");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_window_event_listener,
				      "window:maximize");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_window_event_listener,
				      "window:restore");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_window_event_listener,
				      "window:activate");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_window_event_listener,
				      "window:deactivate");
  g_array_append_val (listener_ids, id);
  id = atk_add_global_event_listener (spi_atk_bridge_state_event_listener,
				      "Gtk:AtkObject:state-change");
  g_array_append_val (listener_ids, id);

  add_signal_listener ("Gtk:AtkObject:children-changed");
  add_signal_listener ("Gtk:AtkObject:visible-data-changed");
  add_signal_listener ("Gtk:AtkSelection:selection-changed");
  add_signal_listener ("Gtk:AtkText:text-selection-changed");
  add_signal_listener ("Gtk:AtkText:text-changed");
  add_signal_listener ("Gtk:AtkText:text-caret-moved");
  add_signal_listener ("Gtk:AtkTable:row-inserted");
  add_signal_listener ("Gtk:AtkTable:row-reordered");
  add_signal_listener ("Gtk:AtkTable:row-deleted");
  add_signal_listener ("Gtk:AtkTable:column-inserted");
  add_signal_listener ("Gtk:AtkTable:column-reordered");
  add_signal_listener ("Gtk:AtkTable:column-deleted");
  add_signal_listener ("Gtk:AtkTable:model-changed");
/*
 * May add the following listeners to implement preemptive key listening for GTK+
 *
 * atk_add_global_event_listener (spi_atk_bridge_widgetkey_listener, "Gtk:GtkWidget:key-press-event");
 * atk_add_global_event_listener (spi_atk_bridge_widgetkey_listener, "Gtk:GtkWidget:key-release-event");
 */
  atk_bridge_key_event_listener_id = atk_add_key_event_listener (
    spi_atk_bridge_key_listener, NULL);
  
  g_object_unref (G_OBJECT (bo));
  g_object_unref (ao);
}

static void
deregister_application (BonoboObject *app)
{
  Accessibility_Registry registry = spi_atk_bridge_get_registry ();	
  Accessibility_Registry_deregisterApplication (registry, BONOBO_OBJREF (app), &ev);

  registry = bonobo_object_release_unref (registry, &ev);
  
  app = bonobo_object_unref (app);
}

static void
spi_atk_bridge_exit_func (void)
{
  BonoboObject *app = (BonoboObject *) this_app;

  DBG (1, g_message ("exiting bridge\n"));

  if (!app)
    {
      return;
    }
  this_app = NULL;

  /*
   *  FIXME: this may be incorrect for apps that do their own bonobo
   *  shutdown, until we can explicitly shutdown to get the ordering
   *  right.
   */
  if (!bonobo_is_initialized ())
    {
      DBG (1, g_warning ("Re-initializing bonobo\n"));
      g_assert (bonobo_init (0, NULL));
      g_assert (bonobo_activate ());
    }
  
  deregister_application (app);

  DBG (1, g_message ("bridge exit func complete.\n"));

  if (g_getenv ("AT_BRIDGE_SHUTDOWN"))
    {
      g_assert (!bonobo_debug_shutdown ());
    }
}

void
gnome_accessibility_module_init (void)
{
  atk_bridge_init (NULL, NULL);

  g_print("Atk Accessibilty bridge initialized\n");
}

void
gnome_accessibility_module_shutdown (void)
{
  BonoboObject *app = (BonoboObject *) this_app;
  int     i;
  GArray *ids = listener_ids;
  
  if (!atk_bridge_initialized)
    {
      return;
    }
  atk_bridge_initialized = FALSE;
  this_app = NULL;

  g_print("Atk Accessibilty bridge shutdown\n");

  listener_ids = NULL;
  atk_remove_focus_tracker (atk_bridge_focus_tracker_id);
  
  for (i = 0; ids && i < ids->len; i++)
  {
          atk_remove_global_event_listener (g_array_index (ids, guint, i));
  }
  
  atk_remove_key_event_listener (atk_bridge_key_event_listener_id);

  deregister_application (app);
}

static void
spi_atk_bridge_focus_tracker (AtkObject *object)
{
  SpiAccessible *source;
  Accessibility_Event e;

  source = spi_accessible_new (object);

  e.type = "focus:";
  e.source = BONOBO_OBJREF (source);
  e.detail1 = 0;
  e.detail2 = 0;
  spi_init_any_nil (&e.any_data);
  Accessibility_Registry_notifyEvent (spi_atk_bridge_get_registry (), &e, &ev);
  if (BONOBO_EX (&ev)) registry_died = TRUE;
  
  Accessibility_Accessible_unref (e.source, &ev);
  
  CORBA_exception_free (&ev);
}

static void
spi_atk_emit_eventv (const GObject         *gobject,
		     long                   detail1,
		     long                   detail2,
		     CORBA_any             *any,
		     const char            *format, ...)
{
  va_list             args;
  Accessibility_Event e;
  SpiAccessible      *source;
  AtkObject          *aobject;
#ifdef SPI_BRIDGE_DEBUG
  CORBA_string s;
#endif
  
  va_start (args, format);
  
  if (ATK_IS_IMPLEMENTOR (gobject))
    {
      aobject = atk_implementor_ref_accessible (ATK_IMPLEMENTOR (gobject));
      source  = spi_accessible_new (aobject);
      g_object_unref (G_OBJECT (aobject));
    }
  else if (ATK_IS_OBJECT (gobject))
    {
      aobject = ATK_OBJECT (gobject);
      source  = spi_accessible_new (aobject);
    }
  else
    {
      aobject = NULL;
      source  = NULL;
      DBG (0, g_warning ("received property-change event from non-AtkImplementor"));
    }

  if (source) 
    {
      e.type = g_strdup_vprintf (format, args);
      e.source = BONOBO_OBJREF (source);
      e.detail1 = detail1;
      e.detail2 = detail2;
      if (any) e.any_data = *any;
      else spi_init_any_nil (&e.any_data);
      
#ifdef SPI_BRIDGE_DEBUG
      s = Accessibility_Accessible__get_name (BONOBO_OBJREF (source), &ev);
      g_warning ("Emitting event '%s' (%lu, %lu) on %s",
		 e.type, e.detail1, e.detail2, s);
      CORBA_free (s);
#endif
      CORBA_exception_init (&ev);
      Accessibility_Registry_notifyEvent (spi_atk_bridge_get_registry (), 
					  &e, &ev);
#ifdef SPI_BRIDGE_DEBUG
      if (ev._major != CORBA_NO_EXCEPTION)
	g_warning ("error emitting event %s, (%d) %s",
		   e.type,
		   ev._major,
		   CORBA_exception_id(&ev));
#endif	      
      if (BONOBO_EX (&ev)) registry_died = TRUE;
      Accessibility_Accessible_unref (e.source, &ev);
      
      CORBA_exception_free (&ev);
      
      g_free (e.type);
    }

  va_end (args);

}

static gboolean
spi_atk_bridge_property_event_listener (GSignalInvocationHint *signal_hint,
					guint n_param_values,
					const GValue *param_values,
					gpointer data)
{
  AtkPropertyValues *values;
  GObject *gobject;

#ifdef SPI_BRIDGE_DEBUG
  GSignalQuery signal_query;
  const gchar *name;
  const gchar *s, *s2;
  
  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  s2 = g_type_name (G_OBJECT_TYPE (g_value_get_object (param_values + 0)));
  s = atk_object_get_name (ATK_OBJECT (g_value_get_object (param_values + 0)));
  values = (AtkPropertyValues*) g_value_get_pointer (param_values + 1);
  DBG (2, g_message ("Received (property) signal %s:%s:%s from object %s (gail %s)\n",
	   g_type_name (signal_query.itype), name, values->property_name, s, s2));
  
#endif

  gobject = g_value_get_object (param_values + 0);
  values = (AtkPropertyValues*) g_value_get_pointer (param_values + 1);

  spi_atk_emit_eventv (gobject, 0, 0, NULL,
		       "object:property-change:%s", values->property_name);

  return TRUE;
}

static gboolean
spi_atk_bridge_state_event_listener (GSignalInvocationHint *signal_hint,
				     guint n_param_values,
				     const GValue *param_values,
				     gpointer data)
{
  GObject *gobject;
  gchar *property_name;
  gchar *type;
  unsigned long detail1;
#ifdef SPI_BRIDGE_DEBUG
  GSignalQuery signal_query;
  const gchar *name;
  
  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;
  fprintf (stderr, "Received (state) signal %s:%s\n",
	   g_type_name (signal_query.itype), name);
#endif

  gobject = g_value_get_object (param_values + 0);
  property_name = g_strdup (g_value_get_string (param_values + 1));
  detail1 = (g_value_get_boolean (param_values + 2))
    ? 1 : 0;
  type = g_strdup_printf ("object:state-changed:%s", property_name);
  spi_atk_emit_eventv (gobject, 
		       detail1,
		       0,
		       NULL,
		       type);
  g_free (property_name);
  g_free (type);
  return TRUE;
}


static void
spi_init_keystroke_from_atk_key_event (Accessibility_DeviceEvent  *keystroke,
				       AtkKeyEventStruct          *event)
{
#ifdef SPI_DEBUG
  if (event)
    {
      g_print ("event %c (%d)\n", (int) event->keyval, (int) event->keycode);
    }
  else
#endif
  if (!event)
    {
      g_print ("WARNING: NULL key event!");
    }
  
  keystroke->id        = (CORBA_long) event->keyval;
  keystroke->hw_code   = (CORBA_short) event->keycode;
  keystroke->timestamp = (CORBA_unsigned_long) event->timestamp;
  keystroke->modifiers = (CORBA_unsigned_short) (event->state & 0xFFFF);
  if (event->string)
    {
      keystroke->event_string = CORBA_string_dup (event->string);
      keystroke->is_text = CORBA_TRUE;
    }
  else
    {
      keystroke->event_string = CORBA_string_dup ("");
      keystroke->is_text = CORBA_FALSE;
    }
  switch (event->type)
    {
    case (ATK_KEY_EVENT_PRESS):
      keystroke->type = Accessibility_KEY_PRESSED_EVENT;
      break;
    case (ATK_KEY_EVENT_RELEASE):
      keystroke->type = Accessibility_KEY_RELEASED_EVENT;
      break;
    default:
      keystroke->type = 0;
      break;
    }
#if 0  
  g_print ("key_event type %d; val=%d code=%d modifiers=%x name=%s is_text=%d, time=%lx\n",
	   (int) keystroke->type, (int) keystroke->id, (int) keystroke->hw_code,
	   (int) keystroke->modifiers,
	   keystroke->event_string, (int) keystroke->is_text, (unsigned long) keystroke->timestamp);
#endif
}

static gint
spi_atk_bridge_key_listener (AtkKeyEventStruct *event, gpointer data)
{
  CORBA_boolean             result;
  Accessibility_DeviceEvent key_event;
  Accessibility_DeviceEventController controller;
	
  if (BONOBO_EX (&ev))
	g_warning ("failure: pre-listener get dec\n");

  controller =
    Accessibility_Registry_getDeviceEventController (
	    spi_atk_bridge_get_registry (), &ev);

  if (BONOBO_EX (&ev))
    {
      g_warning ("failure: no deviceeventcontroller found\n");
      CORBA_exception_free (&ev);
      registry_died = TRUE;
      result = FALSE;
    }
  else
    {

      spi_init_keystroke_from_atk_key_event (&key_event, event);

      result = Accessibility_DeviceEventController_notifyListenersSync (
        controller, &key_event, &ev);

      bonobo_object_release_unref (controller, &ev);
      CORBA_exception_free (&ev);
    }

  return result;
}


static void
spi_atk_signal_emit_event (const GObject *gobject, 
			   const GSignalQuery *signal_query,
			   long detail1, 
			   long detail2,
			   const gchar *name, 
			   const gchar *detail)
{
  CORBA_any any;
  CORBA_char *sp = NULL;
  AtkObject *ao;

  if (signal_query->signal_id == atk_signal_text_changed)
    {
      sp = atk_text_get_text (ATK_TEXT (gobject),
			      detail1,
			      detail1+detail2);
      spi_init_any_string (&any, &sp);
    }
#ifdef EXTENDED_OBJECT_EVENTS_ARE_WORKING
  else if ((signal_query->signal_id == atk_signal_child_changed) && gobject)
    {
      ao = atk_object_ref_accessible_child (ATK_OBJECT (gobject), 
					    detail1);
      if (ao) 
	{
	  spi_init_any_object (&any, ao); 
	  atk_object_unref (ao);
	}
      else
	{
	  spi_init_any_nil (&any);
	}
    }
#endif
  else
    {
      spi_init_any_nil (&any);
    }

  if (detail)
    spi_atk_emit_eventv (gobject, detail1, detail2, &any,
			 "object:%s:%s", name, detail);
  else
    spi_atk_emit_eventv (gobject, detail1, detail2, &any,
			 "object:%s", name);
}



static gboolean
spi_atk_bridge_signal_listener (GSignalInvocationHint *signal_hint,
				guint n_param_values,
				const GValue *param_values,
				gpointer data)
{
  GObject *gobject;
  GSignalQuery signal_query;
  const gchar *name;
  const gchar *detail;
  CORBA_char *sp;
  
  gint detail1 = 0, detail2 = 0;
#ifdef SPI_BRIDGE_DEBUG
  const gchar *s, *s2;
#endif 
  
  g_signal_query (signal_hint->signal_id, &signal_query);

  name = signal_query.signal_name;
  if (signal_hint->detail)
    detail = g_quark_to_string (signal_hint->detail);
  else
    detail = NULL;

#ifdef SPI_BRIDGE_DEBUG
  s2 = g_type_name (G_OBJECT_TYPE (g_value_get_object (param_values + 0)));
  s = atk_object_get_name (ATK_OBJECT (g_value_get_object (param_values + 0)));
  fprintf (stderr, "Received signal %s:%s detail: %s from object %s (gail %s)\n",
	   g_type_name (signal_query.itype), name, 
	   detail ? detail : "<NULL>", s ? s : "<NULL>" , s2);
#endif
  
  gobject = g_value_get_object (param_values + 0);
  if (G_VALUE_TYPE (param_values + 1) == G_TYPE_INT)
    detail1 = g_value_get_int (param_values + 1);
  if (G_VALUE_TYPE (param_values + 2) == G_TYPE_INT)
    detail2 = g_value_get_int (param_values + 2);
  
  spi_atk_signal_emit_event (gobject, &signal_query,
			     detail2, detail2,
			     name, detail);
  return TRUE;
}

static gboolean
spi_atk_bridge_window_event_listener (GSignalInvocationHint *signal_hint,
				      guint n_param_values,
				      const GValue *param_values,
				      gpointer data)
{
  GObject *gobject;
  GSignalQuery signal_query;
  CORBA_any any;
  const gchar *name, *s;
#ifdef SPI_BRIDGE_DEBUG
  const gchar *s2;
#endif
  
  g_signal_query (signal_hint->signal_id, &signal_query);

  name = signal_query.signal_name;

#ifdef SPI_BRIDGE_DEBUG
  s2 = g_type_name (G_OBJECT_TYPE (g_value_get_object (param_values + 0)));
  s = atk_object_get_name (ATK_OBJECT (g_value_get_object (param_values + 0)));
  fprintf (stderr, "Received signal %s:%s from object %s (gail %s)\n",
	   g_type_name (signal_query.itype), name, s ? s : "<NULL>" , s2);
#endif
  
  gobject = g_value_get_object (param_values + 0);

  s = atk_object_get_name (ATK_OBJECT (gobject));
  spi_init_any_string (&any, &s);
  
  spi_atk_emit_eventv (gobject, 0, 0, &any,
		       "window:%s", name);
  return TRUE;
}
