spi_to_spi_gpio(const struct spi_device *spi)
{
	const struct spi_bitbang	*bang;
	struct spi_gpio			*spi_gpio;

	bang = spi_master_get_devdata(spi->master);
	spi_gpio = container_of(bang, struct spi_gpio, bitbang);
	return spi_gpio;
}