static void rpmsg_ctrldev_release_device(struct device *dev)
{
	struct rpmsg_ctrldev *ctrldev = dev_to_ctrldev(dev);

	ida_simple_remove(&rpmsg_ctrl_ida, dev->id);
	ida_simple_remove(&rpmsg_minor_ida, MINOR(dev->devt));
	cdev_del(&ctrldev->cdev);
	kfree(ctrldev);
}