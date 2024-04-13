static inline int getmiso(const struct spi_device *spi)
{
	struct spi_gpio *spi_gpio = spi_to_spi_gpio(spi);

	if (spi->mode & SPI_3WIRE)
		return !!gpiod_get_value_cansleep(spi_gpio->mosi);
	else
		return !!gpiod_get_value_cansleep(spi_gpio->miso);
}