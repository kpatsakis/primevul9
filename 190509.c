static inline void set_inode_flag(struct inode *inode, int flag)
{
	if (!test_bit(flag, &F2FS_I(inode)->flags))
		set_bit(flag, &F2FS_I(inode)->flags);
	__mark_inode_dirty_flag(inode, flag, true);
}