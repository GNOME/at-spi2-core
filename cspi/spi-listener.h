#ifndef _SPI_LISTENER_H_
#define _SPI_LISTENER_H_

#include <cspi/spi-impl.h>

#ifdef  __cplusplus
extern "C" {
#endif

/*
 * Structure used to encapsulate event information
 */
typedef struct {
  const char  *type;
  Accessible  *source;
  long         detail1;
  long         detail2;
} AccessibleEvent;

typedef enum {
  SPI_KEY_PRESSED  = 1<<0,
  SPI_KEY_RELEASED = 1<<1
} AccessibleKeyEventType;


typedef struct {
  long                   keyID;
  short                  keycode;
  char *                 keystring;
  long                   timestamp;
  AccessibleKeyEventType type;
  unsigned short         modifiers;
  SPIBoolean             is_text;	
} AccessibleKeystroke;

/*
 * Function prototype typedefs for Event Listener Callbacks.
 * (see libspi/accessibleeventlistener.h for definition of SpiVoidEventListenerCB).
 *
 * usage: signatures should be
 * void (*AccessibleEventListenerCB) (AccessibleEvent *event);
 *
 * SPIBoolean (*AccessibleKeystrokeListenerCB) (AccessibleKeystrokeEvent *Event);
 */
typedef void       (*AccessibleEventListenerCB)     (AccessibleEvent     *event,
						     void                *user_data);
typedef SPIBoolean (*AccessibleKeystrokeListenerCB) (AccessibleKeystroke *stroke,
						     void                *user_data);

#ifdef  __cplusplus
}
#endif

#endif
