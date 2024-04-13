void adis_cleanup_buffer_and_trigger(struct adis *adis,
	struct iio_dev *indio_dev)
{
	if (adis->spi->irq)
		adis_remove_trigger(adis);
	kfree(adis->buffer);
	kfree(adis->xfer);
	iio_triggered_buffer_cleanup(indio_dev);
}