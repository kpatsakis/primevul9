static int rpmsg_eptdev_open(struct inode *inode, struct file *filp)
{
	struct rpmsg_eptdev *eptdev = cdev_to_eptdev(inode->i_cdev);
	struct rpmsg_endpoint *ept;
	struct rpmsg_device *rpdev = eptdev->rpdev;
	struct device *dev = &eptdev->dev;

	get_device(dev);

	ept = rpmsg_create_ept(rpdev, rpmsg_ept_cb, eptdev, eptdev->chinfo);
	if (!ept) {
		dev_err(dev, "failed to open %s\n", eptdev->chinfo.name);
		put_device(dev);
		return -EINVAL;
	}

	eptdev->ept = ept;
	filp->private_data = eptdev;

	return 0;
}