int unpoison_memory(unsigned long pfn)
{
	struct page *page;
	struct page *p;
	int freeit = 0;
	unsigned int nr_pages;
	static DEFINE_RATELIMIT_STATE(unpoison_rs, DEFAULT_RATELIMIT_INTERVAL,
					DEFAULT_RATELIMIT_BURST);

	if (!pfn_valid(pfn))
		return -ENXIO;

	p = pfn_to_page(pfn);
	page = compound_head(p);

	if (!PageHWPoison(p)) {
		unpoison_pr_info("Unpoison: Page was already unpoisoned %#lx\n",
				 pfn, &unpoison_rs);
		return 0;
	}

	if (page_count(page) > 1) {
		unpoison_pr_info("Unpoison: Someone grabs the hwpoison page %#lx\n",
				 pfn, &unpoison_rs);
		return 0;
	}

	if (page_mapped(page)) {
		unpoison_pr_info("Unpoison: Someone maps the hwpoison page %#lx\n",
				 pfn, &unpoison_rs);
		return 0;
	}

	if (page_mapping(page)) {
		unpoison_pr_info("Unpoison: the hwpoison page has non-NULL mapping %#lx\n",
				 pfn, &unpoison_rs);
		return 0;
	}

	/*
	 * unpoison_memory() can encounter thp only when the thp is being
	 * worked by memory_failure() and the page lock is not held yet.
	 * In such case, we yield to memory_failure() and make unpoison fail.
	 */
	if (!PageHuge(page) && PageTransHuge(page)) {
		unpoison_pr_info("Unpoison: Memory failure is now running on %#lx\n",
				 pfn, &unpoison_rs);
		return 0;
	}

	nr_pages = 1 << compound_order(page);

	if (!get_hwpoison_page(p)) {
		/*
		 * Since HWPoisoned hugepage should have non-zero refcount,
		 * race between memory failure and unpoison seems to happen.
		 * In such case unpoison fails and memory failure runs
		 * to the end.
		 */
		if (PageHuge(page)) {
			unpoison_pr_info("Unpoison: Memory failure is now running on free hugepage %#lx\n",
					 pfn, &unpoison_rs);
			return 0;
		}
		if (TestClearPageHWPoison(p))
			num_poisoned_pages_dec();
		unpoison_pr_info("Unpoison: Software-unpoisoned free page %#lx\n",
				 pfn, &unpoison_rs);
		return 0;
	}

	lock_page(page);
	/*
	 * This test is racy because PG_hwpoison is set outside of page lock.
	 * That's acceptable because that won't trigger kernel panic. Instead,
	 * the PG_hwpoison page will be caught and isolated on the entrance to
	 * the free buddy page pool.
	 */
	if (TestClearPageHWPoison(page)) {
		unpoison_pr_info("Unpoison: Software-unpoisoned page %#lx\n",
				 pfn, &unpoison_rs);
		num_poisoned_pages_sub(nr_pages);
		freeit = 1;
		if (PageHuge(page))
			clear_page_hwpoison_huge_page(page);
	}
	unlock_page(page);

	put_hwpoison_page(page);
	if (freeit && !(pfn == my_zero_pfn(0) && page_count(p) == 1))
		put_hwpoison_page(page);

	return 0;
}