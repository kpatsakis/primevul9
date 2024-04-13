static int ad5755_write_unlocked(struct iio_dev *indio_dev,
	unsigned int reg, unsigned int val)
{
	struct ad5755_state *st = iio_priv(indio_dev);

	st->data[0].d32 = cpu_to_be32((reg << 16) | val);

	return spi_write(st->spi, &st->data[0].d8[1], 3);
}