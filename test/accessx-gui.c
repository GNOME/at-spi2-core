/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001 Sun Microsystems Inc.
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

#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <X11/XKBlib.h> /* AccessX control defs */
#include "spi.h"

#define N_CONTROLS 8

static XkbDescPtr xkb;

typedef struct {
	unsigned long  mask;
	short int     *val;
	char          *label;
	char          *val_label;
} AccessXControlStruct;

static void
gui_exit(GtkButton *notused, void *alsonotused)
{
  gtk_exit(0);
}

static void
toggle_xkb_control (GtkButton *toggle_button, AccessXControlStruct *control_struct)
{
  gboolean is_active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (toggle_button));
  XkbChangeEnabledControls (GDK_DISPLAY(), XkbUseCoreKbd, control_struct->mask,
			    is_active ? control_struct->mask : 0);
}

static void
toggle_xkb_notify_control (GtkButton *toggle_button, AccessXControlStruct *control_struct)
{
  gboolean is_active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (toggle_button));
  unsigned short mask, always_off_mask;
  XkbChangeEnabledControls (GDK_DISPLAY(), XkbUseCoreKbd, XkbAccessXFeedbackMask | XkbAudibleBellMask,
			    XkbAccessXFeedbackMask | XkbAudibleBellMask);
  
  mask = XkbAX_TwoKeysMask | XkbAX_LatchToLockMask | XkbAX_DumbBellFBMask;
  always_off_mask = XkbAX_SKPressFBMask | XkbAX_SKReleaseFBMask;

  if (is_active)
	  xkb->ctrls->ax_options |= (control_struct->mask  |  mask);
  else
	  xkb->ctrls->ax_options &= (~control_struct->mask);

  xkb->ctrls->ax_options &= ~always_off_mask;
  
  XkbSetControls (GDK_DISPLAY(), XkbAccessXFeedbackMask, xkb);
}

static void
toggle_accessx (GtkButton *toggle_button, void *unused)
{
  gboolean is_active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (toggle_button));
  XkbChangeEnabledControls (GDK_DISPLAY(), XkbUseCoreKbd, XkbAllBooleanCtrlsMask,
    (is_active) ? xkb->ctrls->enabled_ctrls : 0);
/*if (is_active/) XkbSetControls (GDK_DISPLAY(), XkbAllBooleanCtrlsMask, xkb);*/
}

static void
set_accessx_control_value (GtkRange *range_control, AccessXControlStruct *control_struct)
{
  *(control_struct->val) = (short int) gtk_range_get_value (range_control);
  XkbSetControls (GDK_DISPLAY(), control_struct->mask, xkb);
}

static gboolean
init_xkb ()
{
  int opcode, xkb_event_code, error, result, major, minor;
  Status status;
  result = XkbQueryExtension (GDK_DISPLAY(), &opcode, &xkb_event_code, &error, &major, &minor);
  if (result != True) return False;
  
  xkb = XkbGetMap(GDK_DISPLAY(), 0, XkbUseCoreKbd);
  if ((int)xkb == BadAlloc || xkb == NULL)
  {
      return False; 
  }
  else
  {
      status = XkbGetControls(GDK_DISPLAY(), XkbAllControlsMask, xkb);
      if (status != Success)
      {
	 XkbFreeKeyboard(xkb, XkbAllControlsMask, True);
	 return False;
      }
      else
      {
	if (!xkb->ctrls)
	{
	  XkbFreeKeyboard(xkb, XkbAllControlsMask, True);
	  return False;
        }
      }
  }

  return True;      
}

static AccessXControlStruct *
control_struct_new (unsigned int mask,
		    short int *val_ptr,
		    char *label,
		    char *val_label)
{
  AccessXControlStruct *control = g_new0 (AccessXControlStruct, 1);
  control->mask = mask;
  control->val = val_ptr;
  control->label = label;
  control->val_label = val_label;
  return control;
}

