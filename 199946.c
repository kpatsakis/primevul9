static struct dentry *bad_inode_lookup(struct inode *dir,
			struct dentry *dentry, struct nameidata *nd)
{
	return ERR_PTR(-EIO);
}