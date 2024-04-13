static ssize_t show_bank(struct device *s, struct device_attribute *attr,
			 char *buf)
{
	return sprintf(buf, "%llx\n", attr_to_bank(attr)->ctl);
}