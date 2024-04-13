static inline void f2fs_i_gc_failures_write(struct inode *inode,
					unsigned int count)
{
	F2FS_I(inode)->i_gc_failures[GC_FAILURE_PIN] = count;
	f2fs_mark_inode_dirty_sync(inode, true);
}