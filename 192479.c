static int fsl_lpspi_runtime_resume(struct device *dev)
{
	struct spi_controller *controller = dev_get_drvdata(dev);
	struct fsl_lpspi_data *fsl_lpspi;
	int ret;

	fsl_lpspi = spi_controller_get_devdata(controller);

	ret = clk_prepare_enable(fsl_lpspi->clk_per);
	if (ret)
		return ret;

	ret = clk_prepare_enable(fsl_lpspi->clk_ipg);
	if (ret) {
		clk_disable_unprepare(fsl_lpspi->clk_per);
		return ret;
	}

	return 0;
}