#ifndef __MAGNIFIER_SPI_IMAGE_H__
#define __MAGNIFIER_SPI_IMAGE_H__
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk-pixbuf-xlib/gdk-pixbuf-xlib.h>
#include <gdk/gdkx.h>

typedef struct _point {
  int x;
  int y;
} point;

typedef struct xlib_colormap_struct xlib_colormap;

struct xlib_colormap_struct {
	int size;
	XColor *colors;
	Visual *visual;
	Colormap colormap;
};

typedef struct {
	GdkRectangle extents;
	GdkRectangle roi;
	float zoom_x;
	float zoom_y;
	int contrast;
	gboolean is_managed;
	gboolean is_dirty;
} ZoomRegionData;

point	    center_position;
point	    curpos; /* Position of the magnified cursor */
GdkPixbuf*  image;
GdkPixbuf*  scaled_image;
Window	    spi_image_root_window;
int	    screen_num;
int         old_factor_x;
int         old_factor_y;
xlib_colormap * x_cmap;

typedef struct _MagnifierData {
	int mag_width;
	int mag_height;
	int mag_x;
	int mag_y;
	int factor_x;
	int factor_y;
	point center;
	int follow_mouse;
	int color_inverted;
	int fast_rgb_convert;
	int contrast;
	GtkWidget *output_window;
	void *source_display;
	void *target_display;
	GList *zoom_regions;
} MagnifierData;

#define	FACTOR		1
#define CONTRAST	2
#define FOLLOW_MOUSE	3
#define COLOR_INVERTED	4
#define GOTO		5
#define	STOP		6
#define SET_ROI         7
#define SET_EXTENTS     8

struct sockaddr_un mag_server;
struct sockaddr_un client_sockaddr;
int size_client;
int desc;
GtkWidget *drawing_area;

void parse_message(char *msg, MagnifierData *data);
void update_image(MagnifierData *data);
void expose_event(GtkWidget * w, GdkEventExpose *event, gpointer data);
void get_root_image(Window src,GdkPixbuf *dest,int src_x, int src_y, MagnifierData *data);
int display_image(gpointer data);

#endif

