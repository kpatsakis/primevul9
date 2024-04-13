static inline int walk_component(struct nameidata *nd, struct path *path,
		int follow)
{
	struct inode *inode;
	int err;
	/*
	 * "." and ".." are special - ".." especially so because it has
	 * to be able to know about the current root directory and
	 * parent relationships.
	 */
	if (unlikely(nd->last_type != LAST_NORM))
		return handle_dots(nd, nd->last_type);
	err = lookup_fast(nd, path, &inode);
	if (unlikely(err)) {
		if (err < 0)
			goto out_err;

		err = lookup_slow(nd, path);
		if (err < 0)
			goto out_err;

		inode = path->dentry->d_inode;
	}
	err = -ENOENT;
	if (!inode)
		goto out_path_put;

	if (should_follow_link(inode, follow)) {
		if (nd->flags & LOOKUP_RCU) {
			if (unlikely(unlazy_walk(nd, path->dentry))) {
				err = -ECHILD;
				goto out_err;
			}
		}
		BUG_ON(inode != path->dentry->d_inode);
		return 1;
	}
	path_to_nameidata(path, nd);
	nd->inode = inode;
	return 0;

out_path_put:
	path_to_nameidata(path, nd);
out_err:
	terminate_walk(nd);
	return err;
}