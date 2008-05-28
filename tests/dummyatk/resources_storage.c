#include <atk/atk.h>

GHashTable* resources = NULL;

void resource_storage_init()
{
    if(resources == NULL)
    resources = g_hash_table_new_full(g_str_hash, g_str_equal,
        (GDestroyNotify)g_free, (GDestroyNotify)g_object_unref);
}

void resource_storage_free()
{
    if(resources == NULL) return;
    g_hash_table_destroy(resources);
    resources = NULL;
}

void resource_storage_add(const gchar* name, AtkObject* resource)
{
    if(resources == NULL) return;
    g_hash_table_insert(resources, g_strdup(name), g_object_ref(resource));
}

AtkObject* resource_storage_get(const gchar* name)
{
    if(resources == NULL) return NULL;
    return g_hash_table_lookup(resources, name);
}
void resources_storage_remove(const gchar* name)
{
    if(resources == NULL) return;
    g_hash_table_remove(resources, name);
}
