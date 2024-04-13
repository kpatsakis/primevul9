static inline void rmv_page_order(struct page *page)
{
	__ClearPageBuddy(page);
	set_page_private(page, 0);
}