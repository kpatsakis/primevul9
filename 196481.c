static ssize_t dst_show(struct device *dev, struct device_attribute *attr,
			 char *buf)
{
	struct rpmsg_eptdev *eptdev = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", eptdev->chinfo.dst);
}