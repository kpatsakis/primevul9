static int smaps_open(struct inode *inode, struct file *file)
{
	return do_maps_open(inode, file, &proc_pid_smaps_op);
}