static int spi_gpio_setup(struct spi_device *spi)
{
	struct gpio_desc	*cs;
	int			status = 0;
	struct spi_gpio		*spi_gpio = spi_to_spi_gpio(spi);

	/*
	 * The CS GPIOs have already been
	 * initialized from the descriptor lookup.
	 */
	if (spi_gpio->cs_gpios) {
		cs = spi_gpio->cs_gpios[spi->chip_select];
		if (!spi->controller_state && cs)
			status = gpiod_direction_output(cs,
						  !(spi->mode & SPI_CS_HIGH));
	}

	if (!status)
		status = spi_bitbang_setup(spi);

	return status;
}