static int __soft_offline_page(struct page *page, int flags)
{
	int ret;
	unsigned long pfn = page_to_pfn(page);

	/*
	 * Check PageHWPoison again inside page lock because PageHWPoison
	 * is set by memory_failure() outside page lock. Note that
	 * memory_failure() also double-checks PageHWPoison inside page lock,
	 * so there's no race between soft_offline_page() and memory_failure().
	 */
	lock_page(page);
	wait_on_page_writeback(page);
	if (PageHWPoison(page)) {
		unlock_page(page);
		put_hwpoison_page(page);
		pr_info("soft offline: %#lx page already poisoned\n", pfn);
		return -EBUSY;
	}
	/*
	 * Try to invalidate first. This should work for
	 * non dirty unmapped page cache pages.
	 */
	ret = invalidate_inode_page(page);
	unlock_page(page);
	/*
	 * RED-PEN would be better to keep it isolated here, but we
	 * would need to fix isolation locking first.
	 */
	if (ret == 1) {
		put_hwpoison_page(page);
		pr_info("soft_offline: %#lx: invalidated\n", pfn);
		SetPageHWPoison(page);
		num_poisoned_pages_inc();
		return 0;
	}

	/*
	 * Simple invalidation didn't work.
	 * Try to migrate to a new page instead. migrate.c
	 * handles a large number of cases for us.
	 */
	if (PageLRU(page))
		ret = isolate_lru_page(page);
	else
		ret = isolate_movable_page(page, ISOLATE_UNEVICTABLE);
	/*
	 * Drop page reference which is came from get_any_page()
	 * successful isolate_lru_page() already took another one.
	 */
	put_hwpoison_page(page);
	if (!ret) {
		LIST_HEAD(pagelist);
		/*
		 * After isolated lru page, the PageLRU will be cleared,
		 * so use !__PageMovable instead for LRU page's mapping
		 * cannot have PAGE_MAPPING_MOVABLE.
		 */
		if (!__PageMovable(page))
			inc_node_page_state(page, NR_ISOLATED_ANON +
						page_is_file_cache(page));
		list_add(&page->lru, &pagelist);
		ret = migrate_pages(&pagelist, new_page, NULL, MPOL_MF_MOVE_ALL,
					MIGRATE_SYNC, MR_MEMORY_FAILURE);
		if (ret) {
			if (!list_empty(&pagelist))
				putback_movable_pages(&pagelist);

			pr_info("soft offline: %#lx: migration failed %d, type %lx (%pGp)\n",
				pfn, ret, page->flags, &page->flags);
			if (ret > 0)
				ret = -EIO;
		}
	} else {
		pr_info("soft offline: %#lx: isolation failed: %d, page count %d, type %lx (%pGp)\n",
			pfn, ret, page_count(page), page->flags, &page->flags);
	}
	return ret;
}