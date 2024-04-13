static int ad5755_probe(struct spi_device *spi)
{
	enum ad5755_type type = spi_get_device_id(spi)->driver_data;
	const struct ad5755_platform_data *pdata = dev_get_platdata(&spi->dev);
	struct iio_dev *indio_dev;
	struct ad5755_state *st;
	int ret;

	indio_dev = devm_iio_device_alloc(&spi->dev, sizeof(*st));
	if (indio_dev == NULL) {
		dev_err(&spi->dev, "Failed to allocate iio device\n");
		return  -ENOMEM;
	}

	st = iio_priv(indio_dev);
	spi_set_drvdata(spi, indio_dev);

	st->chip_info = &ad5755_chip_info_tbl[type];
	st->spi = spi;
	st->pwr_down = 0xf;

	indio_dev->dev.parent = &spi->dev;
	indio_dev->name = spi_get_device_id(spi)->name;
	indio_dev->info = &ad5755_info;
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->num_channels = AD5755_NUM_CHANNELS;

	if (spi->dev.of_node)
		pdata = ad5755_parse_dt(&spi->dev);
	else
		pdata = spi->dev.platform_data;

	if (!pdata) {
		dev_warn(&spi->dev, "no platform data? using default\n");
		pdata = &ad5755_default_pdata;
	}

	ret = ad5755_init_channels(indio_dev, pdata);
	if (ret)
		return ret;

	ret = ad5755_setup_pdata(indio_dev, pdata);
	if (ret)
		return ret;

	return devm_iio_device_register(&spi->dev, indio_dev);
}