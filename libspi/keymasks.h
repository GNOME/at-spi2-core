#ifndef _SPI_KEYMASKS_H_
#define _SPI_KEYMASKS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <X11/Xlib.h>

typedef unsigned long KeyMaskType;

#define KEYMASK_ALT       Mod1Mask
#define KEYMASK_MOD1      Mod1Mask
#define KEYMASK_MOD2      Mod2Mask
#define KEYMASK_MOD3      Mod3Mask
#define KEYMASK_CONTROL   ControlMask
#define KEYMASK_SHIFT     ShiftMask
#define KEYMASK_SHIFTLOCK LockMask
#define KEYMASK_UNMODIFIED 0

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
