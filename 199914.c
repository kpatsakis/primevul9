static int bad_inode_unlink(struct inode *dir, struct dentry *dentry)
{
	return -EIO;
}