#include "mag_image.h"


static int get_num(char* msg);
static void get_coord(char* msg,point *p);
static void get_rect(char* msg, point *p1, point *p2);
static void compute_center_coord(point p1, point p2, point *center);
static xlib_colormap * xlib_get_colormap (Colormap id, Visual *visual, MagnifierData *data);
static guint32 mask_table[] = {
	0x00000000, 0x00000001, 0x00000003, 0x00000007,
	0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
	0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
	0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
	0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
	0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
	0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff,
	0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff,
	0xffffffff
};

void parse_message(char *msg, MagnifierData *data){

  int type;
  point roi_pos, roi_size;
  point mag_pos, mag_size;
  
  printf("%s\n",msg);
  if(msg[0] != '~'){
    fprintf(stderr,"corrupted message, discarding\n");
    return;
  }
  type = atoi((char*)&msg[1]);
  switch (type){
	case FACTOR :
	    old_factor_x = data->factor_x;
	    data->factor_x = get_num(msg);
	    old_factor_y = data->factor_y;
	    data->factor_y = get_num(msg);
	    printf("FACTOR = %d\n",data->factor_x);
	    break;
	case CONTRAST :
	    data->contrast = get_num(msg);
	    printf("CONTRAST = %d\n",data->contrast);
	    break;
	case FOLLOW_MOUSE :
	    data->follow_mouse = get_num(msg);
	    printf("FOLLOW_MOUSE = %d\n",data->follow_mouse);
	    break;
	case COLOR_INVERTED :
	    data->color_inverted = get_num(msg);
	    printf("COLOR_INVERTED = %d\n",data->color_inverted);
	    break;
	case GOTO :
	    get_coord(msg, &data->center);
	    printf("GOTO = (%d,%d)\n", data->center.x, data->center.y);
	    break;
        case SET_ROI:
	    get_rect (msg, &roi_pos, &roi_size);
	    printf("ROI = (%d,%d; %d,%d)\n", roi_pos.x, roi_pos.y, roi_size.x, roi_size.y);
	    compute_center_coord (roi_pos, roi_size, &data->center);
	    break;
        case SET_EXTENTS:
	    get_rect (msg, &mag_pos, &mag_size);
	    printf("EXTENTS = (%d,%d; %d,%d)\n", mag_pos.x, mag_pos.y, mag_size.x, mag_size.y);
	    data->mag_width = mag_size.x;
	    data->mag_height = mag_size.y;
	    gdk_window_move_resize (data->output_window->window, mag_pos.x,
				    mag_pos.y,
				    data->mag_width, data->mag_height);
	    break;
        case STOP :
	    printf("STOP\n");
	    gtk_main_quit();
	    break;
	default:
	    break;
  }
}


static int get_num(char* msg){
 int type,num;
 sscanf(msg,"~%d:%d:",&type,&num);
 return num;
}

static void get_coord(char* msg,point *p){
 int type;
 sscanf(msg,"~%d:%d,%d:",&type,&p->x,&p->y);
}

static void get_rect(char* msg, point *p1, point *p2){
 int type;
 sscanf(msg,"~%d:%d,%d;%d,%d:",&type, &p1->x, &p1->y, &p2->x, &p2->y);
}

static void compute_center_coord (point pos, point size, point *center) {
  center->x = pos.x + (size.x/2);
  center->y = pos.y + (size.y/2);
}

int display_image(gpointer data)
{
  MagnifierData *mag_data = (MagnifierData *)data;
  update_image(mag_data);
  gdk_pixbuf_scale(image,
		   scaled_image,
		   0,
		   0,
		   DisplayWidth (mag_data->target_display,screen_num),
		   DisplayHeight(mag_data->target_display,screen_num),
		   0,
		   0,
		   mag_data->factor_x,
		   mag_data->factor_y,
		   GDK_INTERP_NEAREST);
  
  gdk_pixbuf_render_to_drawable (scaled_image,
			drawing_area->window,
			drawing_area->style->fg_gc[GTK_STATE_NORMAL],
			0,0,0,0,DisplayWidth (mag_data->target_display,screen_num),
			DisplayHeight(mag_data->target_display,screen_num),
			GDK_RGB_DITHER_NORMAL,0,0);
  /* TODO: raise this window to top on refresh */
  return TRUE;
}

