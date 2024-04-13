static int fsl_lpspi_runtime_suspend(struct device *dev)
{
	struct spi_controller *controller = dev_get_drvdata(dev);
	struct fsl_lpspi_data *fsl_lpspi;

	fsl_lpspi = spi_controller_get_devdata(controller);

	clk_disable_unprepare(fsl_lpspi->clk_per);
	clk_disable_unprepare(fsl_lpspi->clk_ipg);

	return 0;
}