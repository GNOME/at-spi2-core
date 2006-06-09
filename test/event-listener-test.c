/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002, 2003 Sun Microsystems Inc.,
 * Copyright 2001, 2002, 2003 Ximian, Inc.
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
#include <unistd.h>
#include "../cspi/spi-private.h" /* A hack for now */

static void traverse_accessible_tree (Accessible *accessible);

static void report_event  (const AccessibleEvent *event, void *user_data);
static void report_caret_event  (const AccessibleEvent *event, void *user_data);
static void report_bounds_event  (const AccessibleEvent *event, void *user_data);
static void report_detail_event  (const AccessibleEvent *event, void *user_data);
static void report_detail1_event  (const AccessibleEvent *event, void *user_data);
static void report_text_event  (const AccessibleEvent *event, void *user_data);
static void report_text_selection_event  (const AccessibleEvent *event, void *user_data);
static void report_active_descendant_changed_event  (const AccessibleEvent *event, void *user_data);
static void report_children_changed_event (const AccessibleEvent *event, void *user_data);
static void report_name_changed_event (const AccessibleEvent *event, void *user_data);
static void report_description_changed_event (const AccessibleEvent *event, void *user_data);
static void report_parent_changed_event (const AccessibleEvent *event, void *user_data);
static void report_window_event  (const AccessibleEvent *event, void *user_data);
static void report_table_summary_event  (const AccessibleEvent *event, void *user_data);
static void report_table_header_event  (const AccessibleEvent *event, void *user_data);
static void report_table_caption_event  (const AccessibleEvent *event, void *user_data);
static void report_table_row_description_event  (const AccessibleEvent *event, void *user_data);
static void report_table_column_description_event  (const AccessibleEvent *event, void *user_data);
static void timing_test_event (const AccessibleEvent *event, void *user_data);
static SPIBoolean report_mouse_event  (const AccessibleDeviceEvent *event, void *user_data);

static AccessibleEventListener *generic_listener;
static AccessibleEventListener *specific_listener;
static AccessibleEventListener *bounds_listener;
static AccessibleEventListener *detail1_listener;
static AccessibleEventListener *test_listener;
static AccessibleEventListener *text_listener;
static AccessibleEventListener *caret_listener;
static AccessibleEventListener *text_selection_listener;
static AccessibleEventListener *active_descendant_changed_listener;
static AccessibleEventListener *children_changed_listener;
static AccessibleEventListener *name_changed_listener;
static AccessibleEventListener *description_changed_listener;
static AccessibleEventListener *parent_changed_listener;
static AccessibleEventListener *window_listener;
static AccessibleEventListener *table_summary_listener;
static AccessibleEventListener *table_header_listener;
static AccessibleEventListener *table_caption_listener;
static AccessibleEventListener *table_row_description_listener;
static AccessibleEventListener *table_column_description_listener;
static AccessibleDeviceListener *mouse_device_listener;
static gint n_elements_traversed = 0;
static GTimer *timer;

static gboolean report_mouse_events = TRUE;

void 
usage_and_exit( void )
{
  g_print("\nUsage: event-listener-test [-h] [-m]\n");
  g_print("       -h    : prints this usage message.\n");
  g_print("       -m    : disable mouse event reporting.\n\n");

  exit( 1 );
}