void update_image(MagnifierData *mag_data)
{
 int x, y, total_width, total_height;
  
 if(mag_data->follow_mouse){
    Window root_return, child_return;
    int win_x_return,win_y_return;
    unsigned int mask_return;
    XQueryPointer(mag_data->source_display,spi_image_root_window,
		  &root_return,&child_return,
		  &mag_data->center.x,&mag_data->center.y,
		  &win_x_return,&win_y_return,&mask_return);
  }

  total_width = DisplayWidth (mag_data->source_display,screen_num);
  total_height = DisplayHeight(mag_data->source_display,screen_num);

  x = mag_data->center.x - mag_data->mag_width/mag_data->factor_x/2;
  y = mag_data->center.y - mag_data->mag_height/mag_data->factor_y/2;

  if(mag_data->center.x < mag_data->mag_width/mag_data->factor_x/2)
    x = 0;
  if(mag_data->center.x > (total_width - mag_data->mag_width/mag_data->factor_x/2))
    x = total_width - mag_data->mag_width/mag_data->factor_x;
  if(mag_data->center.y < mag_data->mag_height/mag_data->factor_y/2)
    y = 0;
  if(mag_data->center.y > (total_height - mag_data->mag_height/mag_data->factor_y/2))
    y = total_height - mag_data->mag_height/mag_data->factor_y;
  if(x < 0)
    x = 0;
  if(y < 0)
    y = 0;
/*
  printf("p = (%d,%d), x,y = (%d,%d), h,w = (%d,%d), ht,wt = (%d,%d)\n",
					mag_data->center.x,
					mag_data->center.y,
					x,y,
					mag_data->mag_height,mag_data->mag_width,
					total_height,
					total_width);
*/
  if(mag_data->factor_x != old_factor_x || mag_data->factor_y != old_factor_y){
    g_object_unref((GObject *)image);
    image = gdk_pixbuf_new (GDK_COLORSPACE_RGB,FALSE, 8,
				DisplayWidth (mag_data->target_display,screen_num)/mag_data->factor_x,
				DisplayHeight(mag_data->target_display,screen_num)/mag_data->factor_y);
    /* yes, use target display above, since the size of the area grabbed depends on the target */
    old_factor_x = mag_data->factor_x;
    old_factor_y = mag_data->factor_y;
  }
  get_root_image(spi_image_root_window,
		 image,
		 x,
		 y,
		 mag_data);
}
void expose_event(GtkWidget * w, GdkEventExpose *event, gpointer data){

}




/* SpiImage grabbing and convertion routines from gdk-pixbuf-xlib */
/*
  convert 1 bits-pixel data
  no alpha
*/
static void
rgb1 (XImage *image, guchar *pixels, int rowstride, xlib_colormap *colormap)
{
	int xx, yy;
	int width, height;
	int bpl;
	guint8 *s;
	register guint8 data;
	guint8 *o;
	guint8 *srow = image->data, *orow = pixels;

	/* convert upto 8 pixels/time */
	/* its probably not worth trying to make this run very fast, who uses
	   1 bit displays anymore? */
	width = image->width;
	height = image->height;
	bpl = image->bytes_per_line;

	for (yy = 0; yy < height; yy++) {
		s = srow;
		o = orow;

		for (xx = 0; xx < width; xx ++) {
			data = srow[xx >> 3] >> (7 - (xx & 7)) & 1;
			*o++ = colormap->colors[data].red;
			*o++ = colormap->colors[data].green;
			*o++ = colormap->colors[data].blue;
		}
		srow += bpl;
		orow += rowstride;
	}
}

/*
  convert 1 bits/pixel data
  with alpha
*/
static void
rgb1a (XImage *image, guchar *pixels, int rowstride, xlib_colormap *colormap)
{
	int xx, yy;
	int width, height;
	int bpl;
	guint8 *s;
	register guint8 data;
	guint8 *o;
	guint8 *srow = image->data, *orow = pixels;
	guint32 remap[2];

	/* convert upto 8 pixels/time */
	/* its probably not worth trying to make this run very fast, who uses
	   1 bit displays anymore? */
	width = image->width;
	height = image->height;
	bpl = image->bytes_per_line;

	for (xx = 0; xx < 2; xx++) {
#ifdef LITTLE
		remap[xx] = 0xff000000
			| colormap->colors[xx].blue << 16
			| colormap->colors[xx].green << 8
			| colormap->colors[xx].red;
#else
		remap[xx] = 0xff
			| colormap->colors[xx].red << 24
			| colormap->colors[xx].green << 16
			| colormap->colors[xx].blue << 8;
#endif
	}

	for (yy = 0; yy < height; yy++) {
		s = srow;
		o = orow;

		for (xx = 0; xx < width; xx ++) {
			data = srow[xx >> 3] >> (7 - (xx & 7)) & 1;
			*o++ = remap[data];
		}
		srow += bpl;
		orow += rowstride;
	}
}

/*
  convert 8 bits/pixel data
  no alpha
*/
static void
rgb8 (XImage *image, guchar *pixels, int rowstride, xlib_colormap *colormap)
{
	int xx, yy;
	int width, height;
	int bpl;
	guint32 mask;
	register guint32 data;
	guint8 *srow = image->data, *orow = pixels;
	register guint8 *s;
	register guint8 *o;

	width = image->width;
	height = image->height;
	bpl = image->bytes_per_line;

	mask = mask_table[image->depth];

	for (yy = 0; yy < height; yy++) {
		s = srow;
		o = orow;
		for (xx = 0; xx < width; xx++) {
			data = *s++ & mask;
			*o++ = colormap->colors[data].red;
			*o++ = colormap->colors[data].green;
			*o++ = colormap->colors[data].blue;
		}
		srow += bpl;
		orow += rowstride;
	}
}

