static inline void file_pos_write(struct file *file, loff_t pos)
{
	file->f_pos = pos;
}