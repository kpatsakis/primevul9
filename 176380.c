static void put_compound_head(struct page *page, int refs, unsigned int flags)
{
	if (flags & FOLL_PIN) {
		mod_node_page_state(page_pgdat(page), NR_FOLL_PIN_RELEASED,
				    refs);

		if (hpage_pincount_available(page))
			hpage_pincount_sub(page, refs);
		else
			refs *= GUP_PIN_COUNTING_BIAS;
	}

	VM_BUG_ON_PAGE(page_ref_count(page) < refs, page);
	/*
	 * Calling put_page() for each ref is unnecessarily slow. Only the last
	 * ref needs a put_page().
	 */
	if (refs > 1)
		page_ref_sub(page, refs - 1);
	put_page(page);
}