#ifndef _ATSPI_STATE_SET_H_
#define _ATSPI_STATE_SET_H_

#define ATSPI_TYPE_STATE_SET                        (atspi_state_set_get_type ())
#define ATSPI_STATE_SET(obj)                        (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATSPI_TYPE_STATE_SET, AtspiStateSet))
#define ATSPI_STATE_SET_CLASS(klass)                (G_TYPE_CHECK_CLASS_CAST ((klass), ATSPI_TYPE_STATE_SET, AtspiStateSetClass))
#define ATSPI_IS_STATE_SET(obj)                     (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATSPI_TYPE_STATE_SET))
#define ATSPI_IS_STATE_SET_CLASS(klass)             (G_TYPE_CHECK_CLASS_TYPE ((klass), ATSPI_TYPE_STATE_SET))
#define ATSPI_STATE_SET_GET_CLASS(obj)              (G_TYPE_INSTANCE_GET_CLASS ((obj), ATSPI_TYPE_STATE_SET, AtspiStateSetClass))

typedef struct _AtspiStateSet AtspiStateSet;
struct _AtspiStateSet
{
  GObject parent;
  gint64 states;
};

typedef struct _AtspiStateSetClass AtspiStateSetClass;
struct _AtspiStateSetClass
{
  GObjectClass parent_class;
};

GType atspi_state_set_get_type (void);

AtspiStateSet *
atspi_state_set_new (gint64 states);

#endif	/* _ATSPI_STATE_SET_H_ */
