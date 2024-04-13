static int memory_failure_dev_pagemap(unsigned long pfn, int flags,
		struct dev_pagemap *pgmap)
{
	struct page *page = pfn_to_page(pfn);
	const bool unmap_success = true;
	unsigned long size = 0;
	struct to_kill *tk;
	LIST_HEAD(tokill);
	int rc = -EBUSY;
	loff_t start;
	dax_entry_t cookie;

	/*
	 * Prevent the inode from being freed while we are interrogating
	 * the address_space, typically this would be handled by
	 * lock_page(), but dax pages do not use the page lock. This
	 * also prevents changes to the mapping of this pfn until
	 * poison signaling is complete.
	 */
	cookie = dax_lock_page(page);
	if (!cookie)
		goto out;

	if (hwpoison_filter(page)) {
		rc = 0;
		goto unlock;
	}

	switch (pgmap->type) {
	case MEMORY_DEVICE_PRIVATE:
	case MEMORY_DEVICE_PUBLIC:
		/*
		 * TODO: Handle HMM pages which may need coordination
		 * with device-side memory.
		 */
		goto unlock;
	default:
		break;
	}

	/*
	 * Use this flag as an indication that the dax page has been
	 * remapped UC to prevent speculative consumption of poison.
	 */
	SetPageHWPoison(page);

	/*
	 * Unlike System-RAM there is no possibility to swap in a
	 * different physical page at a given virtual address, so all
	 * userspace consumption of ZONE_DEVICE memory necessitates
	 * SIGBUS (i.e. MF_MUST_KILL)
	 */
	flags |= MF_ACTION_REQUIRED | MF_MUST_KILL;
	collect_procs(page, &tokill, flags & MF_ACTION_REQUIRED);

	list_for_each_entry(tk, &tokill, nd)
		if (tk->size_shift)
			size = max(size, 1UL << tk->size_shift);
	if (size) {
		/*
		 * Unmap the largest mapping to avoid breaking up
		 * device-dax mappings which are constant size. The
		 * actual size of the mapping being torn down is
		 * communicated in siginfo, see kill_proc()
		 */
		start = (page->index << PAGE_SHIFT) & ~(size - 1);
		unmap_mapping_range(page->mapping, start, start + size, 0);
	}
	kill_procs(&tokill, flags & MF_MUST_KILL, !unmap_success, pfn, flags);
	rc = 0;
unlock:
	dax_unlock_page(page, cookie);
out:
	/* drop pgmap ref acquired in caller */
	put_dev_pagemap(pgmap);
	action_result(pfn, MF_MSG_DAX, rc ? MF_FAILED : MF_RECOVERED);
	return rc;
}