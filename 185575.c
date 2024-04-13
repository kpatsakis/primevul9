static int soft_offline_huge_page(struct page *page, int flags)
{
	int ret;
	unsigned long pfn = page_to_pfn(page);
	struct page *hpage = compound_head(page);
	LIST_HEAD(pagelist);

	/*
	 * This double-check of PageHWPoison is to avoid the race with
	 * memory_failure(). See also comment in __soft_offline_page().
	 */
	lock_page(hpage);
	if (PageHWPoison(hpage)) {
		unlock_page(hpage);
		put_hwpoison_page(hpage);
		pr_info("soft offline: %#lx hugepage already poisoned\n", pfn);
		return -EBUSY;
	}
	unlock_page(hpage);

	ret = isolate_huge_page(hpage, &pagelist);
	/*
	 * get_any_page() and isolate_huge_page() takes a refcount each,
	 * so need to drop one here.
	 */
	put_hwpoison_page(hpage);
	if (!ret) {
		pr_info("soft offline: %#lx hugepage failed to isolate\n", pfn);
		return -EBUSY;
	}

	ret = migrate_pages(&pagelist, new_page, NULL, MPOL_MF_MOVE_ALL,
				MIGRATE_SYNC, MR_MEMORY_FAILURE);
	if (ret) {
		pr_info("soft offline: %#lx: migration failed %d, type %lx\n",
			pfn, ret, page->flags);
		/*
		 * We know that soft_offline_huge_page() tries to migrate
		 * only one hugepage pointed to by hpage, so we need not
		 * run through the pagelist here.
		 */
		putback_active_hugepage(hpage);
		if (ret > 0)
			ret = -EIO;
	} else {
		/* overcommit hugetlb page will be freed to buddy */
		if (PageHuge(page)) {
			set_page_hwpoison_huge_page(hpage);
			dequeue_hwpoisoned_huge_page(hpage);
			num_poisoned_pages_add(1 << compound_order(hpage));
		} else {
			SetPageHWPoison(page);
			num_poisoned_pages_inc();
		}
	}
	return ret;
}