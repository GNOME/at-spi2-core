#include <stdlib.h>
#include <libbonobo.h>
#include "registry.h"

int
main (int argc,
      char **argv)
{
        Registry *registry;
        char *obj_id;

        if (!bonobo_init (&argc, argv))
          {
            g_error ("Could not initialize oaf / Bonobo");
          }

        obj_id = "OAFIID:Accessibility_Registry:proto0.1";

        registry = registry_new ();

        oaf_active_server_register (
                obj_id,
                bonobo_object_corba_objref (bonobo_object (registry)));

        fprintf (stderr, "Registry Message: Registry daemon is running.\n");
        bonobo_main ();

        return 0;
}



