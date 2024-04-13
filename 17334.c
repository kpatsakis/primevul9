static int complete_walk(struct nameidata *nd)
{
	struct dentry *dentry = nd->path.dentry;
	int status;

	if (nd->flags & LOOKUP_RCU) {
		nd->flags &= ~LOOKUP_RCU;
		if (!(nd->flags & LOOKUP_ROOT))
			nd->root.mnt = NULL;
		spin_lock(&dentry->d_lock);
		if (unlikely(!__d_rcu_to_refcount(dentry, nd->seq))) {
			spin_unlock(&dentry->d_lock);
			unlock_rcu_walk();
			return -ECHILD;
		}
		BUG_ON(nd->inode != dentry->d_inode);
		spin_unlock(&dentry->d_lock);
		mntget(nd->path.mnt);
		unlock_rcu_walk();
	}

	if (likely(!(nd->flags & LOOKUP_JUMPED)))
		return 0;

	if (likely(!(dentry->d_flags & DCACHE_OP_WEAK_REVALIDATE)))
		return 0;

	status = dentry->d_op->d_weak_revalidate(dentry, nd->flags);
	if (status > 0)
		return 0;

	if (!status)
		status = -ESTALE;

	path_put(&nd->path);
	return status;
}