/* ATK -  Accessibility Toolkit
 * Copyright 2001 Sun Microsystems Inc.
 * Copyright 2014 Igalia S.L.
 *
 * Author: Alejandro Piñeiro Iglesias <apinheiro@igalia.com>
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
#include <atk/atk.h>

/**
 * SECTION:testvalue
 * @Short_description: this example serves as a unit test for AtkValue
 *  and also as an example of how to implement #AtkValue on a given
 *  GObject.
 *
 *  This test will represent a volume slider, smart enough to have
 *  classify the values on the global range [0,1] with the
 *  descriptions "low", "medium", "high" and "very high".  As the
 *  clasification is fixed, it also expose all the four possible
 *  subranges. To fill the description name it will use some of the
 *  already defined #AtkValueType.
 *
 *  This will implement all the methods of #AtkValue, but note that
 *  this is not mandatory on all the cases. In several cases it is not
 *  needed to implement the subranges methods. See #AtkValue
 *  documentation for further information.
 *
 */

#define EXPECTED_NUMBER 7

#define LOWER_LIMIT 0
#define LOW_THRESHOLD 0.2
#define NORMAL_THRESHOLD 0.4
#define HIGH_THRESHOLD 0.8
#define RISKY_THRESHOLD 1.0
#define UPPER_LIMIT 1.0
#define INCREMENT 0.15

GMainLoop *global_loop = NULL;
gint global_number_emissions = 0;
gboolean test_success = TRUE;
GObject *my_value;

#define TEST_TYPE_VALUE                         (test_value_get_type ())
#define TEST_VALUE(obj)                         (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEST_TYPE_VALUE, TestValue))
#define TEST_VALUE_CLASS(klass)                 (G_TYPE_CHECK_CLASS_CAST ((klass), TEST_TYPE_VALUE, TestValueClass))
#define TEST_IS_VALUE(obj)                      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TEST_TYPE_VALUE))
#define TEST_IS_VALUE_CLASS(klass)              (G_TYPE_CHECK_CLASS_TYPE ((klass), TEST_TYPE_VALUE))
#define TEST_VALUE_GET_CLASS(obj)               (G_TYPE_INSTANCE_GET_CLASS ((obj), TEST_TYPE_VALUE, TestValueClass))

typedef struct _TestValue        TestValue;
typedef struct _TestValueClass   TestValueClass;

struct _TestValue
{
  AtkObject parent;

  gdouble value;
};

struct _TestValueClass
{
  AtkObjectClass parent_class;
};

GType       test_value_get_type (void) G_GNUC_CONST;
static void test_value_interface_init (AtkValueIface *iface);

G_DEFINE_TYPE_WITH_CODE (TestValue,
                         test_value,
                         ATK_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (ATK_TYPE_VALUE,
                                                test_value_interface_init));

static void
test_value_class_init (TestValueClass *klass)
{
}

static void
test_value_init (TestValue *value)
{
}


static const gchar*
get_description (gdouble value)
{
  const gchar *description = NULL;

  if (value < LOW_THRESHOLD)
    description = atk_value_type_get_localized_name (ATK_VALUE_LOW);
  else if (value < NORMAL_THRESHOLD)
    description = atk_value_type_get_localized_name (ATK_VALUE_MEDIUM);
  else if (value < HIGH_THRESHOLD)
    description = atk_value_type_get_localized_name (ATK_VALUE_HIGH);
  else description = atk_value_type_get_localized_name (ATK_VALUE_VERY_HIGH);

  return description;
}

static void
test_value_get_value_and_text (AtkValue *value,
                               gdouble *current_value,
                               gchar **description)
{
  g_return_if_fail (TEST_IS_VALUE (value));

  TestValue *self = TEST_VALUE (value);

  if (current_value != NULL)
    *current_value = self->value;

  if (description != NULL)
    *description = g_strdup (get_description (self->value));
}

AtkRange*
test_value_get_range (AtkValue *value)
{
  g_return_val_if_fail (TEST_IS_VALUE (value), NULL);

  AtkRange *result = atk_range_new (LOWER_LIMIT,
                                    UPPER_LIMIT,
                                    NULL);

  return result;
}

gdouble
test_value_get_increment (AtkValue *value)
{
  g_return_val_if_fail (TEST_IS_VALUE (value), 0.0);

  return INCREMENT;
}

