static void fsl_lpspi_read_rx_fifo(struct fsl_lpspi_data *fsl_lpspi)
{
	while (!(readl(fsl_lpspi->base + IMX7ULP_RSR) & RSR_RXEMPTY))
		fsl_lpspi->rx(fsl_lpspi);
}