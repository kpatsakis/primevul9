static int may_o_create(struct path *dir, struct dentry *dentry, umode_t mode)
{
	int error = security_path_mknod(dir, dentry, mode, 0);
	if (error)
		return error;

	error = inode_permission(dir->dentry->d_inode, MAY_WRITE | MAY_EXEC);
	if (error)
		return error;

	return security_inode_create(dir->dentry->d_inode, dentry, mode);
}