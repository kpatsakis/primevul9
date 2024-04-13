static int lpspi_prepare_xfer_hardware(struct spi_controller *controller)
{
	struct fsl_lpspi_data *fsl_lpspi =
				spi_controller_get_devdata(controller);
	int ret;

	ret = pm_runtime_get_sync(fsl_lpspi->dev);
	if (ret < 0) {
		dev_err(fsl_lpspi->dev, "failed to enable clock\n");
		return ret;
	}

	return 0;
}