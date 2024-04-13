void __paginginit zero_resv_unavail(void)
{
	phys_addr_t start, end;
	unsigned long pfn;
	u64 i, pgcnt;

	/*
	 * Loop through ranges that are reserved, but do not have reported
	 * physical memory backing.
	 */
	pgcnt = 0;
	for_each_resv_unavail_range(i, &start, &end) {
		for (pfn = PFN_DOWN(start); pfn < PFN_UP(end); pfn++) {
			mm_zero_struct_page(pfn_to_page(pfn));
			pgcnt++;
		}
	}

	/*
	 * Struct pages that do not have backing memory. This could be because
	 * firmware is using some of this memory, or for some other reasons.
	 * Once memblock is changed so such behaviour is not allowed: i.e.
	 * list of "reserved" memory must be a subset of list of "memory", then
	 * this code can be removed.
	 */
	if (pgcnt)
		pr_info("Reserved but unavailable: %lld pages", pgcnt);
}