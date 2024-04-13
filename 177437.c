static int memory_failure_hugetlb(unsigned long pfn, int flags)
{
	struct page *p = pfn_to_page(pfn);
	struct page *head = compound_head(p);
	int res;
	unsigned long page_flags;

	if (TestSetPageHWPoison(head)) {
		pr_err("Memory failure: %#lx: already hardware poisoned\n",
		       pfn);
		return 0;
	}

	num_poisoned_pages_inc();

	if (!(flags & MF_COUNT_INCREASED) && !get_hwpoison_page(p)) {
		/*
		 * Check "filter hit" and "race with other subpage."
		 */
		lock_page(head);
		if (PageHWPoison(head)) {
			if ((hwpoison_filter(p) && TestClearPageHWPoison(p))
			    || (p != head && TestSetPageHWPoison(head))) {
				num_poisoned_pages_dec();
				unlock_page(head);
				return 0;
			}
		}
		unlock_page(head);
		dissolve_free_huge_page(p);
		action_result(pfn, MF_MSG_FREE_HUGE, MF_DELAYED);
		return 0;
	}

	lock_page(head);
	page_flags = head->flags;

	if (!PageHWPoison(head)) {
		pr_err("Memory failure: %#lx: just unpoisoned\n", pfn);
		num_poisoned_pages_dec();
		unlock_page(head);
		put_hwpoison_page(head);
		return 0;
	}

	/*
	 * TODO: hwpoison for pud-sized hugetlb doesn't work right now, so
	 * simply disable it. In order to make it work properly, we need
	 * make sure that:
	 *  - conversion of a pud that maps an error hugetlb into hwpoison
	 *    entry properly works, and
	 *  - other mm code walking over page table is aware of pud-aligned
	 *    hwpoison entries.
	 */
	if (huge_page_size(page_hstate(head)) > PMD_SIZE) {
		action_result(pfn, MF_MSG_NON_PMD_HUGE, MF_IGNORED);
		res = -EBUSY;
		goto out;
	}

	if (!hwpoison_user_mappings(p, pfn, flags, &head)) {
		action_result(pfn, MF_MSG_UNMAP_FAILED, MF_IGNORED);
		res = -EBUSY;
		goto out;
	}

	res = identify_page_state(pfn, p, page_flags);
out:
	unlock_page(head);
	return res;
}