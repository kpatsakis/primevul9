static inline void setmosi(const struct spi_device *spi, int is_on)
{
	struct spi_gpio *spi_gpio = spi_to_spi_gpio(spi);

	gpiod_set_value_cansleep(spi_gpio->mosi, is_on);
}