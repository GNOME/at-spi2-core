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

#include <popt.h>
#include <gdk/gdkwindow.h>
#include <libbonobo.h>
#include "magnifier.h"
#include "mag_image.h"

#define ENV_STRING_MAX_SIZE 128

/*
 * Our parent GObject type
 */
#define PARENT_TYPE BONOBO_OBJECT_TYPE

struct sockaddr_un mag_server = { AF_UNIX ,  "/tmp/magnifier_socket" };

typedef struct {
	gchar *target_display;
	gchar *source_display;
	int   vertical_split;
	int   horizontal_split;
	int   dual_head;
	int   fullscreen;
	int   mouse_follow;
	int   invert_image;
	float zoom_factor;
	int   min_refresh_time;
	int   no_bonobo;
	int   fast_cmap_convert;
	int   no_initial_region;
} MagnifierOptions;

static MagnifierOptions global_options = { ":0.0",
					   ":0.0",
					   0,
					   0,
					   0,
					   0,
					   0,
					   0,
					   2.0,
					   200,
					   0,
					   0,
					   0
                                         };

struct poptOption magnifier_options [] = {
	{"target_display", 't', POPT_ARG_STRING, &global_options.target_display, 't', "specify display on which to show magnified view", NULL},
	{"source_display", 's', POPT_ARG_STRING, &global_options.source_display, 's', "specify display to magnify", NULL},
	{"vertical", 'v', POPT_ARG_NONE, &global_options.vertical_split, 'v', "split screen vertically (if target display = source display)", NULL},
	{"horizontal", 'h', POPT_ARG_NONE, &global_options.horizontal_split, 'h', "split screen horizontally (if target display = source display)", NULL},
	{"dual-head", 'd', POPT_ARG_NONE, &global_options.dual_head, 'd', "dual-head display mode (maps magnifier to second display)", NULL},
	{"mouse follow", 'm', POPT_ARG_NONE, &global_options.mouse_follow, 'm', "track mouse movements", NULL},
	{"refresh time", 'r', POPT_ARG_NONE, &global_options.min_refresh_time, 'r', "minimum refresh time for mouse follow and idle, in ms", NULL},
	{"zoom (scale) factor", 'z', POPT_ARG_FLOAT, &global_options.zoom_factor, 'z', "zoom (scale) factor used to magnify source display", NULL}, 
/*	{"invert image", 'i', POPT_ARG_NONE, &global_options.invert_image, 'i', "invert the image colormap", NULL}, */
  	{"fast-colormap-conversion", 'c', POPT_ARG_NONE, &global_options.fast_cmap_convert, 'c', "use faster colormap conversion algorithm (fails for 6 bit color)", NULL}, 
	{"no-bonobo", '\0', POPT_ARG_NONE, &global_options.no_bonobo, '\0', "don't use bonobo for controls, use sockets", NULL},
	{"no-initial-region", '\0', POPT_ARG_NONE, &global_options.no_initial_region, '\0', "don't create an initial zoom region", NULL},
	{NULL, 0, 0, NULL, 0, 0}
};

#define MSG_LEN 80

static GtkWidget *window; /* TODO: clean up, with accessor func? */

static void
magnifier_realize (GtkWidget *widget)
{
  XWMHints wm_hints;
  Atom wm_window_protocols[2];
  static gboolean initialized = FALSE;
  
  if (!initialized)
    {
      wm_window_protocols[0] = gdk_x11_get_xatom_by_name ("WM_DELETE_WINDOW");
      wm_window_protocols[1] = gdk_x11_get_xatom_by_name ("_NET_WM_PING");
    }
  
  wm_hints.flags = InputHint;
  wm_hints.input = False;
  
  XSetWMHints (GDK_WINDOW_XDISPLAY (widget->window),
	       GDK_WINDOW_XWINDOW (widget->window), &wm_hints);
  
  XSetWMProtocols (GDK_WINDOW_XDISPLAY (widget->window),
		   GDK_WINDOW_XWINDOW (widget->window), wm_window_protocols, 2);
}

static void
magnifier_exit()
{
  gtk_exit(0);
}

static gboolean get_commands(GIOChannel* client,
			     GIOCondition condition,
			     gpointer data){
  char msg[MSG_LEN];
  int desc_client = 0;
  
  memset(msg,0,MSG_LEN);
  
  if((desc_client = accept(desc,(struct sockaddr*)&client_sockaddr,&size_client)) == -1){
    perror("Could connect to client");exit(1);
  }
  
  read(desc_client,msg,sizeof(msg));
  parse_message(msg, data);
  return TRUE;

}

