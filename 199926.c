static int bad_inode_mknod (struct inode *dir, struct dentry *dentry,
			int mode, dev_t rdev)
{
	return -EIO;
}