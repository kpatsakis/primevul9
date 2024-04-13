static int bad_file_release(struct inode *inode, struct file *filp)
{
	return -EIO;
}