static noinline int vmalloc_fault(unsigned long address)
{
	unsigned long pgd_paddr;
	pmd_t *pmd_k;
	pte_t *pte_k;

	/* Make sure we are in vmalloc area: */
	if (!(address >= VMALLOC_START && address < VMALLOC_END))
		return -1;

	/*
	 * Synchronize this task's top level page-table
	 * with the 'reference' page table.
	 *
	 * Do _not_ use "current" here. We might be inside
	 * an interrupt in the middle of a task switch..
	 */
	pgd_paddr = read_cr3_pa();
	pmd_k = vmalloc_sync_one(__va(pgd_paddr), address);
	if (!pmd_k)
		return -1;

	if (pmd_large(*pmd_k))
		return 0;

	pte_k = pte_offset_kernel(pmd_k, address);
	if (!pte_present(*pte_k))
		return -1;

	return 0;
}