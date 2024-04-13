static inline void clear_file(struct inode *inode, int type)
{
	F2FS_I(inode)->i_advise &= ~type;
	f2fs_mark_inode_dirty_sync(inode, true);
}