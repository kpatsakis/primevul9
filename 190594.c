static inline int f2fs_exist_data(struct inode *inode)
{
	return is_inode_flag_set(inode, FI_DATA_EXIST);
}