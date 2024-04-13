static inline void inode_inc_dirty_pages(struct inode *inode)
{
	atomic_inc(&F2FS_I(inode)->dirty_pages);
	inc_page_count(F2FS_I_SB(inode), S_ISDIR(inode->i_mode) ?
				F2FS_DIRTY_DENTS : F2FS_DIRTY_DATA);
	if (IS_NOQUOTA(inode))
		inc_page_count(F2FS_I_SB(inode), F2FS_DIRTY_QDATA);
}