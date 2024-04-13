int adis_setup_buffer_and_trigger(struct adis *adis, struct iio_dev *indio_dev,
	irqreturn_t (*trigger_handler)(int, void *))
{
	int ret;

	if (!trigger_handler)
		trigger_handler = adis_trigger_handler;

	ret = iio_triggered_buffer_setup(indio_dev, &iio_pollfunc_store_time,
		trigger_handler, NULL);
	if (ret)
		return ret;

	if (adis->spi->irq) {
		ret = adis_probe_trigger(adis, indio_dev);
		if (ret)
			goto error_buffer_cleanup;
	}
	return 0;

error_buffer_cleanup:
	iio_triggered_buffer_cleanup(indio_dev);
	return ret;
}