static int ad5755_write_ctrl(struct iio_dev *indio_dev, unsigned int channel,
	unsigned int reg, unsigned int val)
{
	int ret;

	mutex_lock(&indio_dev->mlock);
	ret = ad5755_write_ctrl_unlocked(indio_dev, channel, reg, val);
	mutex_unlock(&indio_dev->mlock);

	return ret;
}