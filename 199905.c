static int bad_file_fsync(struct file *file, struct dentry *dentry,
			int datasync)
{
	return -EIO;
}