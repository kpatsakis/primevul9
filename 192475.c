static int fsl_lpspi_dma_init(struct device *dev,
			      struct fsl_lpspi_data *fsl_lpspi,
			      struct spi_controller *controller)
{
	int ret;

	/* Prepare for TX DMA: */
	controller->dma_tx = dma_request_slave_channel_reason(dev, "tx");
	if (IS_ERR(controller->dma_tx)) {
		ret = PTR_ERR(controller->dma_tx);
		dev_dbg(dev, "can't get the TX DMA channel, error %d!\n", ret);
		controller->dma_tx = NULL;
		goto err;
	}

	/* Prepare for RX DMA: */
	controller->dma_rx = dma_request_slave_channel_reason(dev, "rx");
	if (IS_ERR(controller->dma_rx)) {
		ret = PTR_ERR(controller->dma_rx);
		dev_dbg(dev, "can't get the RX DMA channel, error %d\n", ret);
		controller->dma_rx = NULL;
		goto err;
	}

	init_completion(&fsl_lpspi->dma_rx_completion);
	init_completion(&fsl_lpspi->dma_tx_completion);
	controller->can_dma = fsl_lpspi_can_dma;
	controller->max_dma_len = FSL_LPSPI_MAX_EDMA_BYTES;

	return 0;
err:
	fsl_lpspi_dma_exit(controller);
	return ret;
}