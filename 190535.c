static inline struct f2fs_inode *F2FS_INODE(struct page *page)
{
	return &((struct f2fs_node *)page_address(page))->i;
}