/*
  convert 8 bits/pixel data
  with alpha
*/
static void
rgb8a (XImage *image, guchar *pixels, int rowstride, xlib_colormap *colormap)
{
	int xx, yy;
	int width, height;
	int bpl;
	guint32 mask;
	register guint32 data;
	guint32 remap[256];
	register guint8 *s;	/* read 2 pixels at once */
	register guint32 *o;
	guint8 *srow = image->data, *orow = pixels;

	width = image->width;
	height = image->height;
	bpl = image->bytes_per_line;

	mask = mask_table[image->depth];

	for (xx = 0; xx < colormap->size; xx++) {
#ifdef LITTLE
		remap[xx] = 0xff000000
			| colormap->colors[xx].blue << 16
			| colormap->colors[xx].green << 8
			| colormap->colors[xx].red;
#else
		remap[xx] = 0xff
			| colormap->colors[xx].red << 24
			| colormap->colors[xx].green << 16
			| colormap->colors[xx].blue << 8;
#endif
	}

	for (yy = 0; yy < height; yy++) {
		s = srow;
		o = (guint32 *) orow;
		for (xx = 0; xx < width; xx ++) {
			data = *s++ & mask;
			*o++ = remap[data];
		}
		srow += bpl;
		orow += rowstride;
	}
}

/*
  convert 16 bits/pixel data
  no alpha
  data in lsb format
*/
static void
rgb565lsb (XImage *image, guchar *pixels, int rowstride, xlib_colormap *colormap)
{
	int xx, yy;
	int width, height;
	int bpl;

#ifdef LITTLE
	register guint32 *s;	/* read 2 pixels at once */
#else
	register guint8 *s;	/* read 2 pixels at once */
#endif
	register guint16 *o;
	guint8 *srow = image->data, *orow = pixels;

	width = image->width;
	height = image->height;
	bpl = image->bytes_per_line;

	for (yy = 0; yy < height; yy++) {
#ifdef LITTLE
		s = (guint32 *) srow;
#else
		s = srow;
#endif
		o = (guint16 *) orow;
		for (xx = 1; xx < width; xx += 2) {
			register guint32 data;
#ifdef LITTLE
			data = *s++;
			*o++ = (data & 0xf800) >> 8 | (data & 0xe000) >> 13
				| (data & 0x7e0) << 5 | (data & 0x600) >> 1;
			*o++ = (data & 0x1f) << 3 | (data & 0x1c) >> 2
				| (data & 0xf8000000) >> 16 | (data & 0xe0000000) >> 21;
			*o++ = (data & 0x7e00000) >> 19 | (data & 0x6000000) >> 25
				| (data & 0x1f0000) >> 5 | (data & 0x1c0000) >> 10;
#else
			/* swap endianness first */
			data = s[0] | s[1] << 8 | s[2] << 16 | s[3] << 24;
			s += 4;
			*o++ = (data & 0xf800) | (data & 0xe000) >> 5
				| (data & 0x7e0) >> 3 | (data & 0x600) >> 9;
			*o++ = (data & 0x1f) << 11 | (data & 0x1c) << 6
				| (data & 0xf8000000) >> 24 | (data & 0xe0000000) >> 29;
			*o++ = (data & 0x7e00000) >> 11 | (data & 0x6000000) >> 17
				| (data & 0x1f0000) >> 13 | (data & 0x1c0000) >> 18;
#endif
		}
		/* check for last remaining pixel */
		if (width & 1) {
			register guint16 data;
#ifdef LITTLE
			data = *((short *) s);
#else
			data = *((short *) s);
			data = ((data >> 8) & 0xff) | ((data & 0xff) << 8);
#endif
			((char *) o)[0] = ((data >> 8) & 0xf8) | ((data >> 13) & 0x7);
			((char *) o)[1] = ((data >> 3) & 0xfc) | ((data >> 9) & 0x3);
			((char *) o)[2] = ((data << 3) & 0xf8) | ((data >> 2) & 0x7);
		}
		srow += bpl;
		orow += rowstride;
	}
}

