static inline int d_revalidate(struct dentry *dentry, unsigned int flags)
{
	return dentry->d_op->d_revalidate(dentry, flags);
}