static bool bulkfree_pcp_prepare(struct page *page)
{
	return free_pages_check(page);
}