static int fsl_lpspi_reset(struct fsl_lpspi_data *fsl_lpspi)
{
	u32 temp;

	if (!fsl_lpspi->usedma) {
		/* Disable all interrupt */
		fsl_lpspi_intctrl(fsl_lpspi, 0);
	}

	/* W1C for all flags in SR */
	temp = 0x3F << 8;
	writel(temp, fsl_lpspi->base + IMX7ULP_SR);

	/* Clear FIFO and disable module */
	temp = CR_RRF | CR_RTF;
	writel(temp, fsl_lpspi->base + IMX7ULP_CR);

	return 0;
}