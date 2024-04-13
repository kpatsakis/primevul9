static irqreturn_t adis_trigger_handler(int irq, void *p)
{
	struct iio_poll_func *pf = p;
	struct iio_dev *indio_dev = pf->indio_dev;
	struct adis *adis = iio_device_get_drvdata(indio_dev);
	int ret;

	if (!adis->buffer)
		return -ENOMEM;

	if (adis->data->has_paging) {
		mutex_lock(&adis->txrx_lock);
		if (adis->current_page != 0) {
			adis->tx[0] = ADIS_WRITE_REG(ADIS_REG_PAGE_ID);
			adis->tx[1] = 0;
			spi_write(adis->spi, adis->tx, 2);
		}
	}

	ret = spi_sync(adis->spi, &adis->msg);
	if (ret)
		dev_err(&adis->spi->dev, "Failed to read data: %d", ret);


	if (adis->data->has_paging) {
		adis->current_page = 0;
		mutex_unlock(&adis->txrx_lock);
	}

	iio_push_to_buffers_with_timestamp(indio_dev, adis->buffer,
		pf->timestamp);

	iio_trigger_notify_done(indio_dev->trig);

	return IRQ_HANDLED;
}