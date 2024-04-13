static inline struct f2fs_node *F2FS_NODE(struct page *page)
{
	return (struct f2fs_node *)page_address(page);
}