bool f2fs_empty_dir(struct inode *dir);

static inline int f2fs_add_link(struct dentry *dentry, struct inode *inode)
{
	return f2fs_do_add_link(d_inode(dentry->d_parent), &dentry->d_name,