static void
magnifier_pack_regions (Magnifier *magnifier)
{
  /* 
   * this call prevents resizing, which is a bother, but required to 
   * work around dtwm incompatibilities.  Perhaps a better workaround will
   * be found, or we can make this a runtime option.
   */ 
  gtk_widget_set_size_request (magnifier->mag_data->output_window, 
			       magnifier->mag_data->mag_width, 
			       magnifier->mag_data->mag_height);

  gdk_window_move(magnifier->mag_data->output_window->window,
		  magnifier->mag_data->mag_x,
		  magnifier->mag_data->mag_y);

}

int main (int argc, char** argv){
  GIOChannel *mag_channel;
  char *dpyname;
  char env_string[ENV_STRING_MAX_SIZE];

  /* TODO: finish replacing socket connection IPC with bonobo service. */
  Magnifier *magnifier;
  char * obj_id;
  GdkGeometry geometry;
  
  x_cmap = NULL;

  size_client = sizeof(client_sockaddr);

  image = NULL;

  if (!bonobo_init (&argc, argv))
    {
      g_error ("Could not initialize Bonobo");
    }

  magnifier = magnifier_new (argc, argv);
  
  magnifier->mag_data->follow_mouse =
	  global_options.mouse_follow;
  magnifier->mag_data->color_inverted =
	  global_options.invert_image;
  if (!global_options.no_initial_region) 
    {
      magnifier->mag_data->zoom_regions =
	      g_list_prepend (magnifier->mag_data->zoom_regions,
			      g_new0 (ZoomRegionData, 1));
      magnifier->mag_data->factor_x = (int) global_options.zoom_factor; 
      magnifier->mag_data->factor_y = (int) global_options.zoom_factor;
    }
  else {
      g_print ("starting magnifier with no initial zoom region\n");
      magnifier->mag_data->mag_width = 0; 
      magnifier->mag_data->mag_height = 0;
  }
      /* TODO: enable fractional magnifications option? */
  if (global_options.target_display) {
    g_snprintf (env_string, (size_t) (ENV_STRING_MAX_SIZE-1), "DISPLAY=%s", global_options.target_display);
    putenv (env_string);
  }
  gtk_init (&argc, &argv);

  window = g_object_connect (gtk_widget_new (gtk_window_get_type (),
					     "user_data", NULL,
					     "can_focus", FALSE,
					     "type", GTK_WINDOW_TOPLEVEL,
					     "title", "magnifier",
					     "allow_grow", FALSE,
					     "allow_shrink", FALSE,
					     "border_width", 10,
					     NULL),
			     "signal::realize", magnifier_realize, NULL,
			     "signal::destroy", magnifier_exit, NULL,
			     NULL);
  
  drawing_area = gtk_drawing_area_new();
  gtk_container_add (GTK_CONTAINER (window),drawing_area);
  gtk_widget_add_events(GTK_WIDGET(drawing_area),GDK_BUTTON_PRESS_MASK);
  gtk_signal_connect (GTK_OBJECT (drawing_area),"expose_event",
		      GTK_SIGNAL_FUNC(expose_event),NULL);
  
  /* Init socket */
  if (global_options.no_bonobo) {
    if((desc = socket(AF_UNIX,SOCK_STREAM,0)) == -1){
      perror("Socket");exit(1);
    }
    unlink("/tmp/magnifier_socket");

    if(bind(desc,(struct sockaddr*)&mag_server,sizeof(mag_server)) == -1){
       perror("Bind");exit(1);
    }

    if(listen(desc,100) == -1){
       perror("Listen");exit(1);
    }
    mag_channel = g_io_channel_unix_new(desc);
    g_io_add_watch(mag_channel,
		   G_IO_IN | G_IO_ERR,
		   get_commands,
		   magnifier->mag_data);

  }
  
  /* init image information */
  if (!global_options.source_display) global_options.source_display = gdk_get_display();
  dpyname = global_options.source_display + (strlen(global_options.source_display) - 1);
  screen_num = atoi(dpyname);
  if (!global_options.target_display) {
    if((screen_num == 0) && global_options.dual_head)
      screen_num++;
    else if (global_options.dual_head)
      screen_num--;
    global_options.target_display =
	    (char *) malloc (strlen (global_options.source_display));
    strncpy (global_options.target_display,
	     global_options.source_display,
	     strlen(global_options.source_display)-2);
    global_options.target_display[strlen(global_options.source_display)-2] = 0;
    sprintf(global_options.target_display, "%s.%d",
	    global_options.target_display, screen_num);
  }

  printf("displays: source=%s; target=%s\n",
	 global_options.source_display,
	 global_options.target_display);
  
  magnifier->mag_data->source_display = XOpenDisplay (global_options.source_display);
  magnifier->mag_data->target_display = GDK_DISPLAY();

  spi_image_root_window = RootWindow(magnifier->mag_data->source_display, screen_num);
  gdk_pixbuf_xlib_init (magnifier->mag_data->source_display, screen_num);
  image = gdk_pixbuf_new        (GDK_COLORSPACE_RGB,FALSE, 8,
				DisplayWidth (magnifier->mag_data->source_display,screen_num)/2,
				DisplayHeight(magnifier->mag_data->source_display,screen_num)/2);
  scaled_image = gdk_pixbuf_new (GDK_COLORSPACE_RGB,FALSE, 8,
				DisplayWidth (magnifier->mag_data->target_display,screen_num),
				DisplayHeight(magnifier->mag_data->target_display,screen_num));
  if (global_options.vertical_split)
    {
      magnifier->mag_data->mag_width =
	      DisplayWidth (magnifier->mag_data->target_display,screen_num)/2;
      magnifier->mag_data->mag_height =
	      DisplayHeight (magnifier->mag_data->target_display, screen_num);
    }
  else if (global_options.horizontal_split)
    {
      magnifier->mag_data->mag_width =
	    DisplayWidth (magnifier->mag_data->target_display, screen_num);
      magnifier->mag_data->mag_height =
	    DisplayHeight (magnifier->mag_data->target_display,screen_num)/2;
    }
  else if (global_options.fullscreen)
    {
      magnifier->mag_data->mag_width =
	    DisplayWidth (magnifier->mag_data->target_display, screen_num);
      magnifier->mag_data->mag_height =
	    DisplayHeight (magnifier->mag_data->target_display,screen_num);
    }
  magnifier->mag_data->mag_x =
	  DisplayWidth (magnifier->mag_data->target_display, screen_num)
	  - magnifier->mag_data->mag_width;
  magnifier->mag_data->mag_y =
	  DisplayHeight (magnifier->mag_data->target_display, screen_num)
	  - magnifier->mag_data->mag_height;

  magnifier->mag_data->output_window = window;
  gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
  gtk_widget_show_all (window);

  magnifier_pack_regions (magnifier);
  
  /* if (global_options.fullscreen) */
  gdk_window_stick (window->window);
  gdk_window_set_functions(window->window, 0);
  gdk_window_raise(window->window);
  
  gtk_timeout_add(global_options.min_refresh_time, display_image, magnifier->mag_data);

  obj_id = "OAFIID:Accessibility_Util_Magnifier:proto0.1";

  if (! global_options.no_bonobo)
    {	  
      int ret = bonobo_activation_active_server_register (
	obj_id, BONOBO_OBJREF (magnifier));

      if (ret == Bonobo_ACTIVATION_REG_SUCCESS)
        {
          bonobo_main ();
	}
    }
  else
    {
      gtk_main ();
    }

  unlink("magnifier_socket");
  return 0;
}

