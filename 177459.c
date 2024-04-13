int memory_failure(unsigned long pfn, int flags)
{
	struct page *p;
	struct page *hpage;
	struct page *orig_head;
	struct dev_pagemap *pgmap;
	int res;
	unsigned long page_flags;

	if (!sysctl_memory_failure_recovery)
		panic("Memory failure on page %lx", pfn);

	if (!pfn_valid(pfn)) {
		pr_err("Memory failure: %#lx: memory outside kernel control\n",
			pfn);
		return -ENXIO;
	}

	pgmap = get_dev_pagemap(pfn, NULL);
	if (pgmap)
		return memory_failure_dev_pagemap(pfn, flags, pgmap);

	p = pfn_to_page(pfn);
	if (PageHuge(p))
		return memory_failure_hugetlb(pfn, flags);
	if (TestSetPageHWPoison(p)) {
		pr_err("Memory failure: %#lx: already hardware poisoned\n",
			pfn);
		return 0;
	}

	orig_head = hpage = compound_head(p);
	num_poisoned_pages_inc();

	/*
	 * We need/can do nothing about count=0 pages.
	 * 1) it's a free page, and therefore in safe hand:
	 *    prep_new_page() will be the gate keeper.
	 * 2) it's part of a non-compound high order page.
	 *    Implies some kernel user: cannot stop them from
	 *    R/W the page; let's pray that the page has been
	 *    used and will be freed some time later.
	 * In fact it's dangerous to directly bump up page count from 0,
	 * that may make page_ref_freeze()/page_ref_unfreeze() mismatch.
	 */
	if (!(flags & MF_COUNT_INCREASED) && !get_hwpoison_page(p)) {
		if (is_free_buddy_page(p)) {
			action_result(pfn, MF_MSG_BUDDY, MF_DELAYED);
			return 0;
		} else {
			action_result(pfn, MF_MSG_KERNEL_HIGH_ORDER, MF_IGNORED);
			return -EBUSY;
		}
	}

	if (PageTransHuge(hpage)) {
		lock_page(p);
		if (!PageAnon(p) || unlikely(split_huge_page(p))) {
			unlock_page(p);
			if (!PageAnon(p))
				pr_err("Memory failure: %#lx: non anonymous thp\n",
					pfn);
			else
				pr_err("Memory failure: %#lx: thp split failed\n",
					pfn);
			if (TestClearPageHWPoison(p))
				num_poisoned_pages_dec();
			put_hwpoison_page(p);
			return -EBUSY;
		}
		unlock_page(p);
		VM_BUG_ON_PAGE(!page_count(p), p);
		hpage = compound_head(p);
	}

	/*
	 * We ignore non-LRU pages for good reasons.
	 * - PG_locked is only well defined for LRU pages and a few others
	 * - to avoid races with __SetPageLocked()
	 * - to avoid races with __SetPageSlab*() (and more non-atomic ops)
	 * The check (unnecessarily) ignores LRU pages being isolated and
	 * walked by the page reclaim code, however that's not a big loss.
	 */
	shake_page(p, 0);
	/* shake_page could have turned it free. */
	if (!PageLRU(p) && is_free_buddy_page(p)) {
		if (flags & MF_COUNT_INCREASED)
			action_result(pfn, MF_MSG_BUDDY, MF_DELAYED);
		else
			action_result(pfn, MF_MSG_BUDDY_2ND, MF_DELAYED);
		return 0;
	}

	lock_page(p);

	/*
	 * The page could have changed compound pages during the locking.
	 * If this happens just bail out.
	 */
	if (PageCompound(p) && compound_head(p) != orig_head) {
		action_result(pfn, MF_MSG_DIFFERENT_COMPOUND, MF_IGNORED);
		res = -EBUSY;
		goto out;
	}

	/*
	 * We use page flags to determine what action should be taken, but
	 * the flags can be modified by the error containment action.  One
	 * example is an mlocked page, where PG_mlocked is cleared by
	 * page_remove_rmap() in try_to_unmap_one(). So to determine page status
	 * correctly, we save a copy of the page flags at this time.
	 */
	if (PageHuge(p))
		page_flags = hpage->flags;
	else
		page_flags = p->flags;

	/*
	 * unpoison always clear PG_hwpoison inside page lock
	 */
	if (!PageHWPoison(p)) {
		pr_err("Memory failure: %#lx: just unpoisoned\n", pfn);
		num_poisoned_pages_dec();
		unlock_page(p);
		put_hwpoison_page(p);
		return 0;
	}
	if (hwpoison_filter(p)) {
		if (TestClearPageHWPoison(p))
			num_poisoned_pages_dec();
		unlock_page(p);
		put_hwpoison_page(p);
		return 0;
	}

	if (!PageTransTail(p) && !PageLRU(p))
		goto identify_page_state;

	/*
	 * It's very difficult to mess with pages currently under IO
	 * and in many cases impossible, so we just avoid it here.
	 */
	wait_on_page_writeback(p);

	/*
	 * Now take care of user space mappings.
	 * Abort on fail: __delete_from_page_cache() assumes unmapped page.
	 *
	 * When the raw error page is thp tail page, hpage points to the raw
	 * page after thp split.
	 */
	if (!hwpoison_user_mappings(p, pfn, flags, &hpage)) {
		action_result(pfn, MF_MSG_UNMAP_FAILED, MF_IGNORED);
		res = -EBUSY;
		goto out;
	}

	/*
	 * Torn down by someone else?
	 */
	if (PageLRU(p) && !PageSwapCache(p) && p->mapping == NULL) {
		action_result(pfn, MF_MSG_TRUNCATED_LRU, MF_IGNORED);
		res = -EBUSY;
		goto out;
	}

identify_page_state:
	res = identify_page_state(pfn, p, page_flags);
out:
	unlock_page(p);
	return res;
}