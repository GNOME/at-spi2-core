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
#include "../cspi/spi-private.h"

#define CLIP_DEBUG

#define BOUNDS_CONTAIN_X(b, p)  (((p)>=(b)->x) && ((p)<=((b)->x + (b)->width))\
	    && ((b)->width > 0) && ((b)->height > 0))

#define BOUNDS_CONTAIN_Y(b, p)  (((p)>=(b)->y) && ((p)<=((b)->y + (b)->height))\
	    && ((b)->width > 0) && ((b)->height > 0))

#define CHUNK_BOUNDS_BEFORE_START(c, i) ((i) && ((c)->clip_bounds.x < \
                                         (i)->clip_bounds.x)) 

#define CHUNK_BOUNDS_END_BEFORE_START(c, i) ((i) && \
                                         (((c)->clip_bounds.x + \
                                         (c)->clip_bounds.width) < \
                                         (i)->clip_bounds.x))

#define CHUNK_BOUNDS_AFTER_END(c, i) ((i) && ((c)->clip_bounds.x >= \
			               ((i)->clip_bounds.x + \
			                (i)->clip_bounds.width)))

#define CHUNK_BOUNDS_SPANS_END(c, i) ((i) && (((c)->clip_bounds.x + \
					      (c)->clip_bounds.width) >= \
			               ((i)->clip_bounds.x + \
			                (i)->clip_bounds.width)))

//#define CHUNK_BOUNDS_WITHIN(c, i)  ((i) && ((c)->clip_bounds.x >= \
//                               (i)->text_bounds.x) && \
//                               (((c)->clip_bounds.x + (c)->clip_bounds.width) \
//                               <= ((i)->text_bounds.x + (i)->text_bounds.width)))

static void report_screen_review_line  (const AccessibleEvent *event, void *user_data);

static gint n_elements_traversed = 0;
static AccessibleEventListener *mouseclick_listener;

typedef struct _BoundaryRect {
	long int x;
	long int y;
	long int width;
	long int height;
	AccessibleRole role; /* role of last clipping element */
	gboolean isClipped;
	gboolean isEmpty;
} BoundaryRect;

typedef struct _TextChunk {
	char           *string;
	AccessibleText *source;
	int             start_offset;
	int             end_offset;
	BoundaryRect    clip_bounds;
	BoundaryRect    text_bounds;
	BoundaryRect    start_char_bounds;
	BoundaryRect    end_char_bounds;
} TextChunk;

typedef struct _ScreenReviewBuffer { /* TODO: implement */
	GList *text_chunks;
} ScreenReviewBuffer;

static gboolean isJava = FALSE;

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

  mouseclick_listener = SPI_createAccessibleEventListener (
	  report_screen_review_line, NULL); 

  SPI_registerGlobalEventListener (mouseclick_listener,
				   "Gtk:GtkWidget:button-press-event");
#define JAVA_TEST_HACK
#ifdef JAVA_TEST_HACK
  SPI_registerGlobalEventListener (mouseclick_listener,
				   "object:text-caret-moved");
  isJava = TRUE;
#endif
  SPI_event_main ();

  putenv ("AT_BRIDGE_SHUTDOWN=1");

  /*
   * TODO: Add a key event listener that calls test_exit, to
   * deregister and cleanup appropriately.
   */

  return SPI_exit ();
}

static inline gboolean
bounds_contain_y (BoundaryRect *bounds, int y)
{
	return (y > bounds->y && y <= (bounds->y + bounds->height)
		&& bounds->width && bounds->height);
}


