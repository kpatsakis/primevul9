static int bad_inode_create (struct inode *dir, struct dentry *dentry,
		int mode, struct nameidata *nd)
{
	return -EIO;
}