static long rpmsg_ctrldev_ioctl(struct file *fp, unsigned int cmd,
				unsigned long arg)
{
	struct rpmsg_ctrldev *ctrldev = fp->private_data;
	void __user *argp = (void __user *)arg;
	struct rpmsg_endpoint_info eptinfo;
	struct rpmsg_channel_info chinfo;

	if (cmd != RPMSG_CREATE_EPT_IOCTL)
		return -EINVAL;

	if (copy_from_user(&eptinfo, argp, sizeof(eptinfo)))
		return -EFAULT;

	memcpy(chinfo.name, eptinfo.name, RPMSG_NAME_SIZE);
	chinfo.name[RPMSG_NAME_SIZE-1] = '\0';
	chinfo.src = eptinfo.src;
	chinfo.dst = eptinfo.dst;

	return rpmsg_eptdev_create(ctrldev, chinfo);
};