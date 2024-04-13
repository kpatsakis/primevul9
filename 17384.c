struct dentry *kern_path_create(int dfd, const char *pathname,
				struct path *path, unsigned int lookup_flags)
{
	struct dentry *dentry = ERR_PTR(-EEXIST);
	struct nameidata nd;
	int err2;
	int error;
	bool is_dir = (lookup_flags & LOOKUP_DIRECTORY);

	/*
	 * Note that only LOOKUP_REVAL and LOOKUP_DIRECTORY matter here. Any
	 * other flags passed in are ignored!
	 */
	lookup_flags &= LOOKUP_REVAL;

	error = do_path_lookup(dfd, pathname, LOOKUP_PARENT|lookup_flags, &nd);
	if (error)
		return ERR_PTR(error);

	/*
	 * Yucky last component or no last component at all?
	 * (foo/., foo/.., /////)
	 */
	if (nd.last_type != LAST_NORM)
		goto out;
	nd.flags &= ~LOOKUP_PARENT;
	nd.flags |= LOOKUP_CREATE | LOOKUP_EXCL;

	/* don't fail immediately if it's r/o, at least try to report other errors */
	err2 = mnt_want_write(nd.path.mnt);
	/*
	 * Do the final lookup.
	 */
	mutex_lock_nested(&nd.path.dentry->d_inode->i_mutex, I_MUTEX_PARENT);
	dentry = lookup_hash(&nd);
	if (IS_ERR(dentry))
		goto unlock;

	error = -EEXIST;
	if (dentry->d_inode)
		goto fail;
	/*
	 * Special case - lookup gave negative, but... we had foo/bar/
	 * From the vfs_mknod() POV we just have a negative dentry -
	 * all is fine. Let's be bastards - you had / on the end, you've
	 * been asking for (non-existent) directory. -ENOENT for you.
	 */
	if (unlikely(!is_dir && nd.last.name[nd.last.len])) {
		error = -ENOENT;
		goto fail;
	}
	if (unlikely(err2)) {
		error = err2;
		goto fail;
	}
	*path = nd.path;
	return dentry;
fail:
	dput(dentry);
	dentry = ERR_PTR(error);
unlock:
	mutex_unlock(&nd.path.dentry->d_inode->i_mutex);
	if (!err2)
		mnt_drop_write(nd.path.mnt);
out:
	path_put(&nd.path);
	return dentry;
}