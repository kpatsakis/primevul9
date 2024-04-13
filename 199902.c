static int bad_file_ioctl (struct inode *inode, struct file *filp,
			unsigned int cmd, unsigned long arg)
{
	return -EIO;
}