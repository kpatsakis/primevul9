static int soft_offline_in_use_page(struct page *page, int flags)
{
	int ret;
	int mt;
	struct page *hpage = compound_head(page);

	if (!PageHuge(page) && PageTransHuge(hpage)) {
		lock_page(page);
		if (!PageAnon(page) || unlikely(split_huge_page(page))) {
			unlock_page(page);
			if (!PageAnon(page))
				pr_info("soft offline: %#lx: non anonymous thp\n", page_to_pfn(page));
			else
				pr_info("soft offline: %#lx: thp split failed\n", page_to_pfn(page));
			put_hwpoison_page(page);
			return -EBUSY;
		}
		unlock_page(page);
	}

	/*
	 * Setting MIGRATE_ISOLATE here ensures that the page will be linked
	 * to free list immediately (not via pcplist) when released after
	 * successful page migration. Otherwise we can't guarantee that the
	 * page is really free after put_page() returns, so
	 * set_hwpoison_free_buddy_page() highly likely fails.
	 */
	mt = get_pageblock_migratetype(page);
	set_pageblock_migratetype(page, MIGRATE_ISOLATE);
	if (PageHuge(page))
		ret = soft_offline_huge_page(page, flags);
	else
		ret = __soft_offline_page(page, flags);
	set_pageblock_migratetype(page, mt);
	return ret;
}