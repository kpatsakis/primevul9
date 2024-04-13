static inline bool managed_dentry_might_block(struct dentry *dentry)
{
	return (dentry->d_flags & DCACHE_MANAGE_TRANSIT &&
		dentry->d_op->d_manage(dentry, true) < 0);
}