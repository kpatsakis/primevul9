static int rndis_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, rndis_proc_show, pde_data(inode));
}