static int fsl_lpspi_remove(struct platform_device *pdev)
{
	struct spi_controller *controller = platform_get_drvdata(pdev);
	struct fsl_lpspi_data *fsl_lpspi =
				spi_controller_get_devdata(controller);

	pm_runtime_disable(fsl_lpspi->dev);

	spi_master_put(controller);

	return 0;
}