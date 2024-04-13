static int maps_open(struct inode *inode, struct file *file)
{
	return do_maps_open(inode, file, &proc_pid_maps_op);
}