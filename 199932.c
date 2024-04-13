static int bad_inode_mkdir(struct inode *dir, struct dentry *dentry,
			int mode)
{
	return -EIO;
}