#include <popt.h>
#include <gdk/gdkwindow.h>
#include <bonobo/Bonobo.h>
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
	int   refresh_time;
	int   no_bonobo;
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
					   0
                                         };

struct poptOption magnifier_options [] = {
	{"target_display", 't', POPT_ARG_STRING, &global_options.target_display, 't', "specify display on which to show magnified view", NULL},
	{"source_display", 's', POPT_ARG_STRING, &global_options.source_display, 's', "specify display to magnify", NULL},
	{"vertical", 'v', POPT_ARG_NONE, &global_options.vertical_split, 'v', "split screen vertically (if target display = source display)", NULL},
	{"horizontal", 'h', POPT_ARG_NONE, &global_options.horizontal_split, 'h', "split screen horizontally (if target display = source display)", NULL},
	{"dual-head", 'd', POPT_ARG_NONE, &global_options.dual_head, 'd', "dual-head display mode (maps magnifier to second display)", NULL},
	{"mouse follow", 'm', POPT_ARG_NONE, &global_options.mouse_follow, 'm', "track mouse movements", NULL},
	{"refresh time", 'r', POPT_ARG_NONE, &global_options.refresh_time, 'r', "refresh time for mouse follow and idle, in ms", NULL},
	{"zoom (scale) factor", 'z', POPT_ARG_FLOAT, &global_options.zoom_factor, 'z', "zoom (scale) factor used to magnify source display", NULL}, 
/*	{"invert image", 'i', POPT_ARG_NONE, &global_options.invert_image, 'i', "invert the image colormap", NULL}, */
	{"no-bonobo", '\0', POPT_ARG_NONE, &global_options.no_bonobo, '\0', "don't use bonobo for controls, use sockets", NULL},
	{NULL, 0, 0, NULL, 0, 0}
};

#define MSG_LEN 80

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

int main (int argc, char** argv){
  GtkWidget *window;
  GIOChannel *mag_channel;
  char *dpyname;
  char env_string[ENV_STRING_MAX_SIZE];

  /* TODO: finish replacing socket connection IPC with bonobo service. */
  Magnifier *magnifier;
  char * obj_id;
  
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
  magnifier->mag_data->factor =
    (int) global_options.zoom_factor; 

  /* TODO: enable fractional magnifications ? */
  
  if (global_options.target_display) {
    snprintf (env_string, (size_t) (ENV_STRING_MAX_SIZE-1), "DISPLAY=%s", global_options.target_display);
    putenv (env_string);
  }
  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
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

  image_root_window = RootWindow(magnifier->mag_data->source_display, screen_num);
  gdk_pixbuf_xlib_init (magnifier->mag_data->source_display, screen_num);
  image = gdk_pixbuf_new        (GDK_COLORSPACE_RGB,FALSE, 8,
				DisplayWidth (magnifier->mag_data->source_display,screen_num)/2,
				DisplayHeight(magnifier->mag_data->source_display,screen_num)/2);
  scaled_image = gdk_pixbuf_new (GDK_COLORSPACE_RGB,FALSE, 8,
				DisplayWidth (magnifier->mag_data->target_display,screen_num),
				DisplayHeight(magnifier->mag_data->target_display,screen_num));
  if (global_options.vertical_split)
	  magnifier->mag_data->mag_width = DisplayWidth (magnifier->mag_data->target_display,screen_num)/2;
  else
	  magnifier->mag_data->mag_width = DisplayWidth (magnifier->mag_data->target_display, screen_num);
  if (global_options.horizontal_split)
	  magnifier->mag_data->mag_height = DisplayHeight (magnifier->mag_data->target_display,screen_num)/2;
  else magnifier->mag_data->mag_height = DisplayHeight (magnifier->mag_data->target_display, screen_num);
  gtk_widget_show_all (window);

  gdk_window_move(window->window,
		  gdk_screen_width() - magnifier->mag_data->mag_width,
		  gdk_screen_height() - magnifier->mag_data->mag_height);
  gdk_window_resize (window->window, magnifier->mag_data->mag_width, magnifier->mag_data->mag_height);
  magnifier->mag_data->output_window = window;
  if (global_options.fullscreen) gdk_window_stick (window->window);
  gdk_window_set_decorations(window->window, 0);
  gdk_window_set_functions(window->window, 0);
  gdk_window_raise(window->window);
  
  gtk_timeout_add(global_options.refresh_time, display_image, magnifier->mag_data);

  obj_id = "OAFIID:Accessibility_Util_Magnifier:proto0.1";

  if (! global_options.no_bonobo)
    {	  
      bonobo_activation_active_server_register (
	  obj_id,
	  bonobo_object_corba_objref (bonobo_object (magnifier)));

      bonobo_main ();
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
}
				   
static void
impl_magnifier_set_extents (PortableServer_Servant servant,
			    CORBA_long x1,
			    CORBA_long y1,
			    CORBA_long x2,
			    CORBA_long y2,
			    CORBA_Environment *ev)
{
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
			       const CORBA_float mag_factor,
			       CORBA_Environment *ev)
{
  Magnifier *magnifier = MAGNIFIER (bonobo_object_from_servant (servant));
  magnifier->mag_data->factor = (float) mag_factor;
}

static void
impl_magnifier_exit (PortableServer_Servant servant, CORBA_Environment *ev)
{
	;
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
        epv->_set_MagFactor = impl_magnifier_set_mag_factor;
	epv->exit = impl_magnifier_exit;
}

static void
magnifier_init (Magnifier *magnifier)
{
  magnifier->mag_data = (MagnifierData *) g_new0 (MagnifierData, 1);
  magnifier->mag_data->factor = 2;
  magnifier->mag_data->contrast = 0;
  magnifier->mag_data->color_inverted = FALSE;
  magnifier->mag_data->center.x = 0;
  magnifier->mag_data->center.y = 0;
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
magnifier_new(int argc, char **argv)
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













    



