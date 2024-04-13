static int ad5755_chan_reg_info(struct ad5755_state *st,
	struct iio_chan_spec const *chan, long info, bool write,
	unsigned int *reg, unsigned int *shift, unsigned int *offset)
{
	switch (info) {
	case IIO_CHAN_INFO_RAW:
		if (write)
			*reg = AD5755_WRITE_REG_DATA(chan->address);
		else
			*reg = AD5755_READ_REG_DATA(chan->address);
		*shift = chan->scan_type.shift;
		*offset = 0;
		break;
	case IIO_CHAN_INFO_CALIBBIAS:
		if (write)
			*reg = AD5755_WRITE_REG_OFFSET(chan->address);
		else
			*reg = AD5755_READ_REG_OFFSET(chan->address);
		*shift = st->chip_info->calib_shift;
		*offset = 32768;
		break;
	case IIO_CHAN_INFO_CALIBSCALE:
		if (write)
			*reg =  AD5755_WRITE_REG_GAIN(chan->address);
		else
			*reg =  AD5755_READ_REG_GAIN(chan->address);
		*shift = st->chip_info->calib_shift;
		*offset = 0;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}