static inline gboolean
chunk_bounds_within (TextChunk *chunk, TextChunk *test_chunk)
{
	int x1, x2, tx1, tx2;
	gboolean gtx1, ltx2;

	x1 = chunk->clip_bounds.x;
	x2 = x1 + chunk->clip_bounds.width;
	tx1 = test_chunk->clip_bounds.x;
	tx2 = tx1 + test_chunk->clip_bounds.width;
	gtx1 = (chunk->clip_bounds.x >= test_chunk->clip_bounds.x);
	ltx2 = (chunk->clip_bounds.x + chunk->clip_bounds.width
		<= test_chunk->clip_bounds.x + test_chunk->clip_bounds.width);

//	fprintf (stderr, "testing BOUNDS %d-%d WITHIN %d-%d: %s\n",
//		 x1, x2, tx1, tx2, ((gtx1 && ltx2) ? "T" : "F"));
	
	return gtx1 && ltx2;
}

#define CHUNK_BOUNDS_WITHIN(a, b) chunk_bounds_within(a, b)

static BoundaryRect **
clip_bounds_clone (BoundaryRect *bounds[])
{
	int i;
	BoundaryRect **bounds_clone;
	bounds_clone = (BoundaryRect **)
		g_new0 (gpointer, SPI_LAYER_LAST_DEFINED);
	for (i = 0; i < SPI_LAYER_LAST_DEFINED; ++i) {
		bounds_clone[i] = g_new0 (BoundaryRect, 1);
		*bounds_clone[i] = *bounds[i];
	}
	return bounds_clone;
}

static void
boundary_clip (BoundaryRect *bounds, BoundaryRect *clipBounds)
{
	int x2 = bounds->x + bounds->width;
	int y2 = bounds->y + bounds->height;
#ifdef CLIP_DEBUG
	fprintf (stderr, "bclip %d-%d, %d-%d; ",
		 bounds->x, x2,
		 clipBounds->x, clipBounds->x+clipBounds->width);
#endif		 
	bounds->x = MAX (bounds->x, clipBounds->x);
	bounds->y = MAX (bounds->y, clipBounds->y);
	x2 =  MIN (x2,  clipBounds->x + clipBounds->width);
	y2 =  MIN (y2, clipBounds->y + clipBounds->height);
	bounds->width = MAX (x2 - bounds->x, 0);
	bounds->height = MAX (y2 - bounds->y, 0);
	if (!bounds->width || !bounds->height)
		bounds->isEmpty = TRUE;
#ifdef CLIP_DEBUG
	fprintf (stderr, "%d-%d\n",
		 bounds->x, bounds->x+bounds->width);
#endif
}

static void
boundary_xclip_head (BoundaryRect *bounds, BoundaryRect *clipBounds)
{
	int x2;
	int cx2 = clipBounds->x + clipBounds->width;
	if (cx2 < bounds->x) return;
	x2 = bounds->x + bounds->width;
	if (cx2 < x2) bounds->x = cx2;
	bounds->width = MAX (0, x2 - cx2);
}

static void
boundary_xclip_tail (BoundaryRect *bounds, BoundaryRect *clipBounds)
{
	int x2 = bounds->x + bounds->width;
	if (clipBounds->x > x2) return;
	bounds->width = MAX (0, clipBounds->x - bounds->x);
}

static TextChunk*
text_chunk_copy (TextChunk *chunk)
{
	TextChunk *copy = g_new0 (TextChunk, 1);
	*copy = *chunk;
	if (chunk->string) copy->string = g_strdup (chunk->string);
	if (copy->source) AccessibleText_ref (copy->source);
	return copy;
}

static void
text_chunk_tail_clip (TextChunk *bottom, TextChunk *top)
{
#ifdef CLIP_DEBUG
	fprintf (stderr, "bottom %d-%d, top %d-%d;",
		 bottom->clip_bounds.x,
		 bottom->clip_bounds.x + bottom->clip_bounds.width,
		 top->clip_bounds.x,
		 top->clip_bounds.x + top->clip_bounds.width);
#endif
	boundary_xclip_tail (&bottom->text_bounds, &top->clip_bounds);
	boundary_xclip_tail (&bottom->clip_bounds, &top->clip_bounds);
	bottom->text_bounds.isClipped = TRUE;
	bottom->clip_bounds.isClipped = TRUE;
#ifdef CLIP_DEBUG
	fprintf (stderr, "result %d-%d\n",
		 bottom->clip_bounds.x,
		 bottom->clip_bounds.x + bottom->clip_bounds.width);
#endif
}

