/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#ifndef _SPI_KEYMASKS_H_
#define _SPI_KEYMASKS_H_

#include <X11/Xlib.h>
#include <glib.h>

G_BEGIN_DECLS

typedef unsigned long SpiKeyMaskType;

#define SPI_KEYMASK_ALT       Mod1Mask
#define SPI_KEYMASK_MOD1      Mod1Mask
#define SPI_KEYMASK_MOD2      Mod2Mask
#define SPI_KEYMASK_MOD3      Mod3Mask
#define SPI_KEYMASK_MOD4      Mod4Mask
#define SPI_KEYMASK_MOD5      Mod5Mask
#define SPI_KEYMASK_BUTTON1   Button1Mask
#define SPI_KEYMASK_BUTTON2   Button2Mask
#define SPI_KEYMASK_BUTTON3   Button3Mask
#define SPI_KEYMASK_BUTTON4   Button4Mask
#define SPI_KEYMASK_BUTTON5   Button5Mask
#define SPI_KEYMASK_CONTROL   ControlMask
#define SPI_KEYMASK_SHIFT     ShiftMask
#define SPI_KEYMASK_SHIFTLOCK LockMask
#define SPI_KEYMASK_NUMLOCK   (1<<14)
#define SPI_KEYMASK_UNMODIFIED 0

G_END_DECLS

#endif
