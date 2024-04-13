static bool should_fail_alloc_page(gfp_t gfp_mask, unsigned int order)
{
	if (order < fail_page_alloc.min_order)
		return false;
	if (gfp_mask & __GFP_NOFAIL)
		return false;
	if (fail_page_alloc.ignore_gfp_highmem && (gfp_mask & __GFP_HIGHMEM))
		return false;
	if (fail_page_alloc.ignore_gfp_reclaim &&
			(gfp_mask & __GFP_DIRECT_RECLAIM))
		return false;

	return should_fail(&fail_page_alloc.attr, 1 << order);
}