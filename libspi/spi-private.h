#ifndef SPI_PRIVATE_H_
#define SPI_PRIVATE_H_

#include <glib/glist.h>

G_BEGIN_DECLS

typedef enum {
	SPI_RE_ENTERANT_CONTINUE = 0,
	SPI_RE_ENTERANT_TERMINATE
} SpiReEnterantContinue;

typedef SpiReEnterantContinue (*SpiReEnterantFn) (GList * const *list,
						  gpointer       user_data);

void spi_re_enterant_list_delete_link (GList * const *element_ptr);
void spi_re_enterant_list_foreach     (GList         **list,
				       SpiReEnterantFn func,
				       gpointer        user_data);

G_END_DECLS

#endif /* SPI_PRIVATE_H_ */
