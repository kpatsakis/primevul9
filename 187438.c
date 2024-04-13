static int mdesc_close(struct inode *inode, struct file *file)
{
	mdesc_release(file->private_data);
	return 0;
}