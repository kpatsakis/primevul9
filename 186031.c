loff_t no_llseek(struct file *file, loff_t offset, int whence)
{
	return -ESPIPE;
}