static u32 spi_gpio_spec_txrx_word_mode1(struct spi_device *spi,
		unsigned nsecs, u32 word, u8 bits, unsigned flags)
{
	flags = spi->master->flags;
	return bitbang_txrx_be_cpha1(spi, nsecs, 0, flags, word, bits);
}