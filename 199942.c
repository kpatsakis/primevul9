static ssize_t bad_inode_listxattr(struct dentry *dentry, char *buffer,
			size_t buffer_size)
{
	return -EIO;
}