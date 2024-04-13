static int bad_inode_link (struct dentry *old_dentry, struct inode *dir,
		struct dentry *dentry)
{
	return -EIO;
}