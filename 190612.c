static inline void f2fs_i_size_write(struct inode *inode, loff_t i_size)
{
	bool clean = !is_inode_flag_set(inode, FI_DIRTY_INODE);
	bool recover = is_inode_flag_set(inode, FI_AUTO_RECOVER);

	if (i_size_read(inode) == i_size)
		return;

	i_size_write(inode, i_size);
	f2fs_mark_inode_dirty_sync(inode, true);
	if (clean || recover)
		set_inode_flag(inode, FI_AUTO_RECOVER);
}