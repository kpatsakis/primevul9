static void spi_gpio_cleanup(struct spi_device *spi)
{
	spi_bitbang_cleanup(spi);
}