static inline void f2fs_i_depth_write(struct inode *inode, unsigned int depth)
{
	F2FS_I(inode)->i_current_depth = depth;
	f2fs_mark_inode_dirty_sync(inode, true);
}