static void fsl_lpspi_write_tx_fifo(struct fsl_lpspi_data *fsl_lpspi)
{
	u8 txfifo_cnt;
	u32 temp;

	txfifo_cnt = readl(fsl_lpspi->base + IMX7ULP_FSR) & 0xff;

	while (txfifo_cnt < fsl_lpspi->txfifosize) {
		if (!fsl_lpspi->remain)
			break;
		fsl_lpspi->tx(fsl_lpspi);
		txfifo_cnt++;
	}

	if (txfifo_cnt < fsl_lpspi->txfifosize) {
		if (!fsl_lpspi->is_slave) {
			temp = readl(fsl_lpspi->base + IMX7ULP_TCR);
			temp &= ~TCR_CONTC;
			writel(temp, fsl_lpspi->base + IMX7ULP_TCR);
		}

		fsl_lpspi_intctrl(fsl_lpspi, IER_FCIE);
	} else
		fsl_lpspi_intctrl(fsl_lpspi, IER_TDIE);
}