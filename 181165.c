static int ad5755_write_raw(struct iio_dev *indio_dev,
	const struct iio_chan_spec *chan, int val, int val2, long info)
{
	struct ad5755_state *st = iio_priv(indio_dev);
	unsigned int shift, reg, offset;
	int ret;

	ret = ad5755_chan_reg_info(st, chan, info, true,
					&reg, &shift, &offset);
	if (ret)
		return ret;

	val <<= shift;
	val += offset;

	if (val < 0 || val > 0xffff)
		return -EINVAL;

	return ad5755_write(indio_dev, reg, val);
}