/*
  convert 16 bits/pixel data
  no alpha
  data in msb format
*/
static void
rgb565msb (XImage *image, guchar *pixels, int rowstride, xlib_colormap *colormap)
{
	int xx, yy;
	int width, height;
	int bpl;

#ifdef LITTLE
	register guint8 *s;	/* need to swap data order */
#else
	register guint32 *s;	/* read 2 pixels at once */
#endif
	register guint16 *o;
	guint8 *srow = image->data, *orow = pixels;

	width = image->width;
	height = image->height;
	bpl = image->bytes_per_line;

	for (yy = 0; yy < height; yy++) {
#ifdef LITTLE
		s = srow;
#else
		s = (guint32 *) srow;
#endif
		o = (guint16 *) orow;
		for (xx = 1; xx < width; xx += 2) {
			register guint32 data;
#ifdef LITTLE
			/* swap endianness first */
			data = s[0] | s[1] << 8 | s[2] << 16 | s[3] << 24;
			s += 4;
			*o++ = (data & 0xf800) >> 8 | (data & 0xe000) >> 13
				| (data & 0x7e0) << 5 | (data & 0x600) >> 1;
			*o++ = (data & 0x1f) << 3 | (data & 0x1c) >> 2
				| (data & 0xf8000000) >> 16 | (data & 0xe0000000) >> 21;
			*o++ = (data & 0x7e00000) >> 19 | (data & 0x6000000) >> 25
				| (data & 0x1f0000) >> 5 | (data & 0x1c0000) >> 10;
#else
			data = *s++;
			*o++ = (data & 0xf800) | (data & 0xe000) >> 5
				| (data & 0x7e0) >> 3 | (data & 0x600) >> 9;
			*o++ = (data & 0x1f) << 11 | (data & 0x1c) << 6
				| (data & 0xf8000000) >> 24 | (data & 0xe0000000) >> 29;
			*o++ = (data & 0x7e00000) >> 11 | (data & 0x6000000) >> 17
				| (data & 0x1f0000) >> 13 | (data & 0x1c0000) >> 18;
#endif
		}
		/* check for last remaining pixel */
		if (width & 1) {
			register guint16 data;
#ifdef LITTLE
			data = *((short *) s);
			data = ((data >> 8) & 0xff) | ((data & 0xff) << 8);
#else
			data = *((short *) s);
#endif
			((char *) o)[0] = ((data >> 8) & 0xf8) | ((data >> 13) & 0x7);
			((char *) o)[1] = ((data >> 3) & 0xfc) | ((data >> 9) & 0x3);
			((char *) o)[2] = ((data << 3) & 0xf8) | ((data >> 2) & 0x7);
		}
		srow += bpl;
		orow += rowstride;
	}
}

/*
  convert 16 bits/pixel data
  with alpha
  data in lsb format
*/
static void
rgb565alsb (XImage *image, guchar *pixels, int rowstride, xlib_colormap *colormap)
{
	int xx, yy;
	int width, height;
	int bpl;

#ifdef LITTLE
	register guint16 *s;	/* read 1 pixels at once */
#else
	register guint8 *s;
#endif
	register guint32 *o;

	guint8 *srow = image->data, *orow = pixels;

	width = image->width;
	height = image->height;
	bpl = image->bytes_per_line;

	for (yy = 0; yy < height; yy++) {
#ifdef LITTLE
		s = (guint16 *) srow;
#else
		s = (guint8 *) srow;
#endif
		o = (guint32 *) orow;
		for (xx = 0; xx < width; xx ++) {
			register guint32 data;
			/*  rrrrrggg gggbbbbb -> rrrrrRRR ggggggGG bbbbbBBB aaaaaaaa */
			/*  little endian: aaaaaaaa bbbbbBBB ggggggGG rrrrrRRR */
#ifdef LITTLE
			data = *s++;
			*o++ = (data & 0xf800) >> 8 | (data & 0xe000) >> 13
				| (data & 0x7e0) << 5 | (data & 0x600) >> 1
				| (data & 0x1f) << 19 | (data & 0x1c) << 14
				| 0xff000000;
#else
			/* swap endianness first */
			data = s[0] | s[1] << 8;
			s += 2;
			*o++ = (data & 0xf800) << 16 | (data & 0xe000) << 11
				| (data & 0x7e0) << 13 | (data & 0x600) << 7
				| (data & 0x1f) << 11 | (data & 0x1c) << 6
				| 0xff;
#endif
		}
		srow += bpl;
		orow += rowstride;
	}
}

/*
  convert 16 bits/pixel data
  with alpha
  data in msb format
*/
static void
rgb565amsb (XImage *image, guchar *pixels, int rowstride, xlib_colormap *colormap)
{
	int xx, yy;
	int width, height;
	int bpl;

#ifdef LITTLE
	register guint8 *s;
#else
	register guint16 *s;	/* read 1 pixels at once */
#endif
	register guint32 *o;

	guint8 *srow = image->data, *orow = pixels;

	width = image->width;
	height = image->height;
	bpl = image->bytes_per_line;

	for (yy = 0; yy < height; yy++) {
		s = srow;
		o = (guint32 *) orow;
		for (xx = 0; xx < width; xx ++) {
			register guint32 data;
			/*  rrrrrggg gggbbbbb -> rrrrrRRR gggggg00 bbbbbBBB aaaaaaaa */
			/*  little endian: aaaaaaaa bbbbbBBB gggggg00 rrrrrRRR */
#ifdef LITTLE
			/* swap endianness first */
			data = s[0] | s[1] << 8;
			s += 2;
			*o++ = (data & 0xf800) >> 8 | (data & 0xe000) >> 13
				| (data & 0x7e0) << 5 | (data & 0x600) >> 1
				| (data & 0x1f) << 19 | (data & 0x1c) << 14
				| 0xff000000;
#else
			data = *s++;
			*o++ = (data & 0xf800) << 16 | (data & 0xe000) << 11
				| (data & 0x7e0) << 13 | (data & 0x600) << 7
				| (data & 0x1f) << 11 | (data & 0x1c) << 6
				| 0xff;
#endif
		}
		srow += bpl;
		orow += rowstride;
	}
}

