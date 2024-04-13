static inline bool __should_serialize_io(struct inode *inode,
					struct writeback_control *wbc)
{
	if (!S_ISREG(inode->i_mode))
		return false;
	if (IS_NOQUOTA(inode))
		return false;
	/* to avoid deadlock in path of data flush */
	if (F2FS_I(inode)->cp_task)
		return false;
	if (wbc->sync_mode != WB_SYNC_ALL)
		return true;
	if (get_dirty_pages(inode) >= SM_I(F2FS_I_SB(inode))->min_seq_blocks)
		return true;
	return false;
}