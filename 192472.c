static void fsl_lpspi_dma_tx_callback(void *cookie)
{
	struct fsl_lpspi_data *fsl_lpspi = (struct fsl_lpspi_data *)cookie;

	complete(&fsl_lpspi->dma_tx_completion);
}