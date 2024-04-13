static int unlazy_walk(struct nameidata *nd, struct dentry *dentry)
{
	struct fs_struct *fs = current->fs;
	struct dentry *parent = nd->path.dentry;
	int want_root = 0;

	BUG_ON(!(nd->flags & LOOKUP_RCU));
	if (nd->root.mnt && !(nd->flags & LOOKUP_ROOT)) {
		want_root = 1;
		spin_lock(&fs->lock);
		if (nd->root.mnt != fs->root.mnt ||
				nd->root.dentry != fs->root.dentry)
			goto err_root;
	}
	spin_lock(&parent->d_lock);
	if (!dentry) {
		if (!__d_rcu_to_refcount(parent, nd->seq))
			goto err_parent;
		BUG_ON(nd->inode != parent->d_inode);
	} else {
		if (dentry->d_parent != parent)
			goto err_parent;
		spin_lock_nested(&dentry->d_lock, DENTRY_D_LOCK_NESTED);
		if (!__d_rcu_to_refcount(dentry, nd->seq))
			goto err_child;
		/*
		 * If the sequence check on the child dentry passed, then
		 * the child has not been removed from its parent. This
		 * means the parent dentry must be valid and able to take
		 * a reference at this point.
		 */
		BUG_ON(!IS_ROOT(dentry) && dentry->d_parent != parent);
		BUG_ON(!parent->d_count);
		parent->d_count++;
		spin_unlock(&dentry->d_lock);
	}
	spin_unlock(&parent->d_lock);
	if (want_root) {
		path_get(&nd->root);
		spin_unlock(&fs->lock);
	}
	mntget(nd->path.mnt);

	unlock_rcu_walk();
	nd->flags &= ~LOOKUP_RCU;
	return 0;

err_child:
	spin_unlock(&dentry->d_lock);
err_parent:
	spin_unlock(&parent->d_lock);
err_root:
	if (want_root)
		spin_unlock(&fs->lock);
	return -ECHILD;
}