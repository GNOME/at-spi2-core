/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
 * Copyright 2008, 2009, 2010 Codethink Ltd.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "accessible-register.h"
#include "bridge.h"

/*
 * This module is responsible for keeping track of all the AtkObjects in
 * the application, so that they can be accessed remotely and placed in
 * a client side cache.
 *
 * To access an AtkObject remotely we need to provide a D-Bus object
 * path for it. The D-Bus object paths used have a standard prefix
 * (SPI_ATK_OBJECT_PATH_PREFIX). Appended to this prefix is a string
 * representation of an integer reference. So to access an AtkObject
 * remotely we keep a Hashtable that maps the given reference to
 * the AtkObject pointer. An object in this hash table is said to be 'registered'.
 *
 * The architecture of AT-SPI dbus is such that AtkObjects are not
 * remotely reference counted. This means that we need to keep track of
 * object destruction. When an object is destroyed it must be 'deregistered'
 * To do this lookup we keep a dbus-id attribute on each AtkObject.
 *
 */

#define SPI_ATK_PATH_PREFIX_LENGTH 27
#define SPI_ATK_OBJECT_PATH_PREFIX "/org/a11y/atspi/accessible/"
#define SPI_ATK_OBJECT_PATH_ROOT "root"

#define SPI_ATK_OBJECT_REFERENCE_TEMPLATE SPI_ATK_OBJECT_PATH_PREFIX "%d"

#define SPI_DBUS_ID "spi-dbus-id"

SpiRegister *spi_global_register = NULL;

static const gchar *spi_register_root_path = SPI_ATK_OBJECT_PATH_PREFIX SPI_ATK_OBJECT_PATH_ROOT;

enum
{
  OBJECT_REGISTERED,
  OBJECT_DEREGISTERED,
  LAST_SIGNAL
};
static guint register_signals[LAST_SIGNAL] = { 0 };

/*---------------------------------------------------------------------------*/

static void
spi_register_finalize (GObject *object);

/*---------------------------------------------------------------------------*/

G_DEFINE_TYPE (SpiRegister, spi_register, G_TYPE_OBJECT)

static void
spi_register_class_init (SpiRegisterClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;

  spi_register_parent_class = g_type_class_ref (G_TYPE_OBJECT);

  object_class->finalize = spi_register_finalize;

  register_signals[OBJECT_REGISTERED] =
      g_signal_new ("object-registered",
                    SPI_REGISTER_TYPE,
                    G_SIGNAL_ACTION,
                    0,
                    NULL,
                    NULL,
                    g_cclosure_marshal_VOID__OBJECT,
                    G_TYPE_NONE,
                    1,
                    G_TYPE_OBJECT);

  register_signals[OBJECT_DEREGISTERED] =
      g_signal_new ("object-deregistered",
                    SPI_REGISTER_TYPE,
                    G_SIGNAL_ACTION,
                    0,
                    NULL,
                    NULL,
                    g_cclosure_marshal_VOID__OBJECT,
                    G_TYPE_NONE,
                    1,
                    G_TYPE_OBJECT);
}

static void
spi_register_init (SpiRegister *reg)
{
  reg->ref2ptr = g_hash_table_new (g_direct_hash, g_direct_equal);
  reg->reference_counter = 0;
}

static void
deregister_object (gpointer data, GObject *gobj)
{
  SpiRegister *reg = SPI_REGISTER (data);

  spi_register_deregister_object (reg, gobj, FALSE);
}

static void
spi_register_remove_weak_ref (gpointer key, gpointer val, gpointer reg)
{
  g_object_weak_unref (val, deregister_object, reg);
}

static void
spi_register_finalize (GObject *object)
{
  SpiRegister *reg = SPI_REGISTER (object);

  g_hash_table_foreach (reg->ref2ptr, spi_register_remove_weak_ref, reg);
  g_hash_table_unref (reg->ref2ptr);

  G_OBJECT_CLASS (spi_register_parent_class)->finalize (object);
}

/*---------------------------------------------------------------------------*/

/*
 * Each AtkObject must be asssigned a D-Bus path (Reference)
 *
 * This function provides an integer reference for a new
 * AtkObject.
 *
 * TODO: Make this reference a little more unique, this is shoddy.
 */
static guint
assign_reference (SpiRegister *reg)
{
  reg->reference_counter++;
  /* Reference of 0 not allowed as used as direct key in hash table */
  if (reg->reference_counter == 0)
    reg->reference_counter++;
  return reg->reference_counter;
}