int
main (int argc, char **argv)
{
  int c;

  /* Parse Command-line */
  if ( argc > 1 ) {
      while ( ( c = getopt( argc, argv, "hm")) != EOF ) {
          switch( c ) {
              case 'm':
                  report_mouse_events = FALSE;
                  break;
              default:
                  usage_and_exit();
                  break;
          }
      }
      if ( optind < argc ) {
          usage_and_exit();
      }
  }

  fprintf (stderr, "RUNNING\n");

  SPI_init ();

  generic_listener = SPI_createAccessibleEventListener (
	  report_event, NULL); 
  specific_listener = SPI_createAccessibleEventListener (
	  report_detail_event, NULL); 
  bounds_listener = SPI_createAccessibleEventListener (
	  report_bounds_event, NULL);
  text_listener = SPI_createAccessibleEventListener (
	  report_text_event, NULL);
  caret_listener = SPI_createAccessibleEventListener (
	  report_caret_event, NULL);
  text_selection_listener = SPI_createAccessibleEventListener (
	  report_text_selection_event, NULL);
  active_descendant_changed_listener = SPI_createAccessibleEventListener (
	  report_active_descendant_changed_event, NULL);
  children_changed_listener = SPI_createAccessibleEventListener (
	  report_children_changed_event, NULL);
  name_changed_listener = SPI_createAccessibleEventListener (
	  report_name_changed_event, NULL);
  description_changed_listener = SPI_createAccessibleEventListener (
	  report_description_changed_event, NULL);
  parent_changed_listener = SPI_createAccessibleEventListener (
	  report_parent_changed_event, NULL);
  window_listener = SPI_createAccessibleEventListener (
	  report_window_event, NULL);
  table_summary_listener = SPI_createAccessibleEventListener (
	  report_table_summary_event, NULL);
  table_header_listener = SPI_createAccessibleEventListener (
	  report_table_header_event, NULL);
  table_caption_listener = SPI_createAccessibleEventListener (
	  report_table_caption_event, NULL);
  table_row_description_listener = SPI_createAccessibleEventListener (
	  report_table_row_description_event, NULL);
  table_column_description_listener = SPI_createAccessibleEventListener (
	  report_table_column_description_event, NULL);
  test_listener = SPI_createAccessibleEventListener (
	  timing_test_event, NULL);
  mouse_device_listener = SPI_createAccessibleDeviceListener (
          report_mouse_event, NULL);
  detail1_listener = SPI_createAccessibleEventListener (
	  report_detail1_event, NULL); 

  SPI_registerGlobalEventListener (generic_listener,
                                   "focus:");
  if ( report_mouse_events ) {
      SPI_registerGlobalEventListener (specific_listener,
				       "mouse:rel");
      SPI_registerGlobalEventListener (specific_listener,
				       "mouse:button");
      SPI_registerGlobalEventListener (specific_listener,
				       "mouse:abs");
  }
  SPI_registerDeviceEventListener (mouse_device_listener, 
				   SPI_BUTTON_PRESSED | SPI_BUTTON_RELEASED,
				   NULL);
  SPI_registerGlobalEventListener (specific_listener,
				   "keyboard:modifiers");

  SPI_registerGlobalEventListener (generic_listener,
				   "object:property-change");
  SPI_registerGlobalEventListener (name_changed_listener,
				   "object:property-change:accessible-name");
  SPI_registerGlobalEventListener (description_changed_listener,
				   "object:property-change:accessible-description");
  SPI_registerGlobalEventListener (parent_changed_listener,
				   "object:property-change:accessible-parent");
  SPI_registerGlobalEventListener (generic_listener,
				   "object:state-changed"); 
/*  SPI_registerGlobalEventListener (specific_listener,
    "object:state-changed:focused"); */
  SPI_registerGlobalEventListener (generic_listener,
				   "object:selection-changed"); 

  SPI_registerGlobalEventListener (children_changed_listener,
				   "object:children-changed"); 
  SPI_registerGlobalEventListener (active_descendant_changed_listener,
				   "object:active-descendant-changed"); 
  SPI_registerGlobalEventListener (generic_listener,
				   "object:visible-data-changed"); 
  SPI_registerGlobalEventListener (text_selection_listener,
				   "object:text-selection-changed"); 
  SPI_registerGlobalEventListener (caret_listener,
				   "object:text-caret-moved"); 
  SPI_registerGlobalEventListener (text_listener,
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
  SPI_registerGlobalEventListener (detail1_listener,
				   "object:link-selected"); 
#if 0
  SPI_registerGlobalEventListener (bounds_listener,
				   "object:bounds-changed");
#endif
  SPI_registerGlobalEventListener (window_listener,
				   "window:minimize");
  SPI_registerGlobalEventListener (window_listener,
				   "window:maximize");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:restore");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:activate");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:create");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:deactivate");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:close");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:lower");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:raise");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:resize");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:shade");
  SPI_registerGlobalEventListener (window_listener,
	                           "window:unshade");
  SPI_registerGlobalEventListener (table_summary_listener,
				   "object:property-change:accessible-table-summary");
  SPI_registerGlobalEventListener (table_header_listener,
				   "object:property-change:accessible-table-row-header");
  SPI_registerGlobalEventListener (table_header_listener,
				   "object:property-change:accessible-table-column-header");
  SPI_registerGlobalEventListener (table_summary_listener,
				   "object:property-change:accessible-table-summary");
  SPI_registerGlobalEventListener (table_row_description_listener,
				   "object:property-change:accessible-table-row-description");
  SPI_registerGlobalEventListener (table_column_description_listener,
				   "object:property-change:accessible-table-column-description");
  SPI_registerGlobalEventListener (test_listener,
				   "object:test");
