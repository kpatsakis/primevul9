int seq_file_path(struct seq_file *m, struct file *file, const char *esc)
{
	return seq_path(m, &file->f_path, esc);
}