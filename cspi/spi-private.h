#ifndef _SPI_PRIVATE_H_
#define _SPI_PRIVATE_H_

/*
 * Private internal implementation details of at-spi.
 */

#include <libbonobo.h>
#include <libspi/Accessibility.h>
#include <cspi/spi.h>
#include "cspi/spi-listener.h"

struct _Accessible {
	CORBA_Object objref;
	/* And some other bits */
	guint        ref_count;
};

#define CSPI_OBJREF(a) (((Accessible *)(a))->objref)

CORBA_Environment     *cspi_ev               (void);
SPIBoolean             cspi_exception        (void);
Accessibility_Registry cspi_registry         (void);
Accessible            *cspi_object_add       (CORBA_Object         corba_object);
Accessible            *cspi_object_add_check (CORBA_Object         corba_object);
void                   cspi_object_ref       (Accessible          *accessible);
void                   cspi_object_unref     (Accessible          *accessible);
SPIBoolean             cspi_accessible_is_a  (Accessible          *obj,
					      const char          *interface_name);
SPIBoolean             cspi_warn_ev          (CORBA_Environment   *ev,
					      const char          *error_string);
void                   cspi_check_ev         (CORBA_Environment   *ev,
					      const char          *error_string);

#endif /* _SPI_PRIVATE_H_ */