GSList*
test_value_get_sub_ranges (AtkValue *value)
{
  g_return_val_if_fail (TEST_IS_VALUE (value), NULL);

  GSList *result = NULL;
  AtkRange *range = NULL;

  /* low */
  range = atk_range_new (LOWER_LIMIT, LOW_THRESHOLD,
                         get_description (LOWER_LIMIT));

  result = g_slist_append (result, range);

  /* normal */
  range = atk_range_new (LOW_THRESHOLD, NORMAL_THRESHOLD,
                         get_description (LOW_THRESHOLD));
  result = g_slist_append (result, range);

  /* high */
  range = atk_range_new (NORMAL_THRESHOLD, HIGH_THRESHOLD,
                         get_description (NORMAL_THRESHOLD));
  result = g_slist_append (result, range);

  /* very high */
  range = atk_range_new (HIGH_THRESHOLD, UPPER_LIMIT,
                         get_description (HIGH_THRESHOLD));
  result = g_slist_append (result, range);

  return result;
}

void
test_value_set_value (AtkValue *value,
                      double new_value)
{
  g_return_if_fail (TEST_IS_VALUE (value));

  TestValue *self = TEST_VALUE (value);

  if (new_value < LOWER_LIMIT)
    new_value = LOWER_LIMIT;

  if (new_value > UPPER_LIMIT)
    new_value = UPPER_LIMIT;

  if (new_value != self->value) {
    gchar *description = g_strdup (get_description (new_value));
    self->value = new_value;
    g_signal_emit_by_name (value, "value-changed", new_value, description, NULL);
    g_free (description);
  }
}

static void
test_value_interface_init (AtkValueIface *iface)
{
  iface->get_value_and_text = test_value_get_value_and_text;
  iface->get_range = test_value_get_range;
  iface->get_increment = test_value_get_increment;
  iface->get_sub_ranges = test_value_get_sub_ranges;
  iface->set_value = test_value_set_value;
}

static void
value_page_changed_cb (AtkValue *value,
                       gdouble new_value,
                       gchar *new_description,
                       gpointer data)
{
  g_print ("value-changed callback=(%f,%s)\n", new_value, new_description);
  global_number_emissions++;
}

/**
 * This call simulates a user interacting with the slider.
 *
 */
static gboolean
do_value_changed (gpointer data)
{
  TestValue* test_value = TEST_VALUE (data);

  atk_value_set_value (ATK_VALUE (test_value),
                       test_value->value + INCREMENT);

  if (global_number_emissions == EXPECTED_NUMBER) {
    g_main_loop_quit (global_loop);
    return G_SOURCE_REMOVE;
  } else
    return G_SOURCE_CONTINUE;
}

/**
 * Prints all the info from an AtkValue
 */
static void
print_info (AtkValue *atk_value)
{
  double value;
  gchar *description;
  AtkRange *range;
  GSList *sub_ranges;
  GSList *iter;
  gdouble increment;
  gint i = 0;

  atk_value_get_value_and_text (atk_value, &value, &description);
  range = atk_value_get_range (atk_value);
  increment = atk_value_get_increment (atk_value);
  atk_value_set_value (atk_value, 0);

  g_print ("Current AtkValue data:\n");
  g_print ("\t (value,description)=(%f,%s) \n", value, description);
  if (range != NULL)
    g_print ("\t (min,max,description)=(%f, %f, %s)\n",
             atk_range_get_lower_limit (range), atk_range_get_upper_limit (range), atk_range_get_description (range));
  else
    test_success = FALSE; /* Any AtkValue implementation should provide a range */
  g_print ("\t minimum increment=%f\n", increment);

  if (range)
    atk_range_free (range);

  sub_ranges = atk_value_get_sub_ranges (atk_value);
  for (iter = sub_ranges; iter != NULL; iter = g_slist_next (iter),i++) {
    range = iter->data;
    g_print ("\t\t sub_range%i = (%f, %f, %s)\n", i,
             atk_range_get_lower_limit (range), atk_range_get_upper_limit (range), atk_range_get_description (range));
  }

  g_slist_free_full (sub_ranges, (GDestroyNotify) atk_range_free);
}


static gboolean
init_test_value (void)
{
  my_value = g_object_new (TEST_TYPE_VALUE, NULL);

  g_signal_connect (my_value, "value-changed",
                    G_CALLBACK (value_page_changed_cb),
                    NULL);

  print_info (ATK_VALUE (my_value));

  g_idle_add (do_value_changed, my_value);

  return TRUE;
}


int
main (gint  argc,
      char* argv[])
{
  global_loop = g_main_loop_new (NULL, FALSE);

  g_print("Starting Value test suite\n\n\n");

  init_test_value ();
  g_main_loop_run (global_loop);

  if (global_number_emissions == EXPECTED_NUMBER && test_success)
    g_print ("\n\nValue tests succeeded\n\n\n");
  else
    g_print ("\n\nValue tests failed\n\n\n");

  print_info (ATK_VALUE (my_value));

  return 0;
}
