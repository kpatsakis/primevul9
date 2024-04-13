static int configfs_do_nothing(struct usb_composite_dev *cdev)
{
	WARN_ON(1);
	return -EINVAL;
}