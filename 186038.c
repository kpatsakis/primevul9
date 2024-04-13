static inline int unsigned_offsets(struct file *file)
{
	return file->f_mode & FMODE_UNSIGNED_OFFSET;
}