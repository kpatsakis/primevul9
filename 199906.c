static int bad_file_open(struct inode *inode, struct file *filp)
{
	return -EIO;
}