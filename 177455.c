static int me_pagecache_clean(struct page *p, unsigned long pfn)
{
	struct address_space *mapping;

	delete_from_lru_cache(p);

	/*
	 * For anonymous pages we're done the only reference left
	 * should be the one m_f() holds.
	 */
	if (PageAnon(p))
		return MF_RECOVERED;

	/*
	 * Now truncate the page in the page cache. This is really
	 * more like a "temporary hole punch"
	 * Don't do this for block devices when someone else
	 * has a reference, because it could be file system metadata
	 * and that's not safe to truncate.
	 */
	mapping = page_mapping(p);
	if (!mapping) {
		/*
		 * Page has been teared down in the meanwhile
		 */
		return MF_FAILED;
	}

	/*
	 * Truncation is a bit tricky. Enable it per file system for now.
	 *
	 * Open: to take i_mutex or not for this? Right now we don't.
	 */
	return truncate_error_page(p, pfn, mapping);
}