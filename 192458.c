static irqreturn_t fsl_lpspi_isr(int irq, void *dev_id)
{
	u32 temp_SR, temp_IER;
	struct fsl_lpspi_data *fsl_lpspi = dev_id;

	temp_IER = readl(fsl_lpspi->base + IMX7ULP_IER);
	fsl_lpspi_intctrl(fsl_lpspi, 0);
	temp_SR = readl(fsl_lpspi->base + IMX7ULP_SR);

	fsl_lpspi_read_rx_fifo(fsl_lpspi);

	if ((temp_SR & SR_TDF) && (temp_IER & IER_TDIE)) {
		fsl_lpspi_write_tx_fifo(fsl_lpspi);
		return IRQ_HANDLED;
	}

	if (temp_SR & SR_MBF ||
	    readl(fsl_lpspi->base + IMX7ULP_FSR) & FSR_TXCOUNT) {
		writel(SR_FCF, fsl_lpspi->base + IMX7ULP_SR);
		fsl_lpspi_intctrl(fsl_lpspi, IER_FCIE);
		return IRQ_HANDLED;
	}

	if (temp_SR & SR_FCF && (temp_IER & IER_FCIE)) {
		writel(SR_FCF, fsl_lpspi->base + IMX7ULP_SR);
			complete(&fsl_lpspi->xfer_done);
		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}