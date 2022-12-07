#ifndef _DEVICEEVENTCONTROLLER_X11_H_
#define _DEVICEEVENTCONTROLLER_X11_H_

#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <glib.h>

typedef struct
{
  Display *xevie_display;
  unsigned int last_press_keycode;
  unsigned int last_release_keycode;
  struct timeval last_press_time;
  struct timeval last_release_time;
  int have_xkb;
  int xkb_major_extension_opcode;
  int xkb_base_event_code;
  int xkb_base_error_code;
  unsigned int xkb_latch_mask;
  unsigned int pending_xkb_mod_relatch_mask;
  XkbDescPtr xkb_desc;
  KeyCode reserved_keycode;
  KeySym reserved_keysym;
  guint reserved_reset_timeout;
} SpiDEControllerPrivate;

#ifdef HAVE_X11
void spi_dec_setup_x11 (SpiDEControllerClass *klass);
#endif

long ucs2keysym (long ucs);
long keysym2ucs (long keysym);

#endif /* _DEVICEEVENTCONTROLLER_X11_H_ */
