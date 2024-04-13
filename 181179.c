static int ad5755_write(struct iio_dev *indio_dev, unsigned int reg,
	unsigned int val)
{
	int ret;

	mutex_lock(&indio_dev->mlock);
	ret = ad5755_write_unlocked(indio_dev, reg, val);
	mutex_unlock(&indio_dev->mlock);

	return ret;
}