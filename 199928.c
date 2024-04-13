static int bad_inode_removexattr(struct dentry *dentry, const char *name)
{
	return -EIO;
}