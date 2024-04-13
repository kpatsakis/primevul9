static inline void set_pcppage_migratetype(struct page *page, int migratetype)
{
	page->index = migratetype;
}