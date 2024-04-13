static void fsl_lpspi_set_cmd(struct fsl_lpspi_data *fsl_lpspi)
{
	u32 temp = 0;

	temp |= fsl_lpspi->config.bpw - 1;
	temp |= (fsl_lpspi->config.mode & 0x3) << 30;
	if (!fsl_lpspi->is_slave) {
		temp |= fsl_lpspi->config.prescale << 27;
		temp |= (fsl_lpspi->config.chip_select & 0x3) << 24;

		/*
		 * Set TCR_CONT will keep SS asserted after current transfer.
		 * For the first transfer, clear TCR_CONTC to assert SS.
		 * For subsequent transfer, set TCR_CONTC to keep SS asserted.
		 */
		if (!fsl_lpspi->usedma) {
			temp |= TCR_CONT;
			if (fsl_lpspi->is_first_byte)
				temp &= ~TCR_CONTC;
			else
				temp |= TCR_CONTC;
		}
	}
	writel(temp, fsl_lpspi->base + IMX7ULP_TCR);

	dev_dbg(fsl_lpspi->dev, "TCR=0x%x\n", temp);
}