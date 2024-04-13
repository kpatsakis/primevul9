static int yurex_fasync(int fd, struct file *file, int on)
{
	struct usb_yurex *dev;

	dev = file->private_data;
	return fasync_helper(fd, file, on, &dev->async_queue);
}