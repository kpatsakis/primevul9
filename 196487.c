static int rpmsg_chrdev_probe(struct rpmsg_device *rpdev)
{
	struct rpmsg_ctrldev *ctrldev;
	struct device *dev;
	int ret;

	ctrldev = kzalloc(sizeof(*ctrldev), GFP_KERNEL);
	if (!ctrldev)
		return -ENOMEM;

	ctrldev->rpdev = rpdev;

	dev = &ctrldev->dev;
	device_initialize(dev);
	dev->parent = &rpdev->dev;
	dev->class = rpmsg_class;

	cdev_init(&ctrldev->cdev, &rpmsg_ctrldev_fops);
	ctrldev->cdev.owner = THIS_MODULE;

	ret = ida_simple_get(&rpmsg_minor_ida, 0, RPMSG_DEV_MAX, GFP_KERNEL);
	if (ret < 0)
		goto free_ctrldev;
	dev->devt = MKDEV(MAJOR(rpmsg_major), ret);

	ret = ida_simple_get(&rpmsg_ctrl_ida, 0, 0, GFP_KERNEL);
	if (ret < 0)
		goto free_minor_ida;
	dev->id = ret;
	dev_set_name(&ctrldev->dev, "rpmsg_ctrl%d", ret);

	ret = cdev_add(&ctrldev->cdev, dev->devt, 1);
	if (ret)
		goto free_ctrl_ida;

	/* We can now rely on the release function for cleanup */
	dev->release = rpmsg_ctrldev_release_device;

	ret = device_add(dev);
	if (ret) {
		dev_err(&rpdev->dev, "device_add failed: %d\n", ret);
		put_device(dev);
	}

	dev_set_drvdata(&rpdev->dev, ctrldev);

	return ret;

free_ctrl_ida:
	ida_simple_remove(&rpmsg_ctrl_ida, dev->id);
free_minor_ida:
	ida_simple_remove(&rpmsg_minor_ida, MINOR(dev->devt));
free_ctrldev:
	put_device(dev);
	kfree(ctrldev);

	return ret;
}