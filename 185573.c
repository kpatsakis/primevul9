static int me_pagecache_clean(struct page *p, unsigned long pfn)
{
	int err;
	int ret = MF_FAILED;
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
	if (mapping->a_ops->error_remove_page) {
		err = mapping->a_ops->error_remove_page(mapping, p);
		if (err != 0) {
			pr_info("Memory failure: %#lx: Failed to punch page: %d\n",
				pfn, err);
		} else if (page_has_private(p) &&
				!try_to_release_page(p, GFP_NOIO)) {
			pr_info("Memory failure: %#lx: failed to release buffers\n",
				pfn);
		} else {
			ret = MF_RECOVERED;
		}
	} else {
		/*
		 * If the file system doesn't support it just invalidate
		 * This fails on dirty or anything with private pages
		 */
		if (invalidate_inode_page(p))
			ret = MF_RECOVERED;
		else
			pr_info("Memory failure: %#lx: Failed to invalidate\n",
				pfn);
	}
	return ret;
}