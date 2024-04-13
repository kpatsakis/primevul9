static bool fsl_lpspi_can_dma(struct spi_controller *controller,
			      struct spi_device *spi,
			      struct spi_transfer *transfer)
{
	unsigned int bytes_per_word;

	if (!controller->dma_rx)
		return false;

	bytes_per_word = fsl_lpspi_bytes_per_word(transfer->bits_per_word);

	switch (bytes_per_word)
	{
		case 1:
		case 2:
		case 4:
			break;
		default:
			return false;
	}

	return true;
}