/*
  convert 15 bits/pixel data
  no alpha
  data in lsb format
*/
static void
rgb555lsb (XImage *image, guchar *pixels, int rowstride, xlib_colormap *colormap)
{
	int xx, yy;
	int width, height;
	int bpl;

#ifdef LITTLE
	register guint32 *s;	/* read 2 pixels at once */
#else
	register guint8 *s;	/* read 2 pixels at once */
#endif
	register guint16 *o;
	guint8 *srow = image->data, *orow = pixels;

	width = image->width;
	height = image->height;
	bpl = image->bytes_per_line;

	for (yy = 0; yy < height; yy++) {
#ifdef LITTLE
		s = (guint32 *) srow;
#else
		s = srow;
#endif
		o = (guint16 *) orow;
		for (xx = 1; xx < width; xx += 2) {
			register guint32 data;
#ifdef LITTLE
			data = *s++;
			*o++ = (data & 0x7c00) >> 7 | (data & 0x7000) >> 12
				| (data & 0x3e0) << 6 | (data & 0x380) << 1;
			*o++ = (data & 0x1f) << 3 | (data & 0x1c) >> 2
				| (data & 0x7c000000) >> 15 | (data & 0x70000000) >> 20;
			*o++ = (data & 0x3e00000) >> 18 | (data & 0x3800000) >> 23
				| (data & 0x1f0000) >> 5 | (data & 0x1c0000) >> 10;
#else
			/* swap endianness first */
			data = s[0] | s[1] << 8 | s[2] << 16 | s[3] << 24;
			s += 4;
			*o++ = (data & 0x7c00) << 1 | (data & 0x7000) >> 4
				| (data & 0x3e0) >> 2 | (data & 0x380) >> 7;
			*o++ = (data & 0x1f) << 11 | (data & 0x1c) << 6
				| (data & 0x7c000000) >> 23 | (data & 0x70000000) >> 28;
			*o++ = (data & 0x3e00000) >> 10 | (data & 0x3800000) >> 15
				| (data & 0x1f0000) >> 13 | (data & 0x1c0000) >> 18;
#endif
		}
		/* check for last remaining pixel */
		if (width & 1) {
			register guint16 data;
#ifdef LITTLE
			data = *((short *) s);
#else
			data = *((short *) s);
			data = ((data >> 8) & 0xff) | ((data & 0xff) << 8);
#endif
			((char *) o)[0] = (data & 0x7c00) >> 7 | (data & 0x7000) >> 12;
			((char *) o)[1] = (data & 0x3e0) >> 2 | (data & 0x380) >> 7;
			((char *) o)[2] = (data & 0x1f) << 3 | (data & 0x1c) >> 2;
		}
		srow += bpl;
		orow += rowstride;
	}
}

/*
  convert 15 bits/pixel data
  no alpha
  data in msb format
*/
static void
rgb555msb (XImage *image, guchar *pixels, int rowstride, xlib_colormap *colormap)
{
	int xx, yy;
	int width, height;
	int bpl;

#ifdef LITTLE
	register guint8 *s;	/* read 2 pixels at once */
#else
	register guint32 *s;	/* read 2 pixels at once */
#endif
	register guint16 *o;
	guint8 *srow = image->data, *orow = pixels;

	width = image->width;
	height = image->height;
	bpl = image->bytes_per_line;

	for (yy = 0; yy < height; yy++) {
		s = srow;
		o = (guint16 *) orow;
		for (xx = 1; xx < width; xx += 2) {
			register guint32 data;
#ifdef LITTLE
			/* swap endianness first */
			data = s[0] | s[1] << 8 | s[2] << 16 | s[3] << 24;
			s += 4;
			*o++ = (data & 0x7c00) >> 7 | (data & 0x7000) >> 12
				| (data & 0x3e0) << 6 | (data & 0x380) << 1;
			*o++ = (data & 0x1f) << 3 | (data & 0x1c) >> 2
				| (data & 0x7c000000) >> 15 | (data & 0x70000000) >> 20;
			*o++ = (data & 0x3e00000) >> 18 | (data & 0x3800000) >> 23
				| (data & 0x1f0000) >> 5 | (data & 0x1c0000) >> 10;
#else
			data = *s++;
			*o++ = (data & 0x7c00) << 1 | (data & 0x7000) >> 4
				| (data & 0x3e0) >> 2 | (data & 0x380) >> 7;
			*o++ = (data & 0x1f) << 11 | (data & 0x1c) << 6
				| (data & 0x7c000000) >> 23 | (data & 0x70000000) >> 28;
			*o++ = (data & 0x3e00000) >> 10 | (data & 0x3800000) >> 15
				| (data & 0x1f0000) >> 13 | (data & 0x1c0000) >> 18;
#endif
		}
		/* check for last remaining pixel */
		if (width & 1) {
			register guint16 data;
#ifdef LITTLE
			data = *((short *) s);
			data = ((data >> 8) & 0xff) | ((data & 0xff) << 8);
#else
			data = *((short *) s);
#endif
			((char *) o)[0] = (data & 0x7c00) >> 7 | (data & 0x7000) >> 12;
			((char *) o)[1] = (data & 0x3e0) >> 2 | (data & 0x380) >> 7;
			((char *) o)[2] = (data & 0x1f) << 3 | (data & 0x1c) >> 2;
		}
		srow += bpl;
		orow += rowstride;
	}
}

