#ifndef __SPI_PRIVATE_H__
#define __SPI_PRIVATE_H__

#include <cspi/spi.h>

G_BEGIN_DECLS

CORBA_Environment      *spi_ev         (void);
AccessibilityRegistry   spi_registry   (void);

boolean                 spi_exception  (void);

Accessible             *spi_object_add (Accessible corba_object);

int                     SPI_init (void);
void                    SPI_exit (void);

G_END_DECLS

#endif /* __SPI_PRIVATE_H__ */
