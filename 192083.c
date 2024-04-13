static ssize_t set_bank(struct device *s, struct device_attribute *attr,
			const char *buf, size_t size)
{
	u64 new;

	if (kstrtou64(buf, 0, &new) < 0)
		return -EINVAL;

	attr_to_bank(attr)->ctl = new;
	mce_restart();

	return size;
}