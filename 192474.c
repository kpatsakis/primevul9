static int fsl_lpspi_slave_abort(struct spi_controller *controller)
{
	struct fsl_lpspi_data *fsl_lpspi =
				spi_controller_get_devdata(controller);

	fsl_lpspi->slave_aborted = true;
	if (!fsl_lpspi->usedma)
		complete(&fsl_lpspi->xfer_done);
	else {
		complete(&fsl_lpspi->dma_tx_completion);
		complete(&fsl_lpspi->dma_rx_completion);
	}

	return 0;
}