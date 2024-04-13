static noinline int vmalloc_fault(unsigned long address)
{
	pgd_t *pgd, *pgd_k;
	p4d_t *p4d, *p4d_k;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	/* Make sure we are in vmalloc area: */
	if (!(address >= VMALLOC_START && address < VMALLOC_END))
		return -1;

	WARN_ON_ONCE(in_nmi());

	/*
	 * Copy kernel mappings over when needed. This can also
	 * happen within a race in page table update. In the later
	 * case just flush:
	 */
	pgd = (pgd_t *)__va(read_cr3_pa()) + pgd_index(address);
	pgd_k = pgd_offset_k(address);
	if (pgd_none(*pgd_k))
		return -1;

	if (pgtable_l5_enabled()) {
		if (pgd_none(*pgd)) {
			set_pgd(pgd, *pgd_k);
			arch_flush_lazy_mmu_mode();
		} else {
			BUG_ON(pgd_page_vaddr(*pgd) != pgd_page_vaddr(*pgd_k));
		}
	}

	/* With 4-level paging, copying happens on the p4d level. */
	p4d = p4d_offset(pgd, address);
	p4d_k = p4d_offset(pgd_k, address);
	if (p4d_none(*p4d_k))
		return -1;

	if (p4d_none(*p4d) && !pgtable_l5_enabled()) {
		set_p4d(p4d, *p4d_k);
		arch_flush_lazy_mmu_mode();
	} else {
		BUG_ON(p4d_pfn(*p4d) != p4d_pfn(*p4d_k));
	}

	BUILD_BUG_ON(CONFIG_PGTABLE_LEVELS < 4);

	pud = pud_offset(p4d, address);
	if (pud_none(*pud))
		return -1;

	if (pud_large(*pud))
		return 0;

	pmd = pmd_offset(pud, address);
	if (pmd_none(*pmd))
		return -1;

	if (pmd_large(*pmd))
		return 0;

	pte = pte_offset_kernel(pmd, address);
	if (!pte_present(*pte))
		return -1;

	return 0;
}