static void
text_chunk_head_clip (TextChunk *bottom, TextChunk *top)
{
#ifdef CLIP_DEBUG
	fprintf (stderr, "bottom %d-%d, top %d-%d;",
		 bottom->clip_bounds.x,
		 bottom->clip_bounds.x + bottom->clip_bounds.width,
		 top->clip_bounds.x,
		 top->clip_bounds.x + top->clip_bounds.width);
#endif	
	boundary_xclip_head (&bottom->text_bounds, &top->clip_bounds);
	boundary_xclip_head (&bottom->clip_bounds, &top->clip_bounds);
	bottom->text_bounds.isClipped = TRUE;
	bottom->clip_bounds.isClipped = TRUE;
#ifdef CLIP_DEBUG	
	fprintf (stderr, "result %d-%d\n",
		 bottom->clip_bounds.x,
		 bottom->clip_bounds.x + bottom->clip_bounds.width);
#endif	
}

static GList *
text_chunk_split_insert (GList *chunk_list, GList *iter, TextChunk *chunk)
{
	TextChunk *iter_chunk = iter->data;
	TextChunk *iter_copy = text_chunk_copy (iter_chunk);
	/* TODO: FIXME something is wrong here */
#ifdef CLIP_DEBUG
	fprintf (stderr, "***clip insert of %s into %s\n", chunk->string,
		 iter_chunk->string);
#endif	
	chunk_list = g_list_insert_before (chunk_list, iter, iter_copy);
	text_chunk_tail_clip (iter_copy, chunk);
	chunk_list = g_list_insert_before (chunk_list, iter, chunk);
	text_chunk_head_clip (iter_chunk, chunk);
	return chunk_list;
}

/* #define PRINT_CHUNK_DEBUG(a, b, c, d) print_chunk_debug(a, b, c, d) */

#define PRINT_CHUNK_DEBUG(a, b, c, d) 

#ifdef PRINT_CHUNK_DEBUG
static void
print_chunk_debug (TextChunk *chunk, char *opname, GList *prev, GList *next)
{
	fprintf (stderr, "%sing chunk %s between %s and %s; %d-%d\n",
		 opname,
		 chunk->string,
		 (prev ? ((TextChunk *) prev->data)->string : "<null>"),
		 (next ? ((TextChunk *) next->data)->string : "<null>"),
		 chunk->clip_bounds.x,
		 chunk->text_bounds.x + chunk->text_bounds.width);
}
#endif

static GList *
text_chunk_list_head_clip (GList *text_chunk_list,
			   TextChunk *chunk,
			   GList *next)
{
	GList *target, *iter = next, *prev;
	prev = iter->prev;
//	if (chunk->string && strlen (chunk->string)) { 
		text_chunk_list =
			g_list_insert_before (text_chunk_list, next, chunk);
//	}
	while (iter) {
		if (CHUNK_BOUNDS_SPANS_END (chunk, (TextChunk *)iter->data)) {
#ifdef CLIP_DEBUG			
			fprintf (stderr, "deleting %s\n",
				 ((TextChunk *)iter->data)->string);
#endif			
			target = iter;
			iter = iter->next;
			text_chunk_list =
				g_list_delete_link (text_chunk_list, target);
		} else {
			if (!CHUNK_BOUNDS_END_BEFORE_START (chunk,
						      (TextChunk *)iter->data)) {
				text_chunk_head_clip ((TextChunk *)iter->data,
						      chunk);
			}
			if (prev &&
			    !CHUNK_BOUNDS_AFTER_END (
				    chunk,
				    (TextChunk *)prev->data)) {
				text_chunk_tail_clip (
					(TextChunk *)prev->data,
					chunk);
			}
			break;
		}
	}
	
	return text_chunk_list;
}

