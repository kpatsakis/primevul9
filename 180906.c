static inline void set_page_order(struct page *page, unsigned int order)
{
	set_page_private(page, order);
	__SetPageBuddy(page);
}