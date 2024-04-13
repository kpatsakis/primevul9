static int fsl_lpspi_config(struct fsl_lpspi_data *fsl_lpspi)
{
	u32 temp;
	int ret;

	if (!fsl_lpspi->is_slave) {
		ret = fsl_lpspi_set_bitrate(fsl_lpspi);
		if (ret)
			return ret;
	}

	fsl_lpspi_set_watermark(fsl_lpspi);

	if (!fsl_lpspi->is_slave)
		temp = CFGR1_MASTER;
	else
		temp = CFGR1_PINCFG;
	if (fsl_lpspi->config.mode & SPI_CS_HIGH)
		temp |= CFGR1_PCSPOL;
	writel(temp, fsl_lpspi->base + IMX7ULP_CFGR1);

	temp = readl(fsl_lpspi->base + IMX7ULP_CR);
	temp |= CR_RRF | CR_RTF | CR_MEN;
	writel(temp, fsl_lpspi->base + IMX7ULP_CR);

	temp = 0;
	if (fsl_lpspi->usedma)
		temp = DER_TDDE | DER_RDDE;
	writel(temp, fsl_lpspi->base + IMX7ULP_DER);

	return 0;
}