static GtkWidget*
basic_toggles_box ()
{
  GtkWidget *hbox, *outer_vbox, *vbox, *label, *button, *range, *frame;
  GtkAdjustment *adjustment;
  AccessXControlStruct *control;
  
  outer_vbox = gtk_widget_new (GTK_TYPE_VBOX,
			 "GtkWidget::visible", TRUE,
			 NULL);
  
  hbox = gtk_widget_new (GTK_TYPE_HBOX,
			 "GtkWidget::parent", outer_vbox,
			 "GtkWidget::visible", TRUE,
			 NULL);

  button = g_object_connect (gtk_widget_new (GTK_TYPE_CHECK_BUTTON,
					     "GtkButton::label", "Enable AccessX",
					     "GtkWidget::parent", hbox,
					     "GtkWidget::visible", TRUE,
					     NULL),
			     "signal::clicked",
			     toggle_accessx, NULL,
			     NULL);
  
/* FeatureFBMask seems not to work :-(
   
  control = control_struct_new (XkbAX_FeatureFBMask, &xkb->ctrls->ax_options, NULL, NULL);

  button = g_object_connect (gtk_widget_new (GTK_TYPE_CHECK_BUTTON,
					     "GtkButton::label", "Notify Changes",
					     "GtkWidget::parent", hbox,
					     "GtkWidget::visible", TRUE,
					     NULL),
			     "signal::clicked",
			     toggle_xkb_notify_control, control,
			     NULL);
*/			     

  gtk_widget_new (GTK_TYPE_HSEPARATOR,
		  "GtkWidget::parent", outer_vbox,
		  "GtkWidget::visible", TRUE,
		  NULL);

  hbox = gtk_widget_new (GTK_TYPE_HBOX,
			 "GtkWidget::parent", outer_vbox,
			 "GtkWidget::visible", TRUE,
			 NULL);

  control = control_struct_new (XkbStickyKeysMask, &xkb->ctrls->enabled_ctrls, NULL, NULL);

  button = g_object_connect (gtk_widget_new (GTK_TYPE_CHECK_BUTTON,
					     "GtkButton::label", "Sticky Keys",
					     "GtkWidget::parent", hbox,
					     "GtkWidget::visible", TRUE,
					     NULL),
			     "signal::clicked",
			     toggle_xkb_control, control,
			     NULL);
  
  control = control_struct_new (XkbAX_StickyKeysFBMask, &xkb->ctrls->ax_options, NULL, NULL);

  button = g_object_connect (gtk_widget_new (GTK_TYPE_CHECK_BUTTON,
					     "GtkButton::label", "Beep when Latched",
					     "GtkWidget::parent", hbox,
					     "GtkWidget::visible", TRUE,
					     NULL),
			     "signal::clicked",
			     toggle_xkb_notify_control, control,
			     NULL);
  
  frame = gtk_widget_new (gtk_frame_get_type(),
			  "GtkFrame::label", "Slow Keys",
			  "GtkWidget::parent", outer_vbox,
			  "GtkWidget::visible", TRUE,
			  NULL);
    
  vbox = gtk_widget_new (GTK_TYPE_VBOX,
			 "GtkWidget::parent", frame,
			 "GtkWidget::visible", TRUE,
			 NULL);
  
  hbox = gtk_widget_new (GTK_TYPE_HBOX,
			 "GtkWidget::parent", vbox,
			 "GtkWidget::visible", TRUE,
			 NULL);
  
  control = control_struct_new (XkbSlowKeysMask, &xkb->ctrls->slow_keys_delay, NULL, NULL);

  button = g_object_connect (gtk_widget_new (GTK_TYPE_CHECK_BUTTON,
					     "GtkButton::label", "Enable Slow Keys",
					     "GtkWidget::parent", hbox,
					     "GtkWidget::visible", TRUE,
					     NULL),
			     "signal::clicked",
			     toggle_xkb_control, control,
			     NULL);
  
  adjustment = GTK_ADJUSTMENT (gtk_adjustment_new (1.0, 1.0, 2000.0, 1.0, 10.0, 1.0 ));
  
  range = g_object_connect (gtk_widget_new (GTK_TYPE_HSCALE,
					    "GtkWidget::parent", hbox,
					    "GtkWidget::visible", TRUE,
					    "GtkRange::update_policy", GTK_UPDATE_DELAYED,
					    "GtkScale::digits", 0,
					    "GtkScale::adjustment", adjustment,
					    NULL),
			    "signal::value_changed",
			    set_accessx_control_value, control,
			    NULL);
  
  gtk_widget_set_usize (GTK_WIDGET (range), 150, -1);
  
  label = gtk_widget_new (GTK_TYPE_LABEL,
			  "GtkLabel::label", "Delay (mS)",
			  "GtkWidget::parent", hbox,
			  "GtkWidget::visible", TRUE,
			  NULL);
  
  hbox = gtk_widget_new (GTK_TYPE_HBOX,
			 "GtkWidget::parent", vbox,
			 "GtkWidget::visible", TRUE,
			 NULL);

  control = control_struct_new (XkbAX_SKAcceptFBMask, &xkb->ctrls->ax_options, NULL, NULL);

  button = g_object_connect (gtk_widget_new (GTK_TYPE_CHECK_BUTTON,
					     "GtkButton::label", "Beep on Accept",
					     "GtkWidget::parent", hbox,
					     "GtkWidget::visible", TRUE,
					     NULL),
			     "signal::clicked",
			     toggle_xkb_notify_control, control,
			     NULL);
  
  control = control_struct_new (XkbAX_SKRejectFBMask, &xkb->ctrls->ax_options, NULL, NULL);

  button = g_object_connect (gtk_widget_new (GTK_TYPE_CHECK_BUTTON,
					     "GtkButton::label", "Beep on Reject",
					     "GtkWidget::parent", hbox,
					     "GtkWidget::visible", TRUE,
					     NULL),
			     "signal::clicked",
			     toggle_xkb_notify_control, control,
			     NULL);
  
  frame = gtk_widget_new (gtk_frame_get_type(),
			  "GtkFrame::label", "Bounce Keys",
			  "GtkWidget::parent", outer_vbox,
			  "GtkWidget::visible", TRUE,
			  NULL);
    
  vbox = gtk_widget_new (GTK_TYPE_VBOX,
			 "GtkWidget::parent", frame,
			 "GtkWidget::visible", TRUE,
			 NULL);
  
  hbox = gtk_widget_new (GTK_TYPE_HBOX,
			 "GtkWidget::parent", vbox,
			 "GtkWidget::visible", TRUE,
			 NULL);
  
  control = control_struct_new (XkbBounceKeysMask, &xkb->ctrls->debounce_delay, NULL, NULL);

  button = g_object_connect (gtk_widget_new (GTK_TYPE_CHECK_BUTTON,
					     "GtkButton::label", "Enable Key Debounce",
					     "GtkWidget::parent", hbox,
					     "GtkWidget::visible", TRUE,
					     NULL),
			     "signal::clicked",
			     toggle_xkb_control, control,
			     NULL);
  
  adjustment = GTK_ADJUSTMENT (gtk_adjustment_new (1.0, 1.0, 2000.0, 1.0, 10.0, 1.0 ));
  
  range = g_object_connect (gtk_widget_new (GTK_TYPE_HSCALE,
					    "GtkWidget::parent", hbox,
					    "GtkWidget::visible", TRUE,
					    "GtkRange::update_policy", GTK_UPDATE_DELAYED,
					    "GtkScale::digits", 0,
					    "GtkScale::adjustment", adjustment,
					    NULL),
			    "signal::value_changed",
			    set_accessx_control_value, control,
			    NULL);
  
  gtk_widget_set_usize (GTK_WIDGET (range), 150, -1);
  
  label = gtk_widget_new (GTK_TYPE_LABEL,
			  "GtkLabel::label", "Delay (mS)",
			  "GtkWidget::parent", hbox,
			  "GtkWidget::visible", TRUE,
			  NULL);
  
  hbox = gtk_widget_new (GTK_TYPE_HBOX,
			 "GtkWidget::parent", vbox,
			 "GtkWidget::visible", TRUE,
			 NULL);

  control = control_struct_new (XkbAX_BKRejectFBMask, &xkb->ctrls->ax_options, NULL, NULL);

  button = g_object_connect (gtk_widget_new (GTK_TYPE_CHECK_BUTTON,
					     "GtkButton::label", "Beep on Reject",
					     "GtkWidget::parent", hbox,
					     "GtkWidget::visible", TRUE,
					     NULL),
			     "signal::clicked",
			     toggle_xkb_notify_control, control,
			     NULL);
  
  return outer_vbox;
}

