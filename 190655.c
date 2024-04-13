static inline int f2fs_has_inline_data(struct inode *inode)
{
	return is_inode_flag_set(inode, FI_INLINE_DATA);
}