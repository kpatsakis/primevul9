static ssize_t ad5755_write_powerdown(struct iio_dev *indio_dev, uintptr_t priv,
	struct iio_chan_spec const *chan, const char *buf, size_t len)
{
	bool pwr_down;
	int ret;

	ret = strtobool(buf, &pwr_down);
	if (ret)
		return ret;

	ret = ad5755_set_channel_pwr_down(indio_dev, chan->channel, pwr_down);
	return ret ? ret : len;
}