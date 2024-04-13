void unpin_user_pages_dirty_lock(struct page **pages, unsigned long npages,
				 bool make_dirty)
{
	unsigned long index;

	/*
	 * TODO: this can be optimized for huge pages: if a series of pages is
	 * physically contiguous and part of the same compound page, then a
	 * single operation to the head page should suffice.
	 */

	if (!make_dirty) {
		unpin_user_pages(pages, npages);
		return;
	}

	for (index = 0; index < npages; index++) {
		struct page *page = compound_head(pages[index]);
		/*
		 * Checking PageDirty at this point may race with
		 * clear_page_dirty_for_io(), but that's OK. Two key
		 * cases:
		 *
		 * 1) This code sees the page as already dirty, so it
		 * skips the call to set_page_dirty(). That could happen
		 * because clear_page_dirty_for_io() called
		 * page_mkclean(), followed by set_page_dirty().
		 * However, now the page is going to get written back,
		 * which meets the original intention of setting it
		 * dirty, so all is well: clear_page_dirty_for_io() goes
		 * on to call TestClearPageDirty(), and write the page
		 * back.
		 *
		 * 2) This code sees the page as clean, so it calls
		 * set_page_dirty(). The page stays dirty, despite being
		 * written back, so it gets written back again in the
		 * next writeback cycle. This is harmless.
		 */
		if (!PageDirty(page))
			set_page_dirty_lock(page);
		unpin_user_page(page);
	}
}