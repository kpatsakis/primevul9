static int fsl_lpspi_init_rpm(struct fsl_lpspi_data *fsl_lpspi)
{
	struct device *dev = fsl_lpspi->dev;

	pm_runtime_enable(dev);
	pm_runtime_set_autosuspend_delay(dev, FSL_LPSPI_RPM_TIMEOUT);
	pm_runtime_use_autosuspend(dev);

	return 0;
}