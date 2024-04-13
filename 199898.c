static int snd_mem_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, snd_mem_proc_read, NULL);
}