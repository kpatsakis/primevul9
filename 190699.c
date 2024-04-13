static inline int is_inode_flag_set(struct inode *inode, int flag)
{
	return test_bit(flag, &F2FS_I(inode)->flags);
}