static inline int f2fs_has_extra_attr(struct inode *inode)
{
	return is_inode_flag_set(inode, FI_EXTRA_ATTR);
}