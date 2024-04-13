long vfs_truncate(struct path *path, loff_t length)
{
	struct inode *inode;
	long error;

	inode = path->dentry->d_inode;

	/* For directories it's -EISDIR, for other non-regulars - -EINVAL */
	if (S_ISDIR(inode->i_mode))
		return -EISDIR;
	if (!S_ISREG(inode->i_mode))
		return -EINVAL;

	error = mnt_want_write(path->mnt);
	if (error)
		goto out;

	error = inode_permission(inode, MAY_WRITE);
	if (error)
		goto mnt_drop_write_and_out;

	error = -EPERM;
	if (IS_APPEND(inode))
		goto mnt_drop_write_and_out;

	error = get_write_access(inode);
	if (error)
		goto mnt_drop_write_and_out;

	/*
	 * Make sure that there are no leases.  get_write_access() protects
	 * against the truncate racing with a lease-granting setlease().
	 */
	error = break_lease(inode, O_WRONLY);
	if (error)
		goto put_write_and_out;

	error = locks_verify_truncate(inode, NULL, length);
	if (!error)
		error = security_path_truncate(path);
	if (!error)
		error = do_truncate(path->dentry, length, 0, NULL);

put_write_and_out:
	put_write_access(inode);
mnt_drop_write_and_out:
	mnt_drop_write(path->mnt);
out:
	return error;
}