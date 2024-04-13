static int sg_proc_single_open_version(struct inode *inode, struct file *file)
{
	return single_open(file, sg_proc_seq_show_version, NULL);
}