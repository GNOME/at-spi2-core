#ifndef USEFUL_FUNCTIONS_H
#define USEFUL_FUNCTIONS_H
/*
 * Functions and macros widely used in the tests.
 */
 
//macro for creating objects in startup section
#define OBJECT_NEW(obj, type, type_str) obj = g_object_new(type,NULL);\
    if(obj == NULL)\
    {\
        INIT_FAILED("Cannot create instance of type" type_str ".\n");\
    }
//macro for destroying object 
#define OBJECT_UNREF(obj) if(obj != NULL)\
    {\
        g_object_unref((gpointer)obj);\
    }
//for testing signals
#define HANDLER_DISCONNECT(obj, h) if((h) != 0)\
	{\
		g_signal_handler_disconnect(obj, h);\
	}

gboolean my_strcmp(const gchar* str1, const gchar* str2);

gint my_strlen(const gchar* str);

gboolean my_strncmp(const gchar* str1, const gchar* str2, gint n);

#endif /*USEFUL_FUNCTIONS_H*/
