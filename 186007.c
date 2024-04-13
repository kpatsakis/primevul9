static inline loff_t file_pos_read(struct file *file)
{
	return file->f_pos;
}