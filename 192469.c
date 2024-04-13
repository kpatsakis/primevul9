static int fsl_lpspi_pio_transfer(struct spi_controller *controller,
				  struct spi_transfer *t)
{
	struct fsl_lpspi_data *fsl_lpspi =
				spi_controller_get_devdata(controller);
	int ret;

	fsl_lpspi->tx_buf = t->tx_buf;
	fsl_lpspi->rx_buf = t->rx_buf;
	fsl_lpspi->remain = t->len;

	reinit_completion(&fsl_lpspi->xfer_done);
	fsl_lpspi->slave_aborted = false;

	fsl_lpspi_write_tx_fifo(fsl_lpspi);

	ret = fsl_lpspi_wait_for_completion(controller);
	if (ret)
		return ret;

	fsl_lpspi_reset(fsl_lpspi);

	return 0;
}