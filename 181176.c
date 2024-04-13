static int ad5755_init_channels(struct iio_dev *indio_dev,
				const struct ad5755_platform_data *pdata)
{
	struct ad5755_state *st = iio_priv(indio_dev);
	struct iio_chan_spec *channels = st->channels;
	unsigned int i;

	for (i = 0; i < AD5755_NUM_CHANNELS; ++i) {
		channels[i] = st->chip_info->channel_template;
		channels[i].channel = i;
		channels[i].address = i;
		if (pdata && ad5755_is_voltage_mode(pdata->dac[i].mode))
			channels[i].type = IIO_VOLTAGE;
		else
			channels[i].type = IIO_CURRENT;
	}

	indio_dev->channels = channels;

	return 0;
}