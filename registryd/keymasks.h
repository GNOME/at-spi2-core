/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SPI_KEYMASKS_H_
#define SPI_KEYMASKS_H_

#include <glib.h>

G_BEGIN_DECLS

typedef unsigned long SpiKeyMaskType;

/* Values taken from Xlib.h */
#define SPI_KEYMASK_ALT (1 << 3)       /* Mod1Mask */
#define SPI_KEYMASK_MOD1 (1 << 3)      /* Mod1Mask */
#define SPI_KEYMASK_MOD2 (1 << 4)      /* Mod2Mask */
#define SPI_KEYMASK_MOD3 (1 << 5)      /* Mod3Mask */
#define SPI_KEYMASK_MOD4 (1 << 6)      /* Mod4Mask */
#define SPI_KEYMASK_MOD5 (1 << 7)      /* Mod5Mask */
#define SPI_KEYMASK_BUTTON1 (1L << 8)  /* Button1Mask */
#define SPI_KEYMASK_BUTTON2 (1L << 9)  /* Button2Mask */
#define SPI_KEYMASK_BUTTON3 (1L << 10) /* Button3Mask */
#define SPI_KEYMASK_BUTTON4 (1L << 11) /* Button4Mask */
#define SPI_KEYMASK_BUTTON5 (1L << 12) /* Button5Mask */
#define SPI_KEYMASK_CONTROL (1 << 2)   /* ControlMask */
#define SPI_KEYMASK_SHIFT (1 << 0)     /* ShiftMask */
#define SPI_KEYMASK_SHIFTLOCK (1 << 1) /* LockMask */
#define SPI_KEYMASK_NUMLOCK (1 << 14)
#define SPI_KEYMASK_UNMODIFIED 0

G_END_DECLS

#endif /* SPI_KEYMASKS_H_ */
