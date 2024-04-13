static int fsl_lpspi_transfer_one(struct spi_controller *controller,
				  struct spi_device *spi,
				  struct spi_transfer *t)
{
	struct fsl_lpspi_data *fsl_lpspi =
					spi_controller_get_devdata(controller);
	int ret;

	fsl_lpspi->is_first_byte = true;
	ret = fsl_lpspi_setup_transfer(controller, spi, t);
	if (ret < 0)
		return ret;

	fsl_lpspi_set_cmd(fsl_lpspi);
	fsl_lpspi->is_first_byte = false;

	if (fsl_lpspi->usedma)
		ret = fsl_lpspi_dma_transfer(controller, fsl_lpspi, t);
	else
		ret = fsl_lpspi_pio_transfer(controller, t);
	if (ret < 0)
		return ret;

	return 0;
}