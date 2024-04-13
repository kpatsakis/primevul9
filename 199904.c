static long bad_file_unlocked_ioctl(struct file *file, unsigned cmd,
			unsigned long arg)
{
	return -EIO;
}