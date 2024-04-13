static int ad5755_read_raw(struct iio_dev *indio_dev,
	const struct iio_chan_spec *chan, int *val, int *val2, long info)
{
	struct ad5755_state *st = iio_priv(indio_dev);
	unsigned int reg, shift, offset;
	int min, max;
	int ret;

	switch (info) {
	case IIO_CHAN_INFO_SCALE:
		ad5755_get_min_max(st, chan, &min, &max);
		*val = max - min;
		*val2 = chan->scan_type.realbits;
		return IIO_VAL_FRACTIONAL_LOG2;
	case IIO_CHAN_INFO_OFFSET:
		*val = ad5755_get_offset(st, chan);
		return IIO_VAL_INT;
	default:
		ret = ad5755_chan_reg_info(st, chan, info, false,
						&reg, &shift, &offset);
		if (ret)
			return ret;

		ret = ad5755_read(indio_dev, reg);
		if (ret < 0)
			return ret;

		*val = (ret - offset) >> shift;

		return IIO_VAL_INT;
	}

	return -EINVAL;
}