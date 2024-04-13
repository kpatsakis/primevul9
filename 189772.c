static int numa_maps_open(struct inode *inode, struct file *file)
{
	return do_maps_open(inode, file, &proc_pid_numa_maps_op);
}