#ifdef NOT_YET_IMPLEMENTED
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
  g_timer_reset (timer);
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
  static long count = 0;
  char *s = Accessible_getName (event->source);
  fprintf (stderr, "%s %s\n", event->type, s);
  if (s) SPI_freeString (s);
  if (count == 0) {
	  g_timer_reset (timer);
	  g_timer_start (timer);
  }
  ++count;
  if ((count % 100) == 0) {
	  g_print ("%ld events received, %f events/sec\n",
		   count,
		   count/g_timer_elapsed(timer, NULL));
  }

  /* check for Table interface - if present, query selections */
  if (Accessible_isTable (event->source)) 
  {
      gint n_selected_rows = 0;
      gint n_selected_cols = 0;
      long int *rows, *cols;
      AccessibleTable *table = Accessible_getTable (event->source);

      n_selected_rows = AccessibleTable_getSelectedRows (table, &rows);
      n_selected_cols  = AccessibleTable_getSelectedColumns (table, &cols);

      g_print ("TABLE interface: selectedRows=%d, selectedCols = %d\n",
	       n_selected_rows, n_selected_cols);
  }
}

void
report_caret_event (const AccessibleEvent *event, void *user_data)
{
  char *s = Accessible_getName (event->source);
  long int start, end, offset, line_start_begin;
  long int line_start_end, line_end_begin, line_end_end;

  AccessibleText *text = Accessible_getText (event->source);
  offset = event->detail1;
  AccessibleText_getTextAtOffset (text, offset, 
				  SPI_TEXT_BOUNDARY_SENTENCE_START,
				  &start, &end);
  fprintf (stderr, 
	   "caret event from %s %s: offset %ld, SENTENCE_START offsets start=%ld, end=%ld\n", 
	   event->type, s, offset, start, end);
  AccessibleText_getTextAtOffset (text, offset, 
				  SPI_TEXT_BOUNDARY_SENTENCE_END,
				  &start, &end);
  AccessibleText_getTextAtOffset (text, offset, 
				  SPI_TEXT_BOUNDARY_LINE_START,
				  &line_start_begin, &line_start_end);
  AccessibleText_getTextAtOffset (text, offset, 
				  SPI_TEXT_BOUNDARY_LINE_END,
				  &line_end_begin, &line_end_end);
  fprintf (stderr, 
	   "SENTENCE_END: %ld - %ld; LINE_START: %ld - %ld; LINE_END: %ld - %ld\n", 
	   start, end, line_start_begin, line_start_end,
	   line_end_begin, line_end_end);

  AccessibleText_getTextAtOffset (text, offset, 
				  SPI_TEXT_BOUNDARY_WORD_START,
				  &line_start_begin, &line_start_end);
  AccessibleText_getTextAtOffset (text, offset, 
				  SPI_TEXT_BOUNDARY_WORD_END,
				  &line_end_begin, &line_end_end);
  fprintf (stderr, 
	   "WORD_START: %ld - %ld; WORD_END: %ld - %ld\n", 
	   line_start_begin, line_start_end,
	   line_end_begin, line_end_end);

  if (s) SPI_freeString (s);
}

void
report_detail_event (const AccessibleEvent *event, void *user_data)
{
  char *s = Accessible_getName (event->source);
  fprintf (stderr, "(detail) %s %s %ld %ld\n", event->type, s,
	   event->detail1, event->detail2);
  if (s) SPI_freeString (s);
}

void
report_detail1_event (const AccessibleEvent *event, void *user_data)
{
  char *s = Accessible_getName (event->source);
  fprintf (stderr, "(detail) %s %s %ld\n", event->type, s,
	   event->detail1);
  if (s) SPI_freeString (s);
}

