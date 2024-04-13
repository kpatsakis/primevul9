static int bad_inode_rmdir (struct inode *dir, struct dentry *dentry)
{
	return -EIO;
}