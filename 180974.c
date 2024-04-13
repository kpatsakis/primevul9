static unsigned long __init deferred_init_range(int nid, int zid,
						unsigned long start_pfn,
						unsigned long end_pfn)
{
	struct mminit_pfnnid_cache nid_init_state = { };
	unsigned long nr_pgmask = pageblock_nr_pages - 1;
	unsigned long free_base_pfn = 0;
	unsigned long nr_pages = 0;
	unsigned long nr_free = 0;
	struct page *page = NULL;
	unsigned long pfn;

	/*
	 * First we check if pfn is valid on architectures where it is possible
	 * to have holes within pageblock_nr_pages. On systems where it is not
	 * possible, this function is optimized out.
	 *
	 * Then, we check if a current large page is valid by only checking the
	 * validity of the head pfn.
	 *
	 * meminit_pfn_in_nid is checked on systems where pfns can interleave
	 * within a node: a pfn is between start and end of a node, but does not
	 * belong to this memory node.
	 *
	 * Finally, we minimize pfn page lookups and scheduler checks by
	 * performing it only once every pageblock_nr_pages.
	 *
	 * We do it in two loops: first we initialize struct page, than free to
	 * buddy allocator, becuse while we are freeing pages we can access
	 * pages that are ahead (computing buddy page in __free_one_page()).
	 */
	for (pfn = start_pfn; pfn < end_pfn; pfn++) {
		if (!pfn_valid_within(pfn))
			continue;
		if ((pfn & nr_pgmask) || pfn_valid(pfn)) {
			if (meminit_pfn_in_nid(pfn, nid, &nid_init_state)) {
				if (page && (pfn & nr_pgmask))
					page++;
				else
					page = pfn_to_page(pfn);
				__init_single_page(page, pfn, zid, nid);
				cond_resched();
			}
		}
	}

	page = NULL;
	for (pfn = start_pfn; pfn < end_pfn; pfn++) {
		if (!pfn_valid_within(pfn)) {
			nr_pages += __def_free(&nr_free, &free_base_pfn, &page);
		} else if (!(pfn & nr_pgmask) && !pfn_valid(pfn)) {
			nr_pages += __def_free(&nr_free, &free_base_pfn, &page);
		} else if (!meminit_pfn_in_nid(pfn, nid, &nid_init_state)) {
			nr_pages += __def_free(&nr_free, &free_base_pfn, &page);
		} else if (page && (pfn & nr_pgmask)) {
			page++;
			nr_free++;
		} else {
			nr_pages += __def_free(&nr_free, &free_base_pfn, &page);
			page = pfn_to_page(pfn);
			free_base_pfn = pfn;
			nr_free = 1;
			cond_resched();
		}
	}
	/* Free the last block of pages to allocator */
	nr_pages += __def_free(&nr_free, &free_base_pfn, &page);

	return nr_pages;
}