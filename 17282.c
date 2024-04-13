void dentry_unhash(struct dentry *dentry)
{
	shrink_dcache_parent(dentry);
	spin_lock(&dentry->d_lock);
	if (dentry->d_count == 1)
		__d_drop(dentry);
	spin_unlock(&dentry->d_lock);
}