static int ad5755_update_dac_ctrl(struct iio_dev *indio_dev,
	unsigned int channel, unsigned int set, unsigned int clr)
{
	struct ad5755_state *st = iio_priv(indio_dev);
	int ret;

	st->ctrl[channel] |= set;
	st->ctrl[channel] &= ~clr;

	ret = ad5755_write_ctrl_unlocked(indio_dev, channel,
		AD5755_CTRL_REG_DAC, st->ctrl[channel]);

	return ret;
}