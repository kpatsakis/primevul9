static int yurex_open(struct inode *inode, struct file *file)
{
	struct usb_yurex *dev;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;

	subminor = iminor(inode);

	interface = usb_find_interface(&yurex_driver, subminor);
	if (!interface) {
		printk(KERN_ERR "%s - error, can't find device for minor %d",
		       __func__, subminor);
		retval = -ENODEV;
		goto exit;
	}

	dev = usb_get_intfdata(interface);
	if (!dev) {
		retval = -ENODEV;
		goto exit;
	}

	/* increment our usage count for the device */
	kref_get(&dev->kref);

	/* save our object in the file's private structure */
	mutex_lock(&dev->io_mutex);
	file->private_data = dev;
	mutex_unlock(&dev->io_mutex);

exit:
	return retval;
}