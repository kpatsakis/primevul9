static inline bool f2fs_is_drop_cache(struct inode *inode)
{
	return is_inode_flag_set(inode, FI_DROP_CACHE);
}