static GList *
text_chunk_list_clip_and_insert (GList *text_chunk_list,
				 TextChunk *chunk,
				 GList *prev,
				 GList *next)
{
#ifdef CLIP_DEBUG
	fprintf (stderr, "clip-and-insert for %s, between %s and %s\n",
		 chunk->string,
		 (prev ? ((TextChunk *)prev->data)->string : "<null>"),
		 (next ? ((TextChunk *)next->data)->string : "<null>"));
#endif
	/* cases: */
	if (!prev && !next) { /* first element in, no clip needed */
//		if (chunk->string && strlen (chunk->string)) {
			text_chunk_list =
				g_list_append (text_chunk_list, chunk);
			PRINT_CHUNK_DEBUG (chunk, "append",
					   prev,
					   NULL);
//		}
	} else { /* check for clip with prev */
		/* if we split the prev */
		if (prev &&
		    CHUNK_BOUNDS_WITHIN (chunk, (TextChunk *) prev->data)) {
			text_chunk_list =
				text_chunk_split_insert (
					text_chunk_list,
					prev, chunk);
		} else if (next) { 
		    /* we split the 'next' element */
		    if (CHUNK_BOUNDS_WITHIN (chunk, (TextChunk *)next->data)) {
			    text_chunk_list =
				    text_chunk_split_insert (text_chunk_list,
							     next, chunk);
		    } else {
			    /* do an insert +  head clip */
			    text_chunk_list =
				    text_chunk_list_head_clip (
					    text_chunk_list,
					    chunk,
					    next);
		    }
		} else {
			if (!CHUNK_BOUNDS_AFTER_END (chunk,
						     (TextChunk *) prev->data)) {
				text_chunk_tail_clip (prev->data, chunk);
			}
//			if (chunk->string && strlen (chunk->string)) {
				text_chunk_list =
					g_list_append (text_chunk_list, chunk);
//			}
		}
	}
	return text_chunk_list;
}

static GList *
text_chunk_list_insert_chunk (GList *text_chunk_list, TextChunk *chunk)
{
	GList *iter = g_list_first (text_chunk_list);
	TextChunk *iter_chunk = NULL;
	do {
		if (iter) iter_chunk = (TextChunk *) iter->data;
                /* if we're ahead of the current element */
		if (!iter) {
			text_chunk_list =
				text_chunk_list_clip_and_insert (text_chunk_list,
								 chunk,
								 iter,
								 NULL);
			break;
		} else if (CHUNK_BOUNDS_BEFORE_START (chunk, iter_chunk)) {
			text_chunk_list =
				text_chunk_list_clip_and_insert (text_chunk_list,
								 chunk,
								 iter->prev,
								 iter);
			break;
		} else if (!iter->next ) {
			text_chunk_list =
				text_chunk_list_clip_and_insert (text_chunk_list,
								 chunk,
								 iter,
								 NULL);
			break;
		}
		if (iter) iter = iter->next;
	} while (iter);
	return text_chunk_list;
}

