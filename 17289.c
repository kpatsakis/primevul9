static long do_rmdir(int dfd, const char __user *pathname)
{
	int error = 0;
	struct filename *name;
	struct dentry *dentry;
	struct nameidata nd;
	unsigned int lookup_flags = 0;
retry:
	name = user_path_parent(dfd, pathname, &nd, lookup_flags);
	if (IS_ERR(name))
		return PTR_ERR(name);

	switch(nd.last_type) {
	case LAST_DOTDOT:
		error = -ENOTEMPTY;
		goto exit1;
	case LAST_DOT:
		error = -EINVAL;
		goto exit1;
	case LAST_ROOT:
		error = -EBUSY;
		goto exit1;
	}

	nd.flags &= ~LOOKUP_PARENT;
	error = mnt_want_write(nd.path.mnt);
	if (error)
		goto exit1;

	mutex_lock_nested(&nd.path.dentry->d_inode->i_mutex, I_MUTEX_PARENT);
	dentry = lookup_hash(&nd);
	error = PTR_ERR(dentry);
	if (IS_ERR(dentry))
		goto exit2;
	if (!dentry->d_inode) {
		error = -ENOENT;
		goto exit3;
	}
	error = security_path_rmdir(&nd.path, dentry);
	if (error)
		goto exit3;
	error = vfs_rmdir(nd.path.dentry->d_inode, dentry);
exit3:
	dput(dentry);
exit2:
	mutex_unlock(&nd.path.dentry->d_inode->i_mutex);
	mnt_drop_write(nd.path.mnt);
exit1:
	path_put(&nd.path);
	putname(name);
	if (retry_estale(error, lookup_flags)) {
		lookup_flags |= LOOKUP_REVAL;
		goto retry;
	}
	return error;
}