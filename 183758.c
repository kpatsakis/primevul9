static int yurex_release(struct inode *inode, struct file *file)
{
	struct usb_yurex *dev;

	dev = file->private_data;
	if (dev == NULL)
		return -ENODEV;

	/* decrement the count on our device */
	kref_put(&dev->kref, yurex_delete);
	return 0;
}