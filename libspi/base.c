#include <libspi/base.h>

/* A pointer to our parent object class */
static GObjectClass *spi_base_parent_class;

static void
spi_base_dispose (GObject *object)
{
	SpiBase *base = SPI_BASE (object);

	if (base->atko) {
		g_object_unref (base->atko);
		base->atko = NULL;
	}

	spi_base_parent_class->dispose (object);
}

static void
spi_base_class_init (GObjectClass *klass)
{
	spi_base_parent_class = g_type_klass_peek_parent (klass);

	klass->dispose = spi_base_dispose;
}

static void
spi_base_init (SpiBase *base)
{
}

BONOBO_TYPE_FUNC (SpiBase, BONOBO_TYPE_OBJECT, spi_base);

gpointer
spi_base_construct (SpiBase   *base,
		    AtkObject *o)
{
	g_return_val_if_fail (ATK_IS_OBJECT (o), NULL);
	g_return_val_if_fail (SPI_IS_BASE (base), NULL);

	base->atko = g_object_ref (o);
}

AtkObject *
spi_base_get_atk_object (SpiBase *base)
{
	g_return_val_if_fail (SPI_IS_BASE (base), NULL);

	return base->atko;
}