void
report_bounds_event (const AccessibleEvent *event, void *user_data)
{
  char *s = Accessible_getName (event->source);
  SPIRect *bounds = AccessibleBoundsChangedEvent_getNewBounds (event);
  if (!bounds) fprintf (stderr, "bounds-changed event with no bounds?\n");
  fprintf (stderr, "(bounds-changed) %s %s %ld,%ld - %ld,%ld\n", event->type, s,
	   bounds->x, bounds->y, bounds->x + bounds->width, bounds->y + bounds->height);
  SPI_freeRect (bounds);
  if (s) SPI_freeString (s);
}

void
report_text_event (const AccessibleEvent *event, void *user_data)
{
  char *s = Accessible_getName (event->source);
  fprintf (stderr, "(detail) %s %s %ld %ld\n", event->type, s,
	   event->detail1, event->detail2);
  SPI_freeString (s);
  s = AccessibleTextChangedEvent_getChangeString (event);
  fprintf (stderr, "context string %s\n", (s) ? s : "<nil>");
  if (s) SPI_freeString (s);
}

void
report_text_selection_event (const AccessibleEvent *event, void *user_data)
{
  char *s = Accessible_getName (event->source);
  fprintf (stderr, "(detail) %s %s %ld %ld\n", event->type, s,
	   event->detail1, event->detail2);
  SPI_freeString (s);
  s = AccessibleTextSelectionChangedEvent_getSelectionString (event);
  fprintf (stderr, "context string %s\n", (s) ? s : "<nil>");
  SPI_freeString (s);
}

void
report_active_descendant_changed_event (const AccessibleEvent *event, void *user_data)
{
  char *s = Accessible_getName (event->source);
  char *s1;
  Accessible *ao;

  ao = AccessibleActiveDescendantChangedEvent_getActiveDescendant (event);
  s1 = Accessible_getName (ao);
  fprintf (stderr, "(detail) %s parent: %s child: %s %ld %ld\n", event->type, 
           s ? s : "<null>", s1 ? s1 : "<null>",
	   event->detail1, event->detail2);
  SPI_freeString (s);
  SPI_freeString (s1);
  Accessible_unref (ao);
}
void
report_children_changed_event (const AccessibleEvent *event, void *user_data)
{
  char *s = Accessible_getName (event->source);
  char *s1 = NULL, *s2, *s3 = NULL;
  Accessible *ao;

  ao = AccessibleChildChangedEvent_getChildAccessible (event);
  if (ao) s1 = Accessible_getName (ao);
  s2 = Accessible_getRoleName (event->source);
  if (ao) s3 = Accessible_getRoleName (ao);
  fprintf (stderr, "(detail) %s parent: %s [%s] child: %s [%s] %ld %ld\n", 
           event->type, 
           s ? s : "<null>", 
	   s2,
           s1 ? s1 : "<null>",
	   s3 ? s3 : "<null>",
	   event->detail1, event->detail2);
  SPI_freeString (s);
  SPI_freeString (s1);
  SPI_freeString (s2);
  SPI_freeString (s3);
  Accessible_unref (ao);
}

void
report_name_changed_event (const AccessibleEvent *event, void *user_data)
{
  char *s = Accessible_getName (event->source);
  fprintf (stderr, "(detail) %s %s %ld %ld\n", event->type, s,
	   event->detail1, event->detail2);
  SPI_freeString (s);
  s = AccessibleNameChangedEvent_getNameString (event);
  fprintf (stderr, "context string %s\n", (s) ? s : "<nil>");
  SPI_freeString (s);
}

void
report_description_changed_event (const AccessibleEvent *event, void *user_data)
{
  char *s = Accessible_getName (event->source);
  fprintf (stderr, "(detail) %s %s %ld %ld\n", event->type, s,
	   event->detail1, event->detail2);
  SPI_freeString (s);
  s = AccessibleDescriptionChangedEvent_getDescriptionString (event);
  fprintf (stderr, "context string %s\n", (s) ? s : "<nil>");
  SPI_freeString (s);
}

void
report_parent_changed_event (const AccessibleEvent *event, void *user_data)
{
  char *s = Accessible_getName (event->source);
  char *s1;
  Accessible *ao;

  ao = AccessibleParentChangedEvent_getParentAccessible (event);
  s1 = Accessible_getName (ao);
  fprintf (stderr, "(detail) %s parent: %s child: %s %ld %ld\n", event->type, 
           s ? s : "<null>", s1 ? s1 : "<null>",
	   event->detail1, event->detail2);
  SPI_freeString (s);
  SPI_freeString (s1);
  Accessible_unref (ao);
}

