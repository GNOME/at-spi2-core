#ifndef SPI_PRIVATE_H_
#define SPI_PRIVATE_H_

#include <glib/glist.h>

G_BEGIN_DECLS

typedef enum {
	SPI_RE_ENTRANT_CONTINUE = 0,
	SPI_RE_ENTRANT_TERMINATE
} SpiReEntrantContinue;

typedef SpiReEntrantContinue (*SpiReEntrantFn) (GList * const *list,
						gpointer       user_data);

void spi_re_entrant_list_delete_link (GList * const  *element_ptr);
void spi_re_entrant_list_foreach     (GList         **list,
				      SpiReEntrantFn  func,
				      gpointer        user_data);

G_END_DECLS

#endif /* SPI_PRIVATE_H_ */
