static int bad_inode_readlink(struct dentry *dentry, char __user *buffer,
		int buflen)
{
	return -EIO;
}