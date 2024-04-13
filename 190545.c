static inline int get_dirty_pages(struct inode *inode)
{
	return atomic_read(&F2FS_I(inode)->dirty_pages);
}