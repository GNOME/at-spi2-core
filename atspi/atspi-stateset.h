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
  struct _AtspiAccessible *accessible;
  gint64 states;
};

typedef struct _AtspiStateSetClass AtspiStateSetClass;
struct _AtspiStateSetClass
{
  GObjectClass parent_class;
};

GType atspi_state_set_get_type (void);

AtspiStateSet * atspi_state_set_new (GArray *states);

void atspi_state_set_set_by_name (AtspiStateSet *set, const gchar *name, gboolean enabled);

void atspi_state_set_add (AtspiStateSet *set, AtspiStateType state);

AtspiStateSet * atspi_state_set_compare (AtspiStateSet *set, AtspiStateSet *set2);

gboolean atspi_state_set_contains (AtspiStateSet *set, AtspiStateType state);

gboolean atspi_state_set_equals (AtspiStateSet *set, AtspiStateSet *set2);

GArray * atspi_state_set_get_states (AtspiStateSet *set);

gboolean atspi_state_set_is_empty (AtspiStateSet *set);

void atspi_state_set_remove (AtspiStateSet *set, AtspiStateType state);

AtspiStateSet * _atspi_state_set_new_internal (struct _AtspiAccessible *accessible, gint64 states);

#endif	/* _ATSPI_STATE_SET_H_ */
