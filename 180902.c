static inline bool free_pcp_prepare(struct page *page)
{
	return free_pages_prepare(page, 0, true);
}