static void rpmsg_eptdev_release_device(struct device *dev)
{
	struct rpmsg_eptdev *eptdev = dev_to_eptdev(dev);

	ida_simple_remove(&rpmsg_ept_ida, dev->id);
	ida_simple_remove(&rpmsg_minor_ida, MINOR(eptdev->dev.devt));
	cdev_del(&eptdev->cdev);
	kfree(eptdev);
}