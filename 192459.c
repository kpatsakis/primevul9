static void fsl_lpspi_set_watermark(struct fsl_lpspi_data *fsl_lpspi)
{
	u32 temp;

	if (!fsl_lpspi->usedma)
		temp = fsl_lpspi->watermark >> 1 |
		       (fsl_lpspi->watermark >> 1) << 16;
	else
		temp = fsl_lpspi->watermark >> 1;

	writel(temp, fsl_lpspi->base + IMX7ULP_FCR);

	dev_dbg(fsl_lpspi->dev, "FCR=0x%x\n", temp);
}