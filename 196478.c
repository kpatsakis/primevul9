static int rpmsg_eptdev_create(struct rpmsg_ctrldev *ctrldev,
			       struct rpmsg_channel_info chinfo)
{
	struct rpmsg_device *rpdev = ctrldev->rpdev;
	struct rpmsg_eptdev *eptdev;
	struct device *dev;
	int ret;

	eptdev = kzalloc(sizeof(*eptdev), GFP_KERNEL);
	if (!eptdev)
		return -ENOMEM;

	dev = &eptdev->dev;
	eptdev->rpdev = rpdev;
	eptdev->chinfo = chinfo;

	mutex_init(&eptdev->ept_lock);
	spin_lock_init(&eptdev->queue_lock);
	skb_queue_head_init(&eptdev->queue);
	init_waitqueue_head(&eptdev->readq);

	device_initialize(dev);
	dev->class = rpmsg_class;
	dev->parent = &ctrldev->dev;
	dev->groups = rpmsg_eptdev_groups;
	dev_set_drvdata(dev, eptdev);

	cdev_init(&eptdev->cdev, &rpmsg_eptdev_fops);
	eptdev->cdev.owner = THIS_MODULE;

	ret = ida_simple_get(&rpmsg_minor_ida, 0, RPMSG_DEV_MAX, GFP_KERNEL);
	if (ret < 0)
		goto free_eptdev;
	dev->devt = MKDEV(MAJOR(rpmsg_major), ret);

	ret = ida_simple_get(&rpmsg_ept_ida, 0, 0, GFP_KERNEL);
	if (ret < 0)
		goto free_minor_ida;
	dev->id = ret;
	dev_set_name(dev, "rpmsg%d", ret);

	ret = cdev_add(&eptdev->cdev, dev->devt, 1);
	if (ret)
		goto free_ept_ida;

	/* We can now rely on the release function for cleanup */
	dev->release = rpmsg_eptdev_release_device;

	ret = device_add(dev);
	if (ret) {
		dev_err(dev, "device_add failed: %d\n", ret);
		put_device(dev);
	}

	return ret;

free_ept_ida:
	ida_simple_remove(&rpmsg_ept_ida, dev->id);
free_minor_ida:
	ida_simple_remove(&rpmsg_minor_ida, MINOR(dev->devt));
free_eptdev:
	put_device(dev);
	kfree(eptdev);

	return ret;
}