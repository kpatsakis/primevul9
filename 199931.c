static int bad_inode_symlink (struct inode *dir, struct dentry *dentry,
		const char *symname)
{
	return -EIO;
}