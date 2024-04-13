static int ttm_set_pages_caching(struct page **pages,
		enum ttm_caching_state cstate, unsigned cpages)
{
	int r = 0;
	/* Set page caching */
	switch (cstate) {
	case tt_uncached:
		r = ttm_set_pages_array_uc(pages, cpages);
		if (r)
			pr_err("Failed to set %d pages to uc!\n", cpages);
		break;
	case tt_wc:
		r = ttm_set_pages_array_wc(pages, cpages);
		if (r)
			pr_err("Failed to set %d pages to wc!\n", cpages);
		break;
	default:
		break;
	}
	return r;
}