static GtkWidget*
repeat_keys_box ()
{
  GtkWidget *outer_hbox, *vbox, *label, *button, *range, *frame;
  GtkAdjustment *adjustment;
  AccessXControlStruct *control =
	  control_struct_new (XkbRepeatKeysMask, &xkb->ctrls->repeat_delay, NULL, NULL);
  
  frame = gtk_widget_new (gtk_frame_get_type(),
			  "GtkFrame::label","Repeat Keys",
			  "GtkWidget::visible", TRUE,
			  NULL);
  
  outer_hbox = gtk_widget_new (GTK_TYPE_HBOX,
			 "GtkWidget::visible", TRUE,
			  "GtkWidget::parent", frame,
			 NULL);

  button = g_object_connect (gtk_widget_new (GTK_TYPE_CHECK_BUTTON,
					     "GtkButton::label", "Enable RepeatKeys",
					     "GtkWidget::parent", outer_hbox,
					     "GtkWidget::visible", TRUE,
					     NULL),
			     "signal::clicked",
			     toggle_xkb_control, control,
			     NULL);
  
  gtk_box_set_child_packing (GTK_BOX (outer_hbox), button, FALSE, FALSE, 0, GTK_PACK_START);

 
  vbox = gtk_widget_new (GTK_TYPE_VBOX,
			 "GtkWidget::visible", TRUE,
			 "GtkWidget::parent", outer_hbox,
			 NULL);

  adjustment = GTK_ADJUSTMENT (gtk_adjustment_new (200.0, 200.0, 4000.0, 10.0, 100.0, 10.0 ));
  
  range = g_object_connect (gtk_widget_new (GTK_TYPE_HSCALE,
					    "GtkWidget::parent", vbox,
					    "GtkWidget::visible", TRUE,
					    "GtkRange::update_policy", GTK_UPDATE_DELAYED,
					    "GtkScale::digits", 0,
					    "GtkScale::adjustment", adjustment,
					    NULL),
			    "signal::value_changed",
			    set_accessx_control_value, control,
			    NULL);
  
  gtk_widget_set_usize (GTK_WIDGET (range), 150, -1);
  
  adjustment = GTK_ADJUSTMENT (gtk_adjustment_new (200.0, 40.0, 4000.0, 1.0, 10.0, 1.0 ));
  
  control = control_struct_new (XkbRepeatKeysMask, &xkb->ctrls->repeat_interval, NULL, NULL);
  
  range = g_object_connect (gtk_widget_new (GTK_TYPE_HSCALE,
					    "GtkWidget::parent", vbox,
					    "GtkWidget::visible", TRUE,
					    "GtkRange::update_policy", GTK_UPDATE_DELAYED,
					    "GtkScale::digits", 0,
					    "GtkScale::adjustment", adjustment,
					    NULL),
			    "signal::value_changed",
			    set_accessx_control_value, control,
			    NULL);
  
  gtk_widget_set_usize (GTK_WIDGET (range), 150, -1);

  vbox = gtk_widget_new (GTK_TYPE_VBOX,
			 "GtkWidget::visible", TRUE,
			 "GtkWidget::parent", outer_hbox,
			 NULL);
  
  label = gtk_widget_new (GTK_TYPE_LABEL,
			  "GtkLabel::label", "Delay (mS)",
			  "GtkWidget::parent", vbox,
			  "GtkWidget::visible", TRUE,
			  NULL);
  
  label = gtk_widget_new (GTK_TYPE_LABEL,
			  "GtkLabel::label", "Interval (mS)",
			  "GtkWidget::parent", vbox,
			  "GtkWidget::visible", TRUE,
			  NULL);
   
  return frame;
}

