static __maybe_unused struct page *try_grab_compound_head(struct page *page,
							  int refs,
							  unsigned int flags)
{
	if (flags & FOLL_GET)
		return try_get_compound_head(page, refs);
	else if (flags & FOLL_PIN) {
		int orig_refs = refs;

		/*
		 * Can't do FOLL_LONGTERM + FOLL_PIN with CMA in the gup fast
		 * path, so fail and let the caller fall back to the slow path.
		 */
		if (unlikely(flags & FOLL_LONGTERM) &&
				is_migrate_cma_page(page))
			return NULL;

		/*
		 * When pinning a compound page of order > 1 (which is what
		 * hpage_pincount_available() checks for), use an exact count to
		 * track it, via hpage_pincount_add/_sub().
		 *
		 * However, be sure to *also* increment the normal page refcount
		 * field at least once, so that the page really is pinned.
		 */
		if (!hpage_pincount_available(page))
			refs *= GUP_PIN_COUNTING_BIAS;

		page = try_get_compound_head(page, refs);
		if (!page)
			return NULL;

		if (hpage_pincount_available(page))
			hpage_pincount_add(page, refs);

		mod_node_page_state(page_pgdat(page), NR_FOLL_PIN_ACQUIRED,
				    orig_refs);

		return page;
	}

	WARN_ON_ONCE(1);
	return NULL;
}