static loff_t vcs_lseek(struct file *file, loff_t offset, int orig)
{
	int size;

	console_lock();
	size = vcs_size(file_inode(file));
	console_unlock();
	if (size < 0)
		return size;
	return fixed_size_llseek(file, offset, orig, size);
}