static void
impl_magnifier_fullscreen (PortableServer_Servant servant,
			   CORBA_Environment *ev)
{
  Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
  magnifier->mag_data->mag_width = DisplayWidth (magnifier->mag_data->target_display, screen_num);
  magnifier->mag_data->mag_height = DisplayHeight (magnifier->mag_data->target_display, screen_num);
}
				   
static void
impl_magnifier_set_extents (PortableServer_Servant servant,
			    CORBA_long x1,
			    CORBA_long y1,
			    CORBA_long x2,
			    CORBA_long y2,
			    CORBA_Environment *ev)
{
  Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
  magnifier->mag_data->mag_width = x2 - x1;
  magnifier->mag_data->mag_height = y2 - y1;
  gdk_window_move(window->window, x1, y1);
}

static void
impl_magnifier_set_follow_mouse (PortableServer_Servant servant,
				 const CORBA_boolean follow_mouse,
				 CORBA_Environment *ev)
{
  Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
  magnifier->mag_data->follow_mouse = (int) follow_mouse;
}

static void
impl_magnifier_set_contrast (PortableServer_Servant servant,
			     const CORBA_short contrast,
			     CORBA_Environment *ev)
{
  Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
  magnifier->mag_data->contrast = (int) contrast;
}

static void
impl_magnifier_set_source_display (PortableServer_Servant servant,
				   const CORBA_char *display,
				   CORBA_Environment *ev)
{
  Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
  magnifier->mag_data->source_display =
	  XOpenDisplay (global_options.source_display);
}

