static ssize_t lg4ff_real_id_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	/* Real ID is a read-only value */
	return -EPERM;
}