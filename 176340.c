static struct page *new_non_cma_page(struct page *page, unsigned long private)
{
	/*
	 * We want to make sure we allocate the new page from the same node
	 * as the source page.
	 */
	int nid = page_to_nid(page);
	/*
	 * Trying to allocate a page for migration. Ignore allocation
	 * failure warnings. We don't force __GFP_THISNODE here because
	 * this node here is the node where we have CMA reservation and
	 * in some case these nodes will have really less non movable
	 * allocation memory.
	 */
	gfp_t gfp_mask = GFP_USER | __GFP_NOWARN;

	if (PageHighMem(page))
		gfp_mask |= __GFP_HIGHMEM;

#ifdef CONFIG_HUGETLB_PAGE
	if (PageHuge(page)) {
		struct hstate *h = page_hstate(page);
		/*
		 * We don't want to dequeue from the pool because pool pages will
		 * mostly be from the CMA region.
		 */
		return alloc_migrate_huge_page(h, gfp_mask, nid, NULL);
	}
#endif
	if (PageTransHuge(page)) {
		struct page *thp;
		/*
		 * ignore allocation failure warnings
		 */
		gfp_t thp_gfpmask = GFP_TRANSHUGE | __GFP_NOWARN;

		/*
		 * Remove the movable mask so that we don't allocate from
		 * CMA area again.
		 */
		thp_gfpmask &= ~__GFP_MOVABLE;
		thp = __alloc_pages_node(nid, thp_gfpmask, HPAGE_PMD_ORDER);
		if (!thp)
			return NULL;
		prep_transhuge_page(thp);
		return thp;
	}

	return __alloc_pages_node(nid, gfp_mask, 0);
}