#ifndef __SPI_LISTENER_IMPL_H__
#define __SPI_LISTENER_IMP_H__

#include <libspi/eventlistener.h>
#include <libspi/keystrokelistener.h>
#include <cspi/spi-impl.h>
#include <cspi/spi-listener.h>

G_BEGIN_DECLS

#define CSPI_EVENT_LISTENER_TYPE        (cspi_event_listener_get_type ())
#define CSPI_EVENT_LISTENER(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), CSPI_EVENT_LISTENER_TYPE, CSpiEventListener))
#define CSPI_EVENT_LISTENER_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), CSPI_EVENT_LISTENER_TYPE, CSpiEventListenerClass))
#define CSPI_IS_EVENT_LISTENER(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), CSPI_EVENT_LISTENER_TYPE))
#define CSPI_IS_EVENT_LISTENER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), CSPI_EVENT_LISTENER_TYPE))

struct _CSpiEventListener {
	SpiEventListener parent;
	GList           *callbacks;
};
typedef SpiEventListenerClass CSpiEventListenerClass;

GType cspi_event_listener_get_type (void);

#define CSPI_KEYSTROKE_LISTENER_TYPE        (cspi_keystroke_listener_get_type ())
#define CSPI_KEYSTROKE_LISTENER(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), CSPI_KEYSTROKE_LISTENER_TYPE, CSpiKeystrokeListener))
#define CSPI_KEYSTROKE_LISTENER_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), CSPI_KEYSTROKE_LISTENER_TYPE, CSpiKeystrokeListenerClass))
#define CSPI_IS_KEYSTROKE_LISTENER(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), CSPI_KEYSTROKE_LISTENER_TYPE))
#define CSPI_IS_KEYSTROKE_LISTENER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), CSPI_KEYSTROKE_LISTENER_TYPE))

struct _CSpiKeystrokeListener {
	SpiKeystrokeListener parent;
	GList               *callbacks;
};
typedef SpiKeystrokeListenerClass CSpiKeystrokeListenerClass;

GType cspi_keystroke_listener_get_type (void);

G_END_DECLS

#endif /* __SPI_LISTENER_IMPL_H__ */
