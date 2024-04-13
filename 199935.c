static long bad_file_compat_ioctl(struct file *file, unsigned int cmd,
			unsigned long arg)
{
	return -EIO;
}