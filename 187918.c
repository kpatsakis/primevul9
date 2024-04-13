static u32 spi_gpio_txrx_word_mode3(struct spi_device *spi,
		unsigned nsecs, u32 word, u8 bits, unsigned flags)
{
	return bitbang_txrx_be_cpha1(spi, nsecs, 1, flags, word, bits);
}