static GtkWidget*
mouse_keys_box ()
{
  GtkWidget *outer_hbox, *vbox, *label, *button, *range, *frame;
  GtkAdjustment *adjustment;
  AccessXControlStruct *control =
	  control_struct_new (XkbMouseKeysMask, &xkb->ctrls->mk_delay, NULL, NULL);
  
  frame = gtk_widget_new (gtk_frame_get_type(),
			  "GtkFrame::label","Mouse Keys",
			  "GtkWidget::visible", TRUE,
			  NULL);
  
  outer_hbox = gtk_widget_new (GTK_TYPE_HBOX,
			 "GtkWidget::visible", TRUE,
			  "GtkWidget::parent", frame,
			 NULL);

  button = g_object_connect (gtk_widget_new (GTK_TYPE_CHECK_BUTTON,
					     "GtkButton::label", "Enable MouseKeys",
					     "GtkWidget::parent", outer_hbox,
					     "GtkWidget::visible", TRUE,
					     NULL),
			     "signal::clicked",
			     toggle_xkb_control, control,
			     NULL);
  
  gtk_box_set_child_packing (GTK_BOX (outer_hbox), button, FALSE, FALSE, 0, GTK_PACK_START);
 
  vbox = gtk_widget_new (GTK_TYPE_VBOX,
			 "GtkWidget::visible", TRUE,
			 "GtkWidget::parent", outer_hbox,
			 NULL);

  adjustment = GTK_ADJUSTMENT (gtk_adjustment_new (200.0, 20.0, 4000.0, 10.0, 100.0, 10.0 ));
  
  control = control_struct_new (XkbRepeatKeysMask, &xkb->ctrls->mk_interval, NULL, NULL);
  
  range = g_object_connect (gtk_widget_new (GTK_TYPE_HSCALE,
					    "GtkWidget::parent", vbox,
					    "GtkWidget::visible", TRUE,
					    "GtkRange::update_policy", GTK_UPDATE_DELAYED,
					    "GtkScale::digits", 0,
					    "GtkScale::adjustment", adjustment,
					    NULL),
			    "signal::value_changed",
			    set_accessx_control_value, control,
			    NULL);
  
  gtk_widget_set_usize (GTK_WIDGET (range), 150, -1);
  
  adjustment = GTK_ADJUSTMENT (gtk_adjustment_new (800.0, 200.0, 4000.0, 1.0, 10.0, 1.0 ));
  
  control = control_struct_new (XkbRepeatKeysMask, &xkb->ctrls->mk_time_to_max, NULL, NULL);
  
  range = g_object_connect (gtk_widget_new (GTK_TYPE_HSCALE,
					    "GtkWidget::parent", vbox,
					    "GtkWidget::visible", TRUE,
					    "GtkRange::update_policy", GTK_UPDATE_DELAYED,
					    "GtkScale::digits", 0,
					    "GtkScale::adjustment", adjustment,
					    NULL),
			    "signal::value_changed",
			    set_accessx_control_value, control,
			    NULL);
  
  gtk_widget_set_usize (GTK_WIDGET (range), 150, -1);
  
  adjustment = GTK_ADJUSTMENT (gtk_adjustment_new (800.0, 200.0, 4000.0, 1.0, 10.0, 1.0 ));
  
  control = control_struct_new (XkbMouseKeysMask, &xkb->ctrls->mk_max_speed, NULL, NULL);
  
  range = g_object_connect (gtk_widget_new (GTK_TYPE_HSCALE,
					    "GtkWidget::parent", vbox,
					    "GtkWidget::visible", TRUE,
					    "GtkRange::update_policy", GTK_UPDATE_DELAYED,
					    "GtkScale::digits", 0,
					    "GtkScale::adjustment", adjustment,
					    NULL),
			    "signal::value_changed",
			    set_accessx_control_value, control,
			    NULL);
  
  gtk_widget_set_usize (GTK_WIDGET (range), 150, -1);
  
  adjustment = GTK_ADJUSTMENT (gtk_adjustment_new (800.0, 200.0, 4000.0, 1.0, 10.0, 1.0 ));
  
  control = control_struct_new (XkbMouseKeysMask, &xkb->ctrls->mk_curve, NULL, NULL);
  
  range = g_object_connect (gtk_widget_new (GTK_TYPE_HSCALE,
					    "GtkWidget::parent", vbox,
					    "GtkWidget::visible", TRUE,
					    "GtkRange::update_policy", GTK_UPDATE_DELAYED,
					    "GtkScale::digits", 0,
					    "GtkScale::adjustment", adjustment,
					    NULL),
			    "signal::value_changed",
			    set_accessx_control_value, control,
			    NULL);
  
  vbox = gtk_widget_new (gtk_vbox_get_type(),
			 "GtkWidget::name", "MouseKeys",
			 "GtkWidget::parent", outer_hbox,
			 "GtkWidget::visible", TRUE,
		         "GtkContainer::border_width", 5,
			 NULL);
  label = gtk_widget_new (GTK_TYPE_LABEL,
			  "GtkLabel::label", "Delay (mS)",
			  "GtkWidget::parent", vbox,
			  "GtkWidget::visible", TRUE,
			  NULL);
  
  label = gtk_widget_new (GTK_TYPE_LABEL,
			  "GtkLabel::label", "Accel. time (mS)",
			  "GtkWidget::parent", vbox,
			  "GtkWidget::visible", TRUE,
			  NULL);
  
  label = gtk_widget_new (GTK_TYPE_LABEL,
			  "GtkLabel::label", "Interval (mS)",
			  "GtkWidget::parent", vbox,
			  "GtkWidget::visible", TRUE,
			  NULL);
  
  label = gtk_widget_new (GTK_TYPE_LABEL,
			  "GtkLabel::label", "Max. Speed",
			  "GtkWidget::parent", vbox,
			  "GtkWidget::visible", TRUE,
			  NULL);
  
  label = gtk_widget_new (GTK_TYPE_LABEL,
			  "GtkLabel::label", "Curve",
			  "GtkWidget::parent", vbox,
			  "GtkWidget::visible", TRUE,
			  NULL);
  return frame;
}

