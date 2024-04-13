static int open_rio(struct inode *inode, struct file *file)
{
	struct rio_usb_data *rio = &rio_instance;

	/* against disconnect() */
	mutex_lock(&rio500_mutex);
	mutex_lock(&(rio->lock));

	if (rio->isopen || !rio->present) {
		mutex_unlock(&(rio->lock));
		mutex_unlock(&rio500_mutex);
		return -EBUSY;
	}
	rio->isopen = 1;

	init_waitqueue_head(&rio->wait_q);

	mutex_unlock(&(rio->lock));

	dev_info(&rio->rio_dev->dev, "Rio opened.\n");
	mutex_unlock(&rio500_mutex);

	return 0;
}