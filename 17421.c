SYSCALL_DEFINE3(mkdirat, int, dfd, const char __user *, pathname, umode_t, mode)
{
	struct dentry *dentry;
	struct path path;
	int error;
	unsigned int lookup_flags = LOOKUP_DIRECTORY;

retry:
	dentry = user_path_create(dfd, pathname, &path, lookup_flags);
	if (IS_ERR(dentry))
		return PTR_ERR(dentry);

	if (!IS_POSIXACL(path.dentry->d_inode))
		mode &= ~current_umask();
	error = security_path_mkdir(&path, dentry, mode);
	if (!error)
		error = vfs_mkdir(path.dentry->d_inode, dentry, mode);
	done_path_create(&path, dentry);
	if (retry_estale(error, lookup_flags)) {
		lookup_flags |= LOOKUP_REVAL;
		goto retry;
	}
	return error;
}