static TextChunk *
review_buffer_get_text_chunk (ScreenReviewBuffer *reviewBuffer,
			      Accessible *accessible, BoundaryRect *bounds,
			      int screen_x, int screen_y)
{
	AccessibleText *text = NULL;
	AccessibleRole role;
	TextChunk *text_chunk;
	BoundaryRect start_char_bounds, end_char_bounds;
	char *s = NULL;
	int offset;
	long start, end;
	long x2, y2;

	role = Accessible_getRole (accessible);
	text_chunk = g_new0 (TextChunk, 1);
	text_chunk->clip_bounds = *bounds;
	if (Accessible_isText (accessible)) {
		text = Accessible_getText (accessible);
		offset = AccessibleText_getOffsetAtPoint (text,
							  screen_x,
							  screen_y,
							  SPI_COORD_TYPE_SCREEN);
		s = AccessibleText_getTextAtOffset (text, offset,
						    SPI_TEXT_BOUNDARY_LINE_START,
						    &start, &end);
		if (end > start) {
			AccessibleText_getCharacterExtents (
				text, start,
				&text_chunk->start_char_bounds.x,
				&text_chunk->start_char_bounds.y,
				&text_chunk->start_char_bounds.width,
				&text_chunk->start_char_bounds.height,
				SPI_COORD_TYPE_SCREEN);
#ifdef CLIP_DEBUG
			fprintf (stderr, "%s: start char (%d) x, width %d %d;",
				 s,
				 start,
				 text_chunk->start_char_bounds.x,
				 text_chunk->start_char_bounds.width);
#endif
			end--; /* XXX: bug workaround */
			AccessibleText_getCharacterExtents (
				text, end--,
				&text_chunk->end_char_bounds.x,
				&text_chunk->end_char_bounds.y,
				&text_chunk->end_char_bounds.width,
				&text_chunk->end_char_bounds.height,
				SPI_COORD_TYPE_SCREEN);
#ifdef CLIP_DEBUG			
			fprintf (stderr, "end char (%d) x, width %d %d\n",
				 end,
				 text_chunk->end_char_bounds.x,
				 text_chunk->end_char_bounds.width);
#endif
		}
		text_chunk->text_bounds.x = MIN (text_chunk->start_char_bounds.x,
						 text_chunk->end_char_bounds.x);
		text_chunk->text_bounds.y = MIN (text_chunk->start_char_bounds.y,
						 text_chunk->end_char_bounds.y);
		x2 = MAX (text_chunk->start_char_bounds.x +
			  text_chunk->start_char_bounds.width,
			  text_chunk->end_char_bounds.x +
			  text_chunk->end_char_bounds.width);
		text_chunk->text_bounds.width = x2 - text_chunk->text_bounds.x;
		y2 = MAX (text_chunk->start_char_bounds.y +
			  text_chunk->start_char_bounds.height,
			  text_chunk->end_char_bounds.y + 
			  text_chunk->end_char_bounds.height);
		text_chunk->text_bounds.height = y2 - text_chunk->text_bounds.y;
		text_chunk->start_offset = start;
		text_chunk->end_offset = end;
		if ((role != SPI_ROLE_TABLE_CELL) /* XXX bug workaround */
		    && !bounds_contain_y (&text_chunk->text_bounds,
					  screen_y)) {
			s = NULL;
		}
	} else {
		if (role == SPI_ROLE_PUSH_BUTTON ||
		    role == SPI_ROLE_CHECK_BOX ||
		    role == SPI_ROLE_MENU ||
		    role == SPI_ROLE_MENU_ITEM) { /* don't like this
						     special casing :-( */
			s = Accessible_getName (accessible);
                        /* use name instead */
			text_chunk->text_bounds = text_chunk->clip_bounds;
			text_chunk->start_offset = 0;
			text_chunk->end_offset = strlen (s);
		}
	}
	if (s && strlen (s)) {
		if (s[strlen(s)-1] == '\n') s[strlen(s)-1] = ' ';
		/* XXX: if last char is newline, aren't its bounds wrong now? */
		text_chunk->string = s;
		text_chunk->source = text;
		if (text) AccessibleText_ref (text);
#ifdef CLIP_DEBUG
		fprintf (stderr, "%s, bounds %d-%d; clip %d-%d\n",
			 s,
			 text_chunk->text_bounds.x,
			 text_chunk->text_bounds.x+text_chunk->text_bounds.width,
			 text_chunk->clip_bounds.x,
			 text_chunk->clip_bounds.x+text_chunk->clip_bounds.width);
#endif		
	} else {
		text_chunk->string = NULL;
		text_chunk->source = NULL;
	}
	if (text) AccessibleText_unref (text);
	return text_chunk;
}

