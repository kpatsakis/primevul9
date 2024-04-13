static inline void *inline_data_addr(struct inode *inode, struct page *page)
{
	struct f2fs_inode *ri = F2FS_INODE(page);
	int extra_size = get_extra_isize(inode);

	return (void *)&(ri->i_addr[extra_size + DEF_INLINE_RESERVED_SIZE]);
}