/*---------------------------------------------------------------------------*/

/*
 * Returns the reference of the object, or 0 if it is not registered.
 */
static guint
object_to_ref (GObject *gobj)
{
  return GPOINTER_TO_INT (g_object_get_data (gobj, SPI_DBUS_ID));
}

/*
 * Converts the Accessible object reference to its D-Bus object path
 */
static gchar *
ref_to_path (guint ref)
{
  return g_strdup_printf (SPI_ATK_OBJECT_REFERENCE_TEMPLATE, ref);
}

/*---------------------------------------------------------------------------*/

/*
 * Callback for when a registered AtkObject is destroyed.
 *
 * Removes the AtkObject from the reference lookup tables, meaning
 * it is no longer exposed over D-Bus.
 */
void
spi_register_deregister_object (SpiRegister *reg, GObject *gobj, gboolean unref)
{
  guint ref;

  ref = object_to_ref (gobj);
  if (ref != 0)
    {
      if (unref)
        {
          g_signal_emit (reg,
                         register_signals[OBJECT_DEREGISTERED],
                         0,
                         gobj);
          g_object_weak_unref (gobj, deregister_object, reg);
        }
      g_hash_table_remove (reg->ref2ptr, GINT_TO_POINTER (ref));

#ifdef SPI_ATK_DEBUG
      g_debug ("DEREG  - %d", ref);
#endif
    }
}

static void
register_object (SpiRegister *reg, GObject *gobj)
{
  guint ref;
  g_return_if_fail (G_IS_OBJECT (gobj));

  ref = assign_reference (reg);

  g_hash_table_insert (reg->ref2ptr, GINT_TO_POINTER (ref), gobj);
  g_object_set_data (G_OBJECT (gobj), SPI_DBUS_ID, GINT_TO_POINTER (ref));
  g_object_weak_ref (G_OBJECT (gobj), deregister_object, reg);

#ifdef SPI_ATK_DEBUG
  g_debug ("REG  - %d", ref);
#endif

  g_signal_emit (reg, register_signals[OBJECT_REGISTERED], 0, gobj);
}

/*---------------------------------------------------------------------------*/

/*
 * Used to lookup an GObject from its D-Bus path.
 *
 * If the D-Bus path is not found this function returns NULL.
 */
GObject *
spi_register_path_to_object (SpiRegister *reg, const char *path)
{
  guint index;
  void *data;

  g_return_val_if_fail (path, NULL);

  if (strncmp (path, SPI_ATK_OBJECT_PATH_PREFIX, SPI_ATK_PATH_PREFIX_LENGTH) != 0)
    return NULL;

  path += SPI_ATK_PATH_PREFIX_LENGTH; /* Skip over the prefix */

  /* Map the root path to the root object. */
  if (!g_strcmp0 (SPI_ATK_OBJECT_PATH_ROOT, path))
    return G_OBJECT (spi_global_app_data->root);

  index = atoi (path);
  data = g_hash_table_lookup (reg->ref2ptr, GINT_TO_POINTER (index));
  if (data)
    return G_OBJECT (data);
  else
    return NULL;
}

GObject *
spi_global_register_path_to_object (const char *path)
{
  return spi_register_path_to_object (spi_global_register, path);
}

/*
 * Used to lookup a D-Bus path from the GObject.
 *
 * If the objects is not already registered,
 * this function will register it.
 */
gchar *
spi_register_object_to_path (SpiRegister *reg, GObject *gobj)
{
  guint ref;

  if (gobj == NULL)
    return NULL;

  /* Map the root object to the root path. */
  if ((void *) gobj == (void *) spi_global_app_data->root)
    return g_strdup (spi_register_root_path);

  ref = object_to_ref (gobj);
  if (!ref)
    {
      register_object (reg, gobj);
      ref = object_to_ref (gobj);
    }

  if (!ref)
    return NULL;
  else
    return ref_to_path (ref);
}

guint
spi_register_object_to_ref (GObject *gobj)
{
  return object_to_ref (gobj);
}

/*
 * Gets the path that indicates the accessible desktop object.
 * This object is logically located on the registry daemon and not
 * within any particular application.
 */
gchar *
spi_register_root_object_path ()
{
  return g_strdup (SPI_ATK_OBJECT_PATH_PREFIX SPI_ATK_OBJECT_PATH_ROOT);
}

/*END------------------------------------------------------------------------*/
