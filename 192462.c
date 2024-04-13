static int fsl_lpspi_set_bitrate(struct fsl_lpspi_data *fsl_lpspi)
{
	struct lpspi_config config = fsl_lpspi->config;
	unsigned int perclk_rate, scldiv;
	u8 prescale;

	perclk_rate = clk_get_rate(fsl_lpspi->clk_per);

	if (config.speed_hz > perclk_rate / 2) {
		dev_err(fsl_lpspi->dev,
		      "per-clk should be at least two times of transfer speed");
		return -EINVAL;
	}

	for (prescale = 0; prescale < 8; prescale++) {
		scldiv = perclk_rate /
			 (clkdivs[prescale] * config.speed_hz) - 2;
		if (scldiv < 256) {
			fsl_lpspi->config.prescale = prescale;
			break;
		}
	}

	if (prescale == 8 && scldiv >= 256)
		return -EINVAL;

	writel(scldiv | (scldiv << 8) | ((scldiv >> 1) << 16),
					fsl_lpspi->base + IMX7ULP_CCR);

	dev_dbg(fsl_lpspi->dev, "perclk=%d, speed=%d, prescale=%d, scldiv=%d\n",
		perclk_rate, config.speed_hz, prescale, scldiv);

	return 0;
}