static long rpmsg_eptdev_ioctl(struct file *fp, unsigned int cmd,
			       unsigned long arg)
{
	struct rpmsg_eptdev *eptdev = fp->private_data;

	if (cmd != RPMSG_DESTROY_EPT_IOCTL)
		return -EINVAL;

	return rpmsg_eptdev_destroy(&eptdev->dev, NULL);
}