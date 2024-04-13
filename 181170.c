static ssize_t ad5755_read_powerdown(struct iio_dev *indio_dev, uintptr_t priv,
	const struct iio_chan_spec *chan, char *buf)
{
	struct ad5755_state *st = iio_priv(indio_dev);

	return sprintf(buf, "%d\n",
		       (bool)(st->pwr_down & (1 << chan->channel)));
}