static int bad_inode_setxattr(struct dentry *dentry, const char *name,
		const void *value, size_t size, int flags)
{
	return -EIO;
}