#ifndef _SPI_KEYMASKS_H_
#define _SPI_KEYMASKS_H_

#include <X11/Xlib.h>
#include <glib/gmacros.h>

G_BEGIN_DECLS

typedef unsigned long SpiKeyMaskType;

#define SPI_KEYMASK_ALT       Mod1Mask
#define SPI_KEYMASK_MOD1      Mod1Mask
#define SPI_KEYMASK_MOD2      Mod2Mask
#define SPI_KEYMASK_MOD3      Mod3Mask
#define SPI_KEYMASK_CONTROL   ControlMask
#define SPI_KEYMASK_SHIFT     ShiftMask
#define SPI_KEYMASK_SHIFTLOCK LockMask
#define SPI_KEYMASK_UNMODIFIED 0

G_END_DECLS

#endif
