/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001 Sun Microsystems Inc.
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

#include <stdlib.h>
#include "../cspi/spi-private.h" /* A hack for now */

static void traverse_accessible_tree (Accessible *accessible);

static void report_event  (const AccessibleEvent *event, void *user_data);
static void report_detail_event  (const AccessibleEvent *event, void *user_data);

static AccessibleEventListener *generic_listener;
static AccessibleEventListener *specific_listener;
static gint n_elements_traversed = 0;

int
main (int argc, char **argv)
{
  int i, j;
  int n_desktops;
  int n_apps;
  char *s;
  GTimer *timer;
  gdouble elapsed_time;
  Accessible *desktop;
  Accessible *application;
  const char *modules;

  SPI_init ();

  generic_listener = SPI_createAccessibleEventListener (
	  report_event, NULL); 
  specific_listener = SPI_createAccessibleEventListener (
	  report_detail_event, NULL); 

  SPI_registerGlobalEventListener (generic_listener,
				   "focus:");
  SPI_registerGlobalEventListener (generic_listener,
				   "object:property-change");
  SPI_registerGlobalEventListener (specific_listener,
				   "object:property-change:accessible-name");
  SPI_registerGlobalEventListener (generic_listener,
				   "object:state-changed"); 
  SPI_registerGlobalEventListener (generic_listener,
				   "object:selection-changed"); 
  SPI_registerGlobalEventListener (generic_listener,
				   "object:children-changed"); 
  SPI_registerGlobalEventListener (specific_listener,
				   "object:children-changed:add"); 
  SPI_registerGlobalEventListener (generic_listener,
				   "object:visible-data-changed"); 
  SPI_registerGlobalEventListener (generic_listener,
				   "object:text-selection-changed"); 
  SPI_registerGlobalEventListener (generic_listener,
				   "object:text-caret-moved"); 
  SPI_registerGlobalEventListener (generic_listener,
				   "object:text-changed"); 
  SPI_registerGlobalEventListener (generic_listener,
				   "object:column-inserted"); 
  SPI_registerGlobalEventListener (generic_listener,
				   "object:row-inserted"); 
  SPI_registerGlobalEventListener (generic_listener,
				   "object:column-reordered"); 
  SPI_registerGlobalEventListener (generic_listener,
				   "object:row-reordered"); 
  SPI_registerGlobalEventListener (generic_listener,
				   "object:column-deleted"); 
  SPI_registerGlobalEventListener (generic_listener,
				   "object:row-deleted"); 
  SPI_registerGlobalEventListener (generic_listener,
				   "object:model-changed"); 
  SPI_registerGlobalEventListener (generic_listener,
				   "window:minimize");
  SPI_registerGlobalEventListener (generic_listener,
				   "window:maximize");
  SPI_registerGlobalEventListener (generic_listener,
	                           "window:restore");
  SPI_registerGlobalEventListener (generic_listener,
	                           "window:activate");
  SPI_registerGlobalEventListener (generic_listener,
	                           "window:deactivate");
  SPI_registerGlobalEventListener (generic_listener,
	                           "window:close");
#ifdef NOT_YET_IMPLEMENTED
  SPI_registerGlobalEventListener (generic_listener,
	                           "window:lower");
  SPI_registerGlobalEventListener (generic_listener,
	                           "window:raise");
  SPI_registerGlobalEventListener (generic_listener,
	                           "window:resize");
  SPI_registerGlobalEventListener (generic_listener,
	                           "window:shade");
  SPI_registerGlobalEventListener (generic_listener,
	                           "window:unshade");
  /* event below possibly should just be property change? */
  SPI_registerGlobalEventListener (generic_listener,
	                           "window:restyle"); 
  SPI_registerGlobalEventListener (generic_listener,
	                           "window:desktop-create");
  SPI_registerGlobalEventListener (generic_listener,
	                           "window:desktop-destroy");
#endif
  
  timer = g_timer_new ();
  traverse_accessible_tree (SPI_getDesktop (0));
  g_print ("Time for first traversal of %d elements: %lf\n", 
           n_elements_traversed,
	   g_timer_elapsed (timer, NULL));
  g_timer_start (timer);
  traverse_accessible_tree (SPI_getDesktop (0));
  g_timer_stop (timer);
  g_print ("Time for subsequent traversal %f\n", g_timer_elapsed (timer, NULL));
  g_print ("[%f elements/sec, %f SPI calls/sec]\n", 
	n_elements_traversed/g_timer_elapsed(timer, NULL),
	(n_elements_traversed*8+1)/g_timer_elapsed(timer, NULL));
  
  SPI_event_main ();

  putenv ("AT_BRIDGE_SHUTDOWN=1");

  /*
   * TODO: Add a key event listener that calls test_exit, to
   * deregister and cleanup appropriately.
   */

  return SPI_exit ();
}

static void
traverse_accessible_tree (Accessible *accessible)
{
	int n_children;
	int i;
	char *name;
	char *role_name;
	Accessible *child;
	
        n_elements_traversed++;
	name = Accessible_getName (accessible);
	role_name = Accessible_getRoleName (accessible);
#ifdef VERBOSE
	fprintf (stdout, "[%s] \"%s\"\n",
		 role_name, name);
#endif
	SPI_freeString (name);
	SPI_freeString (role_name);
	n_children = Accessible_getChildCount (accessible);
	if (!Accessible_isTable (accessible)) 
	{
		for (i = 0; i < n_children; ++i)
		{
			child = Accessible_getChildAtIndex (accessible, i);
			traverse_accessible_tree (child);
			Accessible_unref (child);
	        }
	}
}

void
report_event (const AccessibleEvent *event, void *user_data)
{
  fprintf (stderr, "%s\n", event->type);
}

void
report_detail_event (const AccessibleEvent *event, void *user_data)
{
  fprintf (stderr, "%s\n", event->type);
}

void
test_exit ()
{
  SPI_deregisterGlobalEventListenerAll (generic_listener);
  AccessibleEventListener_unref (generic_listener);
  SPI_deregisterGlobalEventListenerAll (specific_listener);
  AccessibleEventListener_unref (specific_listener);
}
