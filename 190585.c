static inline bool f2fs_is_pinned_file(struct inode *inode)
{
	return is_inode_flag_set(inode, FI_PIN_FILE);
}