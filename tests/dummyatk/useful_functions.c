#include <string.h>
#include <glib.h>
/*
 * Functions and macros widely used in the tests.
 */
 
//same as strcmp() == 0 but works properly for NULL pointers
gboolean my_strcmp(const gchar* str1, const gchar* str2)
{
    if(str1 == str2) return TRUE;
    if(str1 == NULL || str2 == NULL) return FALSE;
    
    return strcmp(str1,str2) == 0;
}
//same as strlen but works properly for NULL pointer and returns gint instead of guint
gint my_strlen(const gchar* str)
{
    if(str == NULL)return 0;
    return (gint)strlen(str);
}
//same as strncmp() == 0 but works properly for NULL pointers
gboolean my_strncmp(const gchar* str1, const gchar* str2, gint n)
{
    if(n <= 0)return TRUE;
    if(str1 == str2)return TRUE;
    if(str1 == NULL || str2 == NULL)return FALSE;

    return strncmp(str1, str2, n) == 0;
}