static void
debug_chunk_list (GList *iter)
{
	TextChunk *chunk;
	while (iter) {
		chunk = (TextChunk *)iter->data;
		fprintf (stderr, "Chunk %s, clip %d-%d, text %d-%d\n",
			 chunk->string,
			 chunk->clip_bounds.x,
			 chunk->clip_bounds.x + chunk->clip_bounds.width,
			 chunk->text_bounds.x,
			 chunk->text_bounds.x + chunk->text_bounds.width);
		iter = iter->next;
	}
}

static void
clip_into_buffers (Accessible *accessible,  BoundaryRect* parentClipBounds[],
		   ScreenReviewBuffer *reviewBuffers[],
		   int screen_x, int screen_y)
{
	int n_children, child_n;
	Accessible *child;
	BoundaryRect bounds;
	BoundaryRect** clip_bounds;
	TextChunk *text_chunk;
	AccessibleComponent *component;
	int layer;

        clip_bounds = clip_bounds_clone (parentClipBounds);
	if (Accessible_isComponent (accessible)) {
		component = Accessible_getComponent (accessible);
		layer = AccessibleComponent_getLayer (component);
		bounds = *clip_bounds[layer];
		if (!bounds.isEmpty || 1) {
			AccessibleComponent_getExtents (component,
							&bounds.x,
							&bounds.y,
							&bounds.width,
							&bounds.height,
							SPI_COORD_TYPE_SCREEN);
			if (clip_bounds[layer])
				boundary_clip (&bounds, clip_bounds[layer]);
			if (bounds_contain_y (&bounds, screen_y)) {
				text_chunk = review_buffer_get_text_chunk (
					reviewBuffers[layer], accessible, &bounds,
					screen_x, screen_y);
				reviewBuffers[layer]->text_chunks = 
					text_chunk_list_insert_chunk (
						reviewBuffers[layer]->text_chunks,
						text_chunk);
			} else {
				clip_bounds[layer]->isEmpty = TRUE;
			}
		} 
		Accessible_unref (component);
	}
	/*
	 * we always descend into children in case they are in a higher layer
	 * this can of course be optimized for the topmost layer...
	 * but nobody uses that one! (SPI_LAYER_OVERLAY)
	 */
	n_children = Accessible_getChildCount (accessible);
	for (child_n = 0; child_n < n_children; ++child_n) {
		child = Accessible_getChildAtIndex (accessible, child_n);
		clip_into_buffers (child, clip_bounds, reviewBuffers, screen_x, screen_y);
		Accessible_unref (child);
	}
	/* TODO: free the parent clip bounds */
}

static char*
text_chunk_get_clipped_string (TextChunk *chunk)
{
	char *s;
	int i;
	int len;
	gunichar c;
	GString *string = g_string_new ("");
	BoundaryRect char_bounds;
	if (!chunk->text_bounds.isClipped)
		s = chunk->string;
	else if (chunk->source) {
		len = chunk->end_offset - chunk->start_offset;
#ifdef CLIP_DEBUG		
		fprintf (stderr, "clipping %s\n", chunk->string);
#endif		
		for (i = 0; i < len; ++i) {
			AccessibleText_getCharacterExtents (chunk->source,
							    i,
							    &char_bounds.x,
							    &char_bounds.y,
							    &char_bounds.width,
							    &char_bounds.height,
							    SPI_COORD_TYPE_SCREEN);
#ifdef CHARACTER_CLIP_DEBUG
			fprintf (stderr, "testing %d-%d against %d-%d\n",
				 char_bounds.x, char_bounds.x+char_bounds.width,
				 chunk->text_bounds.x,
				 chunk->text_bounds.x + chunk->text_bounds.width);
#endif
			if (BOUNDS_CONTAIN_X (&chunk->text_bounds,
					      char_bounds.x)) {
				c = AccessibleText_getCharacterAtOffset (
					chunk->source, i);
#ifdef CLIP_DEBUG				
				fprintf (stderr, "[%c]", c);
#endif				
				g_string_append_unichar (string, c);
			}
		}
		s = string->str;
	} else { /* we're clipped, but don't implement AccessibleText :-( */
		/* punt for now, maybe we can do betterc someday */
		s = chunk->string;
	}
	g_string_free (string, FALSE);
	return s;
}

