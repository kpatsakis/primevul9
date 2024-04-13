static void mce_unmap_kpfn(unsigned long pfn)
{
	unsigned long decoy_addr;

	/*
	 * Unmap this page from the kernel 1:1 mappings to make sure
	 * we don't log more errors because of speculative access to
	 * the page.
	 * We would like to just call:
	 *	set_memory_np((unsigned long)pfn_to_kaddr(pfn), 1);
	 * but doing that would radically increase the odds of a
	 * speculative access to the poison page because we'd have
	 * the virtual address of the kernel 1:1 mapping sitting
	 * around in registers.
	 * Instead we get tricky.  We create a non-canonical address
	 * that looks just like the one we want, but has bit 63 flipped.
	 * This relies on set_memory_np() not checking whether we passed
	 * a legal address.
	 */

/*
 * Build time check to see if we have a spare virtual bit. Don't want
 * to leave this until run time because most developers don't have a
 * system that can exercise this code path. This will only become a
 * problem if/when we move beyond 5-level page tables.
 *
 * Hard code "9" here because cpp doesn't grok ilog2(PTRS_PER_PGD)
 */
#if PGDIR_SHIFT + 9 < 63
	decoy_addr = (pfn << PAGE_SHIFT) + (PAGE_OFFSET ^ BIT(63));
#else
#error "no unused virtual bit available"
#endif

	if (set_memory_np(decoy_addr, 1))
		pr_warn("Could not invalidate pfn=0x%lx from 1:1 map\n", pfn);
}