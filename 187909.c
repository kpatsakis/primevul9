static void spi_gpio_chipselect(struct spi_device *spi, int is_active)
{
	struct spi_gpio *spi_gpio = spi_to_spi_gpio(spi);

	/* set initial clock line level */
	if (is_active)
		gpiod_set_value_cansleep(spi_gpio->sck, spi->mode & SPI_CPOL);

	/* Drive chip select line, if we have one */
	if (spi_gpio->cs_gpios) {
		struct gpio_desc *cs = spi_gpio->cs_gpios[spi->chip_select];

		/* SPI chip selects are normally active-low */
		gpiod_set_value_cansleep(cs, (spi->mode & SPI_CS_HIGH) ? is_active : !is_active);
	}
}