static int close_rio(struct inode *inode, struct file *file)
{
	struct rio_usb_data *rio = &rio_instance;

	rio->isopen = 0;

	dev_info(&rio->rio_dev->dev, "Rio closed.\n");
	return 0;
}