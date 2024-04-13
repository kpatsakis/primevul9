static int lpspi_unprepare_xfer_hardware(struct spi_controller *controller)
{
	struct fsl_lpspi_data *fsl_lpspi =
				spi_controller_get_devdata(controller);

	pm_runtime_mark_last_busy(fsl_lpspi->dev);
	pm_runtime_put_autosuspend(fsl_lpspi->dev);

	return 0;
}