void
report_window_event (const AccessibleEvent *event, void *user_data)
{
  char *s = Accessible_getName (event->source);
  fprintf (stderr, "(detail) %s %s %ld %ld\n", event->type, s,
	   event->detail1, event->detail2);
  SPI_freeString (s);
  s = AccessibleWindowEvent_getTitleString (event);
  fprintf (stderr, "context string %s\n", (s) ? s : "<nil>");
  SPI_freeString (s);
}

void
report_table_summary_event (const AccessibleEvent *event, void *user_data)
{
  char *s = Accessible_getName (event->source);
  char *s1;
  Accessible *ao;

  ao = AccessibleTableSummaryChangedEvent_getSummaryAccessible (event);
  s1 = Accessible_getName (ao);
  fprintf (stderr, "(detail) %s parent: %s child: %s %ld %ld\n", event->type, 
           s ? s : "<null>", s1 ? s1 : "<null>",
	   event->detail1, event->detail2);
  SPI_freeString (s);
  SPI_freeString (s1);
  Accessible_unref (ao);
}

void
report_table_header_event (const AccessibleEvent *event, void *user_data)
{
  char *s = Accessible_getName (event->source);
  char *s1;
  Accessible *ao;

  ao = AccessibleTableHeaderChangedEvent_getHeaderAccessible (event);
  s1 = Accessible_getName (ao);
  fprintf (stderr, "(detail) %s parent: %s child: %s %ld %ld\n", event->type, 
           s ? s : "<null>", s1 ? s1 : "<null>",
	   event->detail1, event->detail2);
  SPI_freeString (s);
  SPI_freeString (s1);
  Accessible_unref (ao);
}

void
report_table_caption_event (const AccessibleEvent *event, void *user_data)
{
  char *s = Accessible_getName (event->source);
  fprintf (stderr, "(detail) %s %s %ld %ld\n", event->type, s,
	   event->detail1, event->detail2);
  SPI_freeString (s);
  s = AccessibleTableCaptionChangedEvent_getCaptionString (event);
  fprintf (stderr, "context string %s\n", (s) ? s : "<nil>");
  SPI_freeString (s);
}

void
report_table_row_description_event (const AccessibleEvent *event, void *user_data)
{
  char *s = Accessible_getName (event->source);
  fprintf (stderr, "(detail) %s %s %ld %ld\n", event->type, s,
	   event->detail1, event->detail2);
  SPI_freeString (s);
  s = AccessibleTableRowDescriptionChangedEvent_getDescriptionString (event);
  fprintf (stderr, "context string %s\n", (s) ? s : "<nil>");
  SPI_freeString (s);
}

void
report_table_column_description_event (const AccessibleEvent *event, void *user_data)
{
  char *s = Accessible_getName (event->source);
  fprintf (stderr, "(detail) %s %s %ld %ld\n", event->type, s,
	   event->detail1, event->detail2);
  SPI_freeString (s);
  s = AccessibleTableColumnDescriptionChangedEvent_getDescriptionString (event);
  fprintf (stderr, "context string %s\n", (s) ? s : "<nil>");
  SPI_freeString (s);
}

SPIBoolean
report_mouse_event (const AccessibleDeviceEvent *event, void *user_data)
{
  fprintf (stderr, "mouse event %ld %d %x %x\n", 
	   event->keyID, 
	   (int) event->keycode,
	   (unsigned) event->type,
	   (unsigned) event->modifiers);
  return FALSE;
}

void
timing_test_event (const AccessibleEvent *event, void *user_data)
{
	static long count = 0;
	if (count == 0) g_timer_start (timer);
	++count;
	if ((count % 500) == 0) {
		g_print ("%ld events received, %f events/sec\n",
			 count,
			 count/g_timer_elapsed(timer, NULL));
	}
}

void
test_exit (void)
{
  SPI_deregisterGlobalEventListenerAll (generic_listener);
  AccessibleEventListener_unref (generic_listener);
  SPI_deregisterGlobalEventListenerAll (specific_listener);
  AccessibleEventListener_unref (specific_listener);
}
