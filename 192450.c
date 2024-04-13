static int fsl_lpspi_calculate_timeout(struct fsl_lpspi_data *fsl_lpspi,
				       int size)
{
	unsigned long timeout = 0;

	/* Time with actual data transfer and CS change delay related to HW */
	timeout = (8 + 4) * size / fsl_lpspi->config.speed_hz;

	/* Add extra second for scheduler related activities */
	timeout += 1;

	/* Double calculated timeout */
	return msecs_to_jiffies(2 * timeout * MSEC_PER_SEC);
}