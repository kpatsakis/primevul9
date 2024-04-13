static int ad5755_write_ctrl_unlocked(struct iio_dev *indio_dev,
	unsigned int channel, unsigned int reg, unsigned int val)
{
	return ad5755_write_unlocked(indio_dev,
		AD5755_WRITE_REG_CTRL(channel), (reg << 13) | val);
}