static GtkWidget*
timeout_box ()
{
  GtkWidget *hbox, *label, *button, *range;
  GtkAdjustment *adjustment;
  AccessXControlStruct *control =
	  control_struct_new (XkbAccessXTimeoutMask, &xkb->ctrls->ax_timeout, NULL, NULL);
  
  hbox = gtk_widget_new (gtk_hbox_get_type(),
			  "GtkWidget::visible", TRUE,
			  NULL);
  
  button = g_object_connect (gtk_widget_new (GTK_TYPE_CHECK_BUTTON,
					     "GtkButton::label", "AccessX Timeout",
					     "GtkWidget::parent", hbox,
					     "GtkWidget::visible", TRUE,
					     NULL),
			     "signal::clicked",
			     toggle_xkb_control, control,
			     NULL);
  
  adjustment = GTK_ADJUSTMENT (gtk_adjustment_new (1.0, 1.0, 200.0, 1.0, 10.0, 1.0 ));
  
  range = g_object_connect (gtk_widget_new (GTK_TYPE_HSCALE,
					    "GtkWidget::parent", hbox,
					    "GtkWidget::visible", TRUE,
					    "GtkRange::update_policy", GTK_UPDATE_DELAYED,
					    "GtkScale::digits", 0,
					    "GtkScale::adjustment", adjustment,
					    NULL),
			    "signal::value_changed",
			    set_accessx_control_value, control,
			    NULL);
  
  gtk_widget_set_usize (GTK_WIDGET (range), 150, -1);
  
  label = gtk_widget_new (GTK_TYPE_LABEL,
			  "GtkLabel::label", "AccessX Timeout (Min.)",
			  "GtkWidget::parent", hbox,
			  "GtkWidget::visible", TRUE,
			  NULL);
  
  return hbox;
}