static char*
text_chunk_list_to_string (GList *iter)
{
	char *s = "";
	char *string;
	TextChunk *chunk = NULL;
	while (iter) {
		chunk = (TextChunk *)iter->data;
		if (chunk /* && chunk->string */) {
			string = text_chunk_get_clipped_string (chunk);
			if (string)
				s = g_strconcat (s, "|", string, NULL);
			else /* XXX test */
				s = g_strconcat (s, ":", NULL);
		}
		iter = iter->next;
	}
	s = g_strconcat (s, "|", NULL);
	return s;
}

static char*
review_buffer_composite (ScreenReviewBuffer *buffers[])
{
	int i;
	GList *chunk_list, *iter;
	TextChunk *chunk;
	chunk_list = buffers[0]->text_chunks;
	for (i = 1; i < SPI_LAYER_LAST_DEFINED; ++i) {
		iter = buffers[i]->text_chunks;
		fprintf (stderr, "layer %d has %d chunks\n",
			 i, g_list_length (iter));
		while (iter) {
			chunk = (TextChunk *) iter->data;
			if (chunk) {
				fprintf (stderr, "inserting chunk <%s>\n",
					 chunk->string);
				chunk_list =
					text_chunk_list_insert_chunk (chunk_list,
								      chunk);
			}
			iter = iter->next;
		}
	}
	chunk_list = buffers[SPI_LAYER_WIDGET]->text_chunks;
	return text_chunk_list_to_string (chunk_list);
}

