loff_t noop_llseek(struct file *file, loff_t offset, int whence)
{
	return file->f_pos;
}