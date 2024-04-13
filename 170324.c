static void *__bpf_map_area_alloc(u64 size, int numa_node, bool mmapable)
{
	/* We really just want to fail instead of triggering OOM killer
	 * under memory pressure, therefore we set __GFP_NORETRY to kmalloc,
	 * which is used for lower order allocation requests.
	 *
	 * It has been observed that higher order allocation requests done by
	 * vmalloc with __GFP_NORETRY being set might fail due to not trying
	 * to reclaim memory from the page cache, thus we set
	 * __GFP_RETRY_MAYFAIL to avoid such situations.
	 */

	const gfp_t gfp = __GFP_NOWARN | __GFP_ZERO | __GFP_ACCOUNT;
	unsigned int flags = 0;
	unsigned long align = 1;
	void *area;

	if (size >= SIZE_MAX)
		return NULL;

	/* kmalloc()'ed memory can't be mmap()'ed */
	if (mmapable) {
		BUG_ON(!PAGE_ALIGNED(size));
		align = SHMLBA;
		flags = VM_USERMAP;
	} else if (size <= (PAGE_SIZE << PAGE_ALLOC_COSTLY_ORDER)) {
		area = kmalloc_node(size, gfp | GFP_USER | __GFP_NORETRY,
				    numa_node);
		if (area != NULL)
			return area;
	}

	return __vmalloc_node_range(size, align, VMALLOC_START, VMALLOC_END,
			gfp | GFP_KERNEL | __GFP_RETRY_MAYFAIL, PAGE_KERNEL,
			flags, numa_node, __builtin_return_address(0));
}