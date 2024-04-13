struct dentry *kern_path_locked(const char *name, struct path *path)
{
	struct nameidata nd;
	struct dentry *d;
	int err = do_path_lookup(AT_FDCWD, name, LOOKUP_PARENT, &nd);
	if (err)
		return ERR_PTR(err);
	if (nd.last_type != LAST_NORM) {
		path_put(&nd.path);
		return ERR_PTR(-EINVAL);
	}
	mutex_lock_nested(&nd.path.dentry->d_inode->i_mutex, I_MUTEX_PARENT);
	d = __lookup_hash(&nd.last, nd.path.dentry, 0);
	if (IS_ERR(d)) {
		mutex_unlock(&nd.path.dentry->d_inode->i_mutex);
		path_put(&nd.path);
		return d;
	}
	*path = nd.path;
	return d;
}