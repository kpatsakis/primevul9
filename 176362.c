static inline struct page *try_get_compound_head(struct page *page, int refs)
{
	struct page *head = compound_head(page);

	if (WARN_ON_ONCE(page_ref_count(head) < 0))
		return NULL;
	if (unlikely(!page_cache_add_speculative(head, refs)))
		return NULL;
	return head;
}