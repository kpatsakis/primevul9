static ssize_t bad_inode_getxattr(struct dentry *dentry, const char *name,
			void *buffer, size_t size)
{
	return -EIO;
}