static void
impl_magnifier_set_target_display (PortableServer_Servant servant,
				   const CORBA_char *display,
				   CORBA_Environment *ev)
{
  Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
  magnifier->mag_data->target_display = GDK_DISPLAY();
}

static void
impl_magnifier_goto (PortableServer_Servant servant,
		     const CORBA_long x,
		     const CORBA_long y,
		     CORBA_Environment *ev)
{
  Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
  magnifier->mag_data->center.x = (int) x;
  magnifier->mag_data->center.y = (int) y;
}

static void
impl_magnifier_set_roi (PortableServer_Servant servant,
			const CORBA_short zoom_region,
			const CORBA_long x1,
			const CORBA_long y1,
			const CORBA_long x2,
			const CORBA_long y2,
			CORBA_Environment *ev)
{
  Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
  magnifier->mag_data->center.x = (int) ((x1 + x2)/2);
  magnifier->mag_data->center.y = (int) ((y1 + y2)/2);
}

static void
impl_magnifier_set_mag_factor (PortableServer_Servant servant,
			       const CORBA_short zoom_region,
			       const CORBA_float mag_factor_x,
			       const CORBA_float mag_factor_y,
			       CORBA_Environment *ev)
{
  Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
  if (zoom_region == (CORBA_short) 0) /* TODO: fix for multi-zoom-regions */
    {
      magnifier->mag_data->factor_x = (float) mag_factor_x;
      magnifier->mag_data->factor_y = (float) mag_factor_y;
    }
}

static void
impl_magnifier_mark_dirty (PortableServer_Servant servant,
			   const CORBA_short zoom_region,
			   const CORBA_long x1,
			   const CORBA_long y1,
			   const CORBA_long x2,
			   const CORBA_long y2,
			   CORBA_Environment *ev)
{
  Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
  /* TODO: implement */
}

static void
impl_magnifier_mark_unmanaged (PortableServer_Servant servant,
			       const CORBA_short zoom_region,
			       CORBA_Environment *ev)
{
  Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
  /* TODO: implement */
}

static CORBA_short
impl_magnifier_create_zoom_region (PortableServer_Servant servant,
				   const CORBA_float zx,
				   const CORBA_float zy,
				   const CORBA_long x1,
				   const CORBA_long y1,
				   const CORBA_long x2,
				   const CORBA_long y2,
				   CORBA_Environment *ev)
{
  Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
  if (magnifier->mag_data->zoom_regions == NULL)
    {
      magnifier->mag_data->zoom_regions =
	      g_list_prepend (magnifier->mag_data->zoom_regions,
			      g_new0 (ZoomRegionData, 1));
      magnifier->mag_data->factor_x = (int) zx; 
      magnifier->mag_data->factor_y = (int) zy;
      magnifier->mag_data->mag_x = x1;
      magnifier->mag_data->mag_y = y1;
      magnifier->mag_data->mag_width = (x2 - x1);
      magnifier->mag_data->mag_height = (y2 - y1);
      magnifier_pack_regions (magnifier);
      return 0;
    }
  else
    {
      return -1;
    }
}

static CORBA_boolean
impl_magnifier_get_zoom_region_params (PortableServer_Servant servant,
				       const CORBA_short zoom_region,
				       CORBA_float * zx,
				       CORBA_float * zy, CORBA_long * x1,
				       CORBA_long * y1, CORBA_long * x2,
				       CORBA_long * y2,
				       CORBA_Environment * ev)
{
  Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
  if (zoom_region == (CORBA_short) 0)
  {
    *zx = magnifier->mag_data->factor_x;
    *zy = magnifier->mag_data->factor_y;
    *x1 = 0;
    *y1 = 0;
    *x2 = *x1 + magnifier->mag_data->mag_width;
    *y2 = *y1 + magnifier->mag_data->mag_height;
    return CORBA_TRUE;	  
  }
  else return CORBA_FALSE;
}