static void
create_gui ()
{
  GtkWidget *window, *button, *container, *hbox;
  
  window = g_object_connect (gtk_widget_new (gtk_window_get_type (),
					     "user_data", NULL,
					     "can_focus", FALSE,
					     "type", GTK_WINDOW_TOPLEVEL,
					     "window-position", GTK_WIN_POS_CENTER,
					     "title", "AccessX",
					     "allow_grow", TRUE,
					     "allow_shrink", TRUE,
					     "border_width", 10,
					     NULL),
			     "signal::destroy", gui_exit, NULL,
			     NULL);
  
  container = gtk_widget_new (GTK_TYPE_VBOX,
			      "GtkWidget::parent", window,
			      "GtkWidget::visible", TRUE,
			      NULL);

  /* create and add containers for: AccessX on/off,
     basic toggles,
     MouseKeys support, and timeout */

  gtk_container_add (GTK_CONTAINER (container), basic_toggles_box ());
  gtk_container_add (GTK_CONTAINER (container), repeat_keys_box ());
/*  gtk_container_add (GTK_CONTAINER (container), mouse_keys_box ());
  gtk_container_add (GTK_CONTAINER (container), timeout_box ());*/

  hbox = gtk_widget_new (gtk_hbox_get_type(),
			 "GtkWidget::parent", container,
			 "GtkWidget::visible", TRUE,
		         "GtkContainer::border_width", 5,
			 NULL);

  button = g_object_connect (gtk_widget_new (gtk_button_get_type (),
					     "GtkButton::label", "Quit",
					     "GtkWidget::parent", hbox,
					     "GtkWidget::visible", TRUE,
					     NULL),
			     "signal::clicked",
			     gui_exit, NULL,
			     NULL);

  gtk_widget_show (window);	
}

