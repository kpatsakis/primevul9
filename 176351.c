bool __must_check try_grab_page(struct page *page, unsigned int flags)
{
	WARN_ON_ONCE((flags & (FOLL_GET | FOLL_PIN)) == (FOLL_GET | FOLL_PIN));

	if (flags & FOLL_GET)
		return try_get_page(page);
	else if (flags & FOLL_PIN) {
		int refs = 1;

		page = compound_head(page);

		if (WARN_ON_ONCE(page_ref_count(page) <= 0))
			return false;

		if (hpage_pincount_available(page))
			hpage_pincount_add(page, 1);
		else
			refs = GUP_PIN_COUNTING_BIAS;

		/*
		 * Similar to try_grab_compound_head(): even if using the
		 * hpage_pincount_add/_sub() routines, be sure to
		 * *also* increment the normal page refcount field at least
		 * once, so that the page really is pinned.
		 */
		page_ref_add(page, refs);

		mod_node_page_state(page_pgdat(page), NR_FOLL_PIN_ACQUIRED, 1);
	}

	return true;
}