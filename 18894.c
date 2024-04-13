static int hid_debug_rdesc_open(struct inode *inode, struct file *file)
{
	return single_open(file, hid_debug_rdesc_show, inode->i_private);
}