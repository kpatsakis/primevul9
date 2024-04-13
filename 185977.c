loff_t vfs_llseek(struct file *file, loff_t offset, int whence)
{
	loff_t (*fn)(struct file *, loff_t, int);

	fn = no_llseek;
	if (file->f_mode & FMODE_LSEEK) {
		if (file->f_op->llseek)
			fn = file->f_op->llseek;
	}
	return fn(file, offset, whence);
}