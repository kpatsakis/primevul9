static inline int get_pcppage_migratetype(struct page *page)
{
	return page->index;
}