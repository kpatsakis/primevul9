static int fsl_lpspi_prepare_message(struct spi_controller *controller,
				     struct spi_message *msg)
{
	struct fsl_lpspi_data *fsl_lpspi =
					spi_controller_get_devdata(controller);
	struct spi_device *spi = msg->spi;
	int gpio = fsl_lpspi->chipselect[spi->chip_select];

	if (gpio_is_valid(gpio))
		gpio_direction_output(gpio, spi->mode & SPI_CS_HIGH ? 0 : 1);

	return 0;
}