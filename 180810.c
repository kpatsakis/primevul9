static inline int free_pages_check(struct page *page)
{
	if (likely(page_expected_state(page, PAGE_FLAGS_CHECK_AT_FREE)))
		return 0;

	/* Something has gone sideways, find it */
	free_pages_check_bad(page);
	return 1;
}