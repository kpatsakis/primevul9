int finish_open(struct file *file, struct dentry *dentry,
		int (*open)(struct inode *, struct file *),
		int *opened)
{
	int error;
	BUG_ON(*opened & FILE_OPENED); /* once it's opened, it's opened */

	file->f_path.dentry = dentry;
	error = do_dentry_open(file, open, current_cred());
	if (!error)
		*opened |= FILE_OPENED;

	return error;
}