/*
  convert 15 bits/pixel data
  with alpha
  data in lsb format
*/
static void
rgb555alsb (XImage *image, guchar *pixels, int rowstride, xlib_colormap *colormap)
{
	int xx, yy;
	int width, height;
	int bpl;

#ifdef LITTLE
	register guint16 *s;	/* read 1 pixels at once */
#else
	register guint8 *s;
#endif
	register guint32 *o;

	guint8 *srow = image->data, *orow = pixels;

	width = image->width;
	height = image->height;
	bpl = image->bytes_per_line;

	for (yy = 0; yy < height; yy++) {
#ifdef LITTLE
		s = (guint16 *) srow;
#else
		s = srow;
#endif
		o = (guint32 *) orow;
		for (xx = 0; xx < width; xx++) {
			register guint32 data;
			/*  rrrrrggg gggbbbbb -> rrrrrRRR gggggGGG bbbbbBBB aaaaaaaa */
			/*  little endian: aaaaaaaa bbbbbBBB gggggGGG rrrrrRRR */
#ifdef LITTLE
			data = *s++;
			*o++ = (data & 0x7c00) >> 7 | (data & 0x7000) >> 12
				| (data & 0x3e0) << 6 | (data & 0x380) << 1
				| (data & 0x1f) << 19 | (data & 0x1c) << 14
				| 0xff000000;
#else
			/* swap endianness first */
			data = s[0] | s[1] << 8;
			s += 2;
			*o++ = (data & 0x7c00) << 17 | (data & 0x7000) << 12
				| (data & 0x3e0) << 14 | (data & 0x380) << 9
				| (data & 0x1f) << 11 | (data & 0x1c) << 6
				| 0xff;
#endif
		}
		srow += bpl;
		orow += rowstride;
	}
}

/*
  convert 15 bits/pixel data
  with alpha
  data in msb format
*/
static void
rgb555amsb (XImage *image, guchar *pixels, int rowstride, xlib_colormap *colormap)
{
	int xx, yy;
	int width, height;
	int bpl;

#ifdef LITTLE
	register guint16 *s;	/* read 1 pixels at once */
#else
	register guint8 *s;
#endif
	register guint32 *o;

	guint8 *srow = image->data, *orow = pixels;

	width = image->width;
	height = image->height;
	bpl = image->bytes_per_line;

	for (yy = 0; yy < height; yy++) {
#ifdef LITTLE
		s = (guint16 *) srow;
#else
		s = srow;
#endif
		o = (guint32 *) orow;
		for (xx = 0; xx < width; xx++) {
			register guint32 data;
			/*  rrrrrggg gggbbbbb -> rrrrrRRR gggggGGG bbbbbBBB aaaaaaaa */
			/*  little endian: aaaaaaaa bbbbbBBB gggggGGG rrrrrRRR */
#ifdef LITTLE
			/* swap endianness first */
			data = s[0] | s[1] << 8;
			s += 2;
			*o++ = (data & 0x7c00) >> 7 | (data & 0x7000) >> 12
				| (data & 0x3e0) << 6 | (data & 0x380) << 1
				| (data & 0x1f) << 19 | (data & 0x1c) << 14
				| 0xff000000;
#else
			data = *s++;
			*o++ = (data & 0x7c00) << 17 | (data & 0x7000) << 12
				| (data & 0x3e0) << 14 | (data & 0x380) << 9
				| (data & 0x1f) << 11 | (data & 0x1c) << 6
				| 0xff;
#endif
		}
		srow += bpl;
		orow += rowstride;
	}
}


