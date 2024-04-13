static int get_any_page(struct page *page, unsigned long pfn, int flags)
{
	int ret = __get_any_page(page, pfn, flags);

	if (ret == 1 && !PageHuge(page) &&
	    !PageLRU(page) && !__PageMovable(page)) {
		/*
		 * Try to free it.
		 */
		put_hwpoison_page(page);
		shake_page(page, 1);

		/*
		 * Did it turn free?
		 */
		ret = __get_any_page(page, pfn, 0);
		if (ret == 1 && !PageLRU(page)) {
			/* Drop page reference which is from __get_any_page() */
			put_hwpoison_page(page);
			pr_info("soft_offline: %#lx: unknown non LRU page type %lx (%pGp)\n",
				pfn, page->flags, &page->flags);
			return -EIO;
		}
	}
	return ret;
}