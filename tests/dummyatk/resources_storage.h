#ifndef RESOURCES_STORAGE_H
#define RESOURCES_STORAGE_H

#include <glib.h>

void resource_storage_init();

void resource_storage_free();

void resource_storage_add(const gchar* name, AtkObject* resource);

AtkObject* resource_storage_get(const gchar* name);

void resources_storage_remove(const gchar* name);

#endif /*RESOURCES_STORAGE_H*/
