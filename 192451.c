static void fsl_lpspi_dma_exit(struct spi_controller *controller)
{
	if (controller->dma_rx) {
		dma_release_channel(controller->dma_rx);
		controller->dma_rx = NULL;
	}

	if (controller->dma_tx) {
		dma_release_channel(controller->dma_tx);
		controller->dma_tx = NULL;
	}
}