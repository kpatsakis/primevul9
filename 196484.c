static int rpmsg_eptdev_destroy(struct device *dev, void *data)
{
	struct rpmsg_eptdev *eptdev = dev_to_eptdev(dev);

	mutex_lock(&eptdev->ept_lock);
	if (eptdev->ept) {
		rpmsg_destroy_ept(eptdev->ept);
		eptdev->ept = NULL;
	}
	mutex_unlock(&eptdev->ept_lock);

	/* wake up any blocked readers */
	wake_up_interruptible(&eptdev->readq);

	device_del(&eptdev->dev);
	put_device(&eptdev->dev);

	return 0;
}