static void
impl_magnifier_resize_zoom_region (PortableServer_Servant servant,
				   const CORBA_short zoom_region,
				   const CORBA_long x1, const CORBA_long y1,
				   const CORBA_long x2, const CORBA_long y2,
				   CORBA_Environment * ev)
{
  Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
  if (zoom_region == 0)
    {
      magnifier->mag_data->mag_x = x1;
      magnifier->mag_data->mag_y = y1;
      magnifier->mag_data->mag_width = (x2 - x1);
      magnifier->mag_data->mag_height = (y2 - y1);
      magnifier_pack_regions (magnifier);
    }
}

static void
impl_magnifier_destroy_zoom_region (PortableServer_Servant servant,
				    const CORBA_short zoom_region,
				    CORBA_Environment * ev)
{
  Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
  if (zoom_region == 0)
    {
      g_list_free (magnifier->mag_data->zoom_regions);
    }
}

static void
impl_magnifier_clear_all_zoom_regions (PortableServer_Servant servant,
				       CORBA_Environment * ev)
{
  Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
  g_list_free (magnifier->mag_data->zoom_regions);
  magnifier->mag_data->zoom_regions = NULL;
}

static void
impl_magnifier_exit (PortableServer_Servant servant, CORBA_Environment *ev)
{
	Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
	if (magnifier->mag_data->zoom_regions) 
		g_list_free (magnifier->mag_data->zoom_regions);
	g_free (magnifier->mag_data);
	magnifier_exit();
}

static void
magnifier_class_init (MagnifierClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        POA_Accessibility_Magnifier__epv *epv = &klass->epv;
/*
        object_class->finalize = magnifier_finalize;
*/
        epv->_set_SourceDisplay = impl_magnifier_set_source_display;
	epv->_set_TargetDisplay = impl_magnifier_set_target_display;
	epv->setROI = impl_magnifier_set_roi;
        epv->setMagFactor = impl_magnifier_set_mag_factor;
        epv->markDirty = impl_magnifier_mark_dirty;
        epv->markUnmanaged = impl_magnifier_mark_unmanaged;
	epv->createZoomRegion = impl_magnifier_create_zoom_region;
	epv->getZoomRegionParams = impl_magnifier_get_zoom_region_params;
	epv->resizeZoomRegion = impl_magnifier_resize_zoom_region;
	epv->destroyZoomRegion = impl_magnifier_destroy_zoom_region;
	epv->clearAllZoomRegions = impl_magnifier_clear_all_zoom_regions;
	epv->exit = impl_magnifier_exit;
}

static void
magnifier_init (Magnifier *magnifier)
{
  magnifier->mag_data = (MagnifierData *) g_new0 (MagnifierData, 1);
  magnifier->mag_data->factor_x = 2;
  magnifier->mag_data->factor_y = 2;
  magnifier->mag_data->contrast = 0;
  magnifier->mag_data->color_inverted = FALSE;
  magnifier->mag_data->fast_rgb_convert = FALSE;
  magnifier->mag_data->center.x = 0;
  magnifier->mag_data->center.y = 0;
  magnifier->mag_data->zoom_regions = NULL;
}

GType
magnifier_get_type (void)
{
        static GType type = 0;

        if (!type) {
                static const GTypeInfo tinfo = {
                        sizeof (MagnifierClass),
                        (GBaseInitFunc) NULL,
                        (GBaseFinalizeFunc) NULL,
                        (GClassInitFunc) magnifier_class_init,
                        (GClassFinalizeFunc) NULL,
                        NULL, /* class data */
                        sizeof (Magnifier),
                        0, /* n preallocs */
                        (GInstanceInitFunc) magnifier_init,
                        NULL /* value table */
                };
                /*
                 *   Here we use bonobo_type_unique instead of
                 * gtk_type_unique, this auto-generates a load of
                 * CORBA structures for us. All derived types must
                 * use bonobo_type_unique.
                 */
                type = bonobo_type_unique (
                        PARENT_TYPE,
                        POA_Accessibility_Magnifier__init,
                        NULL,
                        G_STRUCT_OFFSET (MagnifierClass, epv),
                        &tinfo,
                        "Magnifier");
        }

        return type;
}

Magnifier *
magnifier_new (int argc, char **argv)
{
  poptContext ctx;	
  Magnifier *magnifier =
	  MAGNIFIER (g_object_new (magnifier_get_type(), NULL));
  ctx = poptGetContext ("magnifier",
			argc,
			(const char **)argv,
			magnifier_options,
			0);

  while (poptGetNextOpt (ctx) >= 0)
	/**/;

  return magnifier;
}