static void
rgb888alsb (XImage *image, guchar *pixels, int rowstride, xlib_colormap *colormap)
{
	int xx, yy;
	int width, height;
	int bpl;

	guint8 *s;	/* for byte order swapping */
	guint8 *o;
	guint8 *srow = image->data, *orow = pixels;

	width = image->width;
	height = image->height;
	bpl = image->bytes_per_line;

	/* lsb data */
	for (yy = 0; yy < height; yy++) {
		s = srow;
		o = orow;
		for (xx = 0; xx < width; xx++) {
			*o++ = s[2];
			*o++ = s[1];
			*o++ = s[0];
			*o++ = 0xff;
			s += 4;
		}
		srow += bpl;
		orow += rowstride;
	}
}

static void
rgb888lsb (XImage *image, guchar *pixels, int rowstride, xlib_colormap *colormap)
{
	int xx, yy;
	int width, height;
	int bpl;

	guint8 *srow = image->data, *orow = pixels;
	guint8 *o, *s;

	width = image->width;
	height = image->height;
	bpl = image->bytes_per_line;


	for (yy = 0; yy < height; yy++) {
		s = srow;
		o = orow;
		for (xx = 0; xx < width; xx++) {
			*o++ = s[2];
			*o++ = s[1];
			*o++ = s[0];
			s += 4;
		}
		srow += bpl;
		orow += rowstride;
	}
}

static void
rgb888amsb (XImage *image, guchar *pixels, int rowstride, xlib_colormap *colormap)
{
	int xx, yy;
	int width, height;
	int bpl;

	guint8 *srow = image->data, *orow = pixels;
#ifdef LITTLE
	guint32 *o;
	guint32 *s;
#else
	guint8 *s;	/* for byte order swapping */
	guint8 *o;
#endif


	width = image->width;
	height = image->height;
	bpl = image->bytes_per_line;

	/* msb data */
	for (yy = 0; yy < height; yy++) {
#ifdef LITTLE
		s = (guint32 *) srow;
		o = (guint32 *) orow;
#else
		s = srow;
		o = orow;
#endif
		for (xx = 0; xx < width; xx++) {
#ifdef LITTLE
			*o++ = s[1];
			*o++ = s[2];
			*o++ = s[3];
			*o++ = 0xff;
			s += 4;
#else
			*o++ = (*s << 8) | 0xff; /* untested */
			s++;
#endif
		}
		srow += bpl;
		orow += rowstride;
	}
}

static void
rgb888msb (XImage *image, guchar *pixels, int rowstride, xlib_colormap *colormap)
{
	int xx, yy;
	int width, height;
	int bpl;

	guint8 *srow = image->data, *orow = pixels;
	guint8 *s;
	guint8 *o;


	width = image->width;
	height = image->height;
	bpl = image->bytes_per_line;

	for (yy = 0; yy < height; yy++) {
		s = srow;
		o = orow;
		for (xx = 0; xx < width; xx++) {
			*o++ = s[1];
			*o++ = s[2];
			*o++ = s[3];
			s += 4;
		}
		srow += bpl;
		orow += rowstride;
	}
}
static void
visual_decompose_mask (gulong  mask,
		       gint   *shift,
		       gint   *prec)
{
	*shift = 0;
	*prec = 0;

	while (!(mask & 0x1)) {
		(*shift)++;
		mask >>= 1;
	}

	while (mask & 0x1) {
		(*prec)++;
		mask >>= 1;
	}
}


/*
  This should work correctly with any display/any endianness, but will probably
  run quite slow
*/
static void
convert_real_slow (XImage *image, guchar *pixels, int rowstride, xlib_colormap *cmap, int alpha)
{
	int xx, yy;
	int width, height;
	int bpl;
	guint8 *srow = image->data, *orow = pixels;
	guint8 *s;
	guint8 *o;
	guint32 pixel;
	Visual *v;
	guint8 component;
	int i;
	int red_shift, red_prec, green_shift, green_prec, blue_shift, blue_prec;

	width = image->width;
	height = image->height;
	bpl = image->bytes_per_line;
	v = cmap->visual;

	visual_decompose_mask (v->red_mask, &red_shift, &red_prec);
	visual_decompose_mask (v->green_mask, &green_shift, &green_prec);
	visual_decompose_mask (v->blue_mask, &blue_shift, &blue_prec);

	for (yy = 0; yy < height; yy++) {
		s = srow;
		o = orow;
		for (xx = 0; xx < width; xx++) {
			pixel = XGetPixel (image, xx, yy);
			switch (v->class) {
				/* I assume this is right for static & greyscale's too? */
			case StaticGray:
			case GrayScale:
			case StaticColor:
			case PseudoColor:
				*o++ = cmap->colors[pixel].red;
				*o++ = cmap->colors[pixel].green;
				*o++ = cmap->colors[pixel].blue;
				break;
			case TrueColor:
				/* This is odd because it must sometimes shift left (otherwise
				   I'd just shift >> (*_shift - 8 + *_prec + <0-7>). This logic
				   should work for all bit sizes/shifts/etc. */
				component = 0;
				for (i = 24; i < 32; i += red_prec)
					component |= ((pixel & v->red_mask) << (32 - red_shift - red_prec)) >> i;
				*o++ = component;
				component = 0;
				for (i = 24; i < 32; i += green_prec)
					component |= ((pixel & v->green_mask) << (32 - green_shift - green_prec)) >> i;
				*o++ = component;
				component = 0;
				for (i = 24; i < 32; i += blue_prec)
					component |= ((pixel & v->blue_mask) << (32 - blue_shift - blue_prec)) >> i;
				*o++ = component;
				break;
			case DirectColor:
				*o++ = cmap->colors[((pixel & v->red_mask) << (32 - red_shift - red_prec)) >> 24].red;
				*o++ = cmap->colors[((pixel & v->green_mask) << (32 - green_shift - green_prec)) >> 24].green;
				*o++ = cmap->colors[((pixel & v->blue_mask) << (32 - blue_shift - blue_prec)) >> 24].blue;
				break;
			}
			if (alpha)
				*o++ = 0xff;
		}
		srow += bpl;
		orow += rowstride;
	}
}