static void
create_gui_x ()
{
  GtkWidget *window, *button, *container, *hbox, *range;
  GtkAdjustment *adjustment;
  int i;
  static AccessXControlStruct* controls[N_CONTROLS];
  controls[0] = control_struct_new (XkbSlowKeysMask,   &xkb->ctrls->slow_keys_delay, "Slow Keys", "Delay, mS");
  controls[1] = control_struct_new (XkbStickyKeysMask, &xkb->ctrls->ax_options, "Sticky Keys", NULL);
  controls[2] = control_struct_new (XkbBounceKeysMask, &xkb->ctrls->debounce_delay, "Key Debounce", "Delay, mS");
  controls[3] = control_struct_new (XkbRepeatKeysMask, &xkb->ctrls->repeat_delay, "Repeat Keys", "Delay, mS");
  controls[4] = control_struct_new (XkbRepeatKeysMask, &xkb->ctrls->repeat_interval, NULL, "Interval, mS");
  controls[5] = control_struct_new (XkbMouseKeysMask,  &xkb->ctrls->mk_delay, "MouseKeys", "Delay, mS");
  controls[6] = control_struct_new (XkbMouseKeysAccelMask, &xkb->ctrls->mk_time_to_max, NULL, "Accel");
  controls[7] = control_struct_new (XkbAccessXTimeoutMask, &xkb->ctrls->ax_timeout, "AccessX timeout", "Sec");
  
  
  
  hbox = gtk_widget_new (gtk_hbox_get_type(),
			 "GtkWidget::parent", container,
			 "GtkWidget::visible", TRUE,
			 NULL);

  i = 0; /* FIXME: this looks bogus */
  button = g_object_connect (gtk_widget_new (gtk_check_button_get_type (),
					     "GtkButton::label", "Access X Timeout",
					     "GtkWidget::parent", hbox,
					     "GtkWidget::visible", TRUE,
					     NULL),
			     "signal::clicked",
			     toggle_xkb_control, controls[i],
			     NULL);

  adjustment = GTK_ADJUSTMENT (gtk_adjustment_new (1.0, 1.0, 200.0, 1.0, 10.0, 1.0 ));
	  
  range = g_object_connect (gtk_widget_new (gtk_hscale_get_type (),
					    "GtkWidget::parent", hbox,
					    "GtkWidget::visible", TRUE,
					    "GtkScale::adjustment", adjustment,
					    NULL),
			    "signal::value_changed",
			    set_accessx_control_value, controls[i],
			    NULL);
	  
  gtk_widget_set_usize (GTK_WIDGET (range), 150, -1);
  gtk_widget_new (gtk_label_get_type(),
		  "GtkWidget::parent", hbox,
		  "GtkLabel::label", controls[i]->val_label,
		  "GtkWidget::visible", TRUE,
		  NULL);

  hbox = gtk_widget_new (gtk_hbox_get_type(),
			 "GtkWidget::parent", container,
			 "GtkWidget::visible", TRUE,
		         "GtkContainer::border_width", 5,
			 NULL);

  button = g_object_connect (gtk_widget_new (gtk_button_get_type (),
					     "GtkButton::label", "Quit",
					     "GtkWidget::parent", hbox,
					     "GtkWidget::visible", TRUE,
					     NULL),
			     "signal::clicked",
			     gui_exit, NULL,
			     NULL);

  gtk_widget_show (window);
}

int
main(int argc, char **argv)
{
  
  if ((argc > 1) && (!strncmp(argv[1],"-h",2)))
  {
    printf ("Usage: keysynth-demo\n");
    exit(0);
  }

  gtk_init (&argc, &argv); /* must call, because this program uses GTK+ */

  if (!init_xkb ())
  {
     g_error ("Xkb extension not present, can't initialize accessX\n");
  }
  
  create_gui ();
  
  gtk_main ();  

  return 0;
}
