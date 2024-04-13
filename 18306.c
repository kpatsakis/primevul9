static inline bool page_copy_sane(struct page *page, size_t offset, size_t n)
{
	struct page *head;
	size_t v = n + offset;

	/*
	 * The general case needs to access the page order in order
	 * to compute the page size.
	 * However, we mostly deal with order-0 pages and thus can
	 * avoid a possible cache line miss for requests that fit all
	 * page orders.
	 */
	if (n <= v && v <= PAGE_SIZE)
		return true;

	head = compound_head(page);
	v += (page - head) << PAGE_SHIFT;

	if (likely(n <= v && v <= (page_size(head))))
		return true;
	WARN_ON(1);
	return false;
}