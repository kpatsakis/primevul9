void arch_exit_mmap(struct mm_struct *mm)
{
	destroy_pagetable_cache(mm);

	if (radix_enabled()) {
		/*
		 * Radix doesn't have a valid bit in the process table
		 * entries. However we know that at least P9 implementation
		 * will avoid caching an entry with an invalid RTS field,
		 * and 0 is invalid. So this will do.
		 *
		 * This runs before the "fullmm" tlb flush in exit_mmap,
		 * which does a RIC=2 tlbie to clear the process table
		 * entry. See the "fullmm" comments in tlb-radix.c.
		 *
		 * No barrier required here after the store because
		 * this process will do the invalidate, which starts with
		 * ptesync.
		 */
		process_tb[mm->context.id].prtb0 = 0;
	}
}