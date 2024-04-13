static int fsl_lpspi_wait_for_completion(struct spi_controller *controller)
{
	struct fsl_lpspi_data *fsl_lpspi =
				spi_controller_get_devdata(controller);

	if (fsl_lpspi->is_slave) {
		if (wait_for_completion_interruptible(&fsl_lpspi->xfer_done) ||
			fsl_lpspi->slave_aborted) {
			dev_dbg(fsl_lpspi->dev, "interrupted\n");
			return -EINTR;
		}
	} else {
		if (!wait_for_completion_timeout(&fsl_lpspi->xfer_done, HZ)) {
			dev_dbg(fsl_lpspi->dev, "wait for completion timeout\n");
			return -ETIMEDOUT;
		}
	}

	return 0;
}