static char *
get_screen_review_line_at (int x, int y)
{
  char *string;
  Accessible *desktop, *app, *toplevel, *child;
  AccessibleComponent *component;
  AccessibleStateSet *states;
  GList *toplevels = NULL, *actives = NULL, *iter;
  ScreenReviewBuffer* reviewBuffers[SPI_LAYER_LAST_DEFINED];
  BoundaryRect* clip_bounds[SPI_LAYER_LAST_DEFINED];
  BoundaryRect toplevel_bounds;
  int n_apps, n_toplevels, n_children, app_n, toplevel_n, child_n;
  GTimer *timer = g_timer_new ();
  int i;

  for (i = 0; i < SPI_LAYER_LAST_DEFINED; ++i) {
	  reviewBuffers[i] = g_new0 (ScreenReviewBuffer, 1);
	  clip_bounds[i] = g_new0 (BoundaryRect, 1);
	  clip_bounds[i]->isClipped = FALSE;
	  clip_bounds[i]->isEmpty = FALSE;
  }
  
  /* how do we decide which desktop ? */
  desktop = SPI_getDesktop (0);
  
  /* for each app */
  n_apps = Accessible_getChildCount (desktop);
  for (app_n = 0; app_n < n_apps; ++app_n) {
	  /* for each toplevel in app */
	  app =  Accessible_getChildAtIndex (desktop, app_n);
	  n_toplevels = Accessible_getChildCount (app);
	  for (toplevel_n = 0; toplevel_n < n_toplevels; ++toplevel_n) {
		  Accessible *toplevel = Accessible_getChildAtIndex (app, toplevel_n);
		  if (Accessible_isComponent (toplevel))
			  toplevels = g_list_prepend (toplevels, toplevel);
		  else {
			  Accessible_unref (toplevel);
			  fprintf (stderr, "warning, app toplevel not a component.\n");
		  }
	  }
  }
  
  /* sort: at the moment we don't have a good way to sort except to put actives on top */
  for (iter = g_list_first (toplevels); iter; iter = iter->next) {
	  Accessible *toplevel =
		  (Accessible *) iter->data;
	  if (AccessibleStateSet_contains (Accessible_getStateSet (toplevel),
					   SPI_STATE_ACTIVE)) {
		  actives = g_list_prepend (actives, toplevel);
	  }
  }

  for (iter = g_list_first (actives); iter; iter = actives->next) {
	  toplevels = g_list_remove (toplevels, iter->data); /* place at end */
	  toplevels = g_list_append (toplevels, iter->data);
  }
  g_list_free (actives);

  /* for each toplevel, ending with the active one(s),
   * clip against children, putting results into appropriate charBuffer.
   */
  for (iter = g_list_first (toplevels); iter; iter = iter->next) {
	  toplevel = (Accessible *) iter->data;
	  if (Accessible_isComponent (toplevel)) {
  	      /* make sure toplevel is visible and not iconified or shaded */
	      states = Accessible_getStateSet (toplevel);
	      if (AccessibleStateSet_contains (states, SPI_STATE_VISIBLE)
		  && !AccessibleStateSet_contains (states, SPI_STATE_ICONIFIED)
		  || isJava) { /* isJava hack! */
		      component = Accessible_getComponent (toplevel);
		      AccessibleComponent_getExtents (component,
						      &toplevel_bounds.x,
						      &toplevel_bounds.y,
						      &toplevel_bounds.width,
						      &toplevel_bounds.height,
						      SPI_COORD_TYPE_SCREEN);
		      toplevel_bounds.isEmpty = FALSE;
		      for (i = 0; i < SPI_LAYER_LAST_DEFINED; ++i) {
			      *clip_bounds[i] = toplevel_bounds;
		      }
#ifdef CLIP_DEBUG		      
		      fprintf (stderr, "toplevel clip starting\n");
		      debug_chunk_list (reviewBuffers[SPI_LAYER_WIDGET]->text_chunks);
#endif		      
		      clip_into_buffers (toplevel, clip_bounds,
				     reviewBuffers, x, y);
	      }
	  }
	  Accessible_unref (toplevel);
  }

  string = review_buffer_composite (reviewBuffers);

  /* SIMPLE SINGLE-PASS ALGORITHM:*/
  /* traverse the tree:
   *   keep a pointer to outermost instance of each layer
   *   clip against outermost in same layer
   *   when this clip occurs, store outermost clipped string in 2d string buffer.
   *   string buffer may have attributes to mark component bounds, line art,
   *      or attributes of text being reviewed.
   *   composite the layers, ignoring NULL chars in the string buffers.
   *
   * Limitations:
   *   sibling clip not correct, text may overwrite if siblings intersect onscreen
   *   length of resulting text buffer may vary!
   *
   * Technical issues:
   *   no API for ordering toplevels yet, other than knowing which is ACTIVE.
   *   not much implementation for the LAYER API yet, other than menus.
   */
  g_timer_stop (timer);
  fprintf (stderr, "elapsed time = %f s\n", g_timer_elapsed (timer, NULL));
  
  return string;
}

void
report_screen_review_line (const AccessibleEvent *event, void *user_data)
{
  static Display *display = NULL;
  int x, y, win_x, win_y;
  Window root_return, child_return;
  unsigned int mask_return;
  
  if (!display) display = XOpenDisplay (getenv ("DISPLAY"));
  /*
   *  we would prefer to get the x,y info in the above event.
   *  At the moment we don't get detail params for "toolkit" events,
   *  so for testing purposes we use XQueryPointer.  Actual apps
   *  probably shouldn't do this.
   */
  XQueryPointer (display,
		 DefaultRootWindow (display),
		 &root_return, &child_return,
		 &x, &y,
		 &win_x, &win_y,
		 &mask_return);

  fprintf (stderr, "screen review event %s at %d, %d\n", event->type,
	   x, y);
  fprintf (stderr, "[%s]\n", 
	   get_screen_review_line_at (x, y));
}

void
test_exit ()
{
  SPI_deregisterGlobalEventListenerAll (mouseclick_listener);
  AccessibleEventListener_unref (mouseclick_listener);
}
