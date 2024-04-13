static int options_open_fs(struct inode *inode, struct file *file)
{
	return single_open(file, options_seq_show, PDE(inode)->data);
}