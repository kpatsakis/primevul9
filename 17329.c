static int lookup_slow(struct nameidata *nd, struct path *path)
{
	struct dentry *dentry, *parent;
	int err;

	parent = nd->path.dentry;
	BUG_ON(nd->inode != parent->d_inode);

	mutex_lock(&parent->d_inode->i_mutex);
	dentry = __lookup_hash(&nd->last, parent, nd->flags);
	mutex_unlock(&parent->d_inode->i_mutex);
	if (IS_ERR(dentry))
		return PTR_ERR(dentry);
	path->mnt = nd->path.mnt;
	path->dentry = dentry;
	err = follow_managed(path, nd->flags);
	if (unlikely(err < 0)) {
		path_put_conditional(path, nd);
		return err;
	}
	if (err)
		nd->flags |= LOOKUP_JUMPED;
	return 0;
}