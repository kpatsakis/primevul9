int finish_no_open(struct file *file, struct dentry *dentry)
{
	file->f_path.dentry = dentry;
	return 1;
}