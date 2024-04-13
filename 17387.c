int generic_readlink(struct dentry *dentry, char __user *buffer, int buflen)
{
	struct nameidata nd;
	void *cookie;
	int res;

	nd.depth = 0;
	cookie = dentry->d_inode->i_op->follow_link(dentry, &nd);
	if (IS_ERR(cookie))
		return PTR_ERR(cookie);

	res = vfs_readlink(dentry, buffer, buflen, nd_get_link(&nd));
	if (dentry->d_inode->i_op->put_link)
		dentry->d_inode->i_op->put_link(dentry, &nd, cookie);
	return res;
}