static xlib_colormap *
xlib_get_colormap (Colormap id, Visual *visual, MagnifierData *mag_data)
{
	int i;
	xlib_colormap *xc = g_new (xlib_colormap, 1);

	xc->size = visual->map_entries;
	xc->colors = g_new (XColor, xc->size);
	xc->visual = visual;
	xc->colormap = id;

	for (i = 0; i < xc->size; i++) {
		xc->colors[i].pixel = i;
		xc->colors[i].flags = DoRed | DoGreen | DoBlue;
	}

	XQueryColors (mag_data->source_display, xc->colormap, xc->colors, xc->size);

	return xc;
}

typedef void (* cfunc) (XImage *image, guchar *pixels, int rowstride, xlib_colormap *cmap);
static cfunc convert_map[] = {
	rgb1,rgb1,rgb1a,rgb1a,
	rgb8,rgb8,rgb8a,rgb8a,
	rgb555lsb,rgb555msb,rgb555alsb,rgb555amsb,
	rgb565lsb,rgb565msb,rgb565alsb,rgb565amsb,
	rgb888lsb,rgb888msb,rgb888alsb,rgb888amsb
};

static void
rgbconvert (XImage *image, guchar *pixels, int rowstride, int alpha, xlib_colormap *cmap,
	    MagnifierData *mag_data)
{
  int index = (image->byte_order == MSBFirst) | (alpha != 0) << 1;
  int bank=5;		/* default fallback converter */
  Visual *v = cmap->visual;
  if (mag_data->fast_rgb_convert)
    {	  
	switch (v->class) {
				/* I assume this is right for static & greyscale's too? */
	case StaticGray:
	case GrayScale:
	case StaticColor:
	case PseudoColor:
		switch (image->bits_per_pixel) {
		case 1:
			bank = 0;
			break;
		case 8:
			bank = 1;
			break;
		}
		break;
	case TrueColor:
		switch (image->depth) {
		case 15:
			if (v->red_mask == 0x7c00 && v->green_mask == 0x3e0 && v->blue_mask == 0x1f
			    && image->bits_per_pixel == 16)
				bank = 2;
			break;
		case 16:
			if (v->red_mask == 0xf800 && v->green_mask == 0x7e0 && v->blue_mask == 0x1f
			    && image->bits_per_pixel == 16)
				bank = 3;
			break;
		case 24:
		case 32:
			if (v->red_mask == 0xff0000 && v->green_mask == 0xff00 && v->blue_mask == 0xff
			    && image->bits_per_pixel == 32)
				bank = 4;
			break;
		}
		break;
	case DirectColor:
		/* always use the slow version */
		break;
	}
    }

	if (bank==5) {
		convert_real_slow(image, pixels, rowstride, cmap, alpha);
	} else {
		index |= bank << 2;
		(* convert_map[index]) (image, pixels, rowstride, cmap);
	}
}

void get_root_image(Window src, GdkPixbuf *dest, int src_x, int src_y, MagnifierData *mag_data)  {
	XImage *image;
	int width = mag_data->mag_width/mag_data->factor_x;
	int height = mag_data->mag_height/mag_data->factor_y;

	/* Get SpiImage in ZPixmap format (packed bits). */
	image = XGetImage (mag_data->source_display, src, src_x, src_y,
			   width, height, AllPlanes, ZPixmap);
	g_assert(image != NULL);

	if(x_cmap == NULL){
	    XWindowAttributes wa;
	    XGetWindowAttributes (mag_data->source_display, src, &wa);
	    x_cmap = xlib_get_colormap (wa.colormap, wa.visual, mag_data);			  
	}
	/* we offset into the image data based on the position we are retrieving from */
	rgbconvert (image, gdk_pixbuf_get_pixels(dest),
		    gdk_pixbuf_get_rowstride(dest),
		    gdk_pixbuf_get_has_alpha(dest),
		    x_cmap,
		    mag_data);

	/* would like to use GDK routine, but since we don't have multi-head
	   yet, we have to use X */

	XDestroyImage (image);
}


