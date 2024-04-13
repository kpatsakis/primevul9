static int spi_gpio_request(struct device *dev, struct spi_gpio *spi_gpio)
{
	spi_gpio->mosi = devm_gpiod_get_optional(dev, "mosi", GPIOD_OUT_LOW);
	if (IS_ERR(spi_gpio->mosi))
		return PTR_ERR(spi_gpio->mosi);

	spi_gpio->miso = devm_gpiod_get_optional(dev, "miso", GPIOD_IN);
	if (IS_ERR(spi_gpio->miso))
		return PTR_ERR(spi_gpio->miso);

	spi_gpio->sck = devm_gpiod_get(dev, "sck", GPIOD_OUT_LOW);
	return PTR_ERR_OR_ZERO(spi_gpio->sck);
}