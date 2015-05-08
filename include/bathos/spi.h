/*
 * SPI generic interface
 * Alessandro Rubini, 2012 GNU GPL2 or later
 */
#ifndef __BATHOS_SPI_H__
#define __BATHOS_SPI_H__

#include <bathos/types.h>

/* This is an interface for SPI, only master is supported */
struct spi_cfg {
	int gpio_cs;
	int freq;		/* Suggested frequency */
	u8 pol, phase, devn;	/* devn selects spi0 or spi1 or more */
};

/* Data transfers include read and write, either or both */
struct spi_ibuf {
	int len;
	u8 *buf;
};

struct spi_obuf {
	int len;
	const u8 *buf;
};

/* Usually a data transfer is a single buffer, but we may need to chain more */
enum spi_flags {
    SPI_F_DEFAULT = 0,
    SPI_F_NOINIT = 1,	/* Don't lower CS (and don't take the lock) */
    SPI_F_NOFINI = 2,	/* Don't raise CS (and don't release the lock) */
    SPI_F_NOCS = 3,	/* Dont' act on CS at all (nor on the lock) */
};

/* This would be opaque if we had malloc, but we miss it by now */
struct spi_dev {
	const	struct spi_cfg *cfg;
	int	current_freq;
};


#ifdef CONFIG_HAS_SPI
#include <arch/spi.h>

extern struct spi_dev *spi_create(struct spi_dev *cfg);
extern void spi_destroy(struct spi_dev *dev);
extern int spi_xfer(struct spi_dev *dev,
		    enum spi_flags flags,
		    const struct spi_ibuf *ibuf,
		    const struct spi_obuf *obuf);

#else
/*
 * Empty SPI definitions, to allow libraries to be built
 * (then, clearly, we can't link with those functions)
 */

extern void __no_spi_code_for_this_architecture__(); /* link error */

static inline struct spi_dev *spi_create(struct spi_dev *cfg)
{
	__no_spi_code_for_this_architecture__();
	return NULL;
}

static inline void spi_destroy(struct spi_dev *dev)
{
	__no_spi_code_for_this_architecture__();
}

static inline int spi_xfer(struct spi_dev *dev,
			   enum spi_flags flags,
			   const struct spi_ibuf *ibuf,
			   const struct spi_obuf *obuf)
{
	__no_spi_code_for_this_architecture__();
	return 0;
}
#endif /* CONFIG_HAS_SPI */

/* Sometimes we want to only read or only write */
static inline int spi_read(struct spi_dev *dev,
			   enum spi_flags flags,
			   const struct spi_ibuf *ibuf)
{
	return spi_xfer(dev, flags, ibuf, NULL);
}

static inline int spi_write(struct spi_dev *dev, enum spi_flags flags,
			    const struct spi_obuf *obuf)
{
	return spi_xfer(dev, flags, NULL, obuf);
}

/* And sometimes we want raw-read  and write (with no buffer) */
static inline int spi_raw_read(struct spi_dev *dev, enum spi_flags flags,
			       int len, u8 *buf)
{
	struct spi_ibuf b = {len, buf};
	return spi_read(dev, flags, &b);
}

static inline int spi_raw_write(struct spi_dev *dev, enum spi_flags flags,
				int len, const u8 *buf)
{
	const struct spi_obuf b = {len, buf};
	return spi_write(dev, flags, &b);
}

#endif /* __BATHOS_SPI_H__ */
