static void dump_pagetable(unsigned long address)
{
	pgd_t *base = __va(read_cr3_pa());
	pgd_t *pgd = &base[pgd_index(address)];
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

#ifdef CONFIG_X86_PAE
	pr_info("*pdpt = %016Lx ", pgd_val(*pgd));
	if (!low_pfn(pgd_val(*pgd) >> PAGE_SHIFT) || !pgd_present(*pgd))
		goto out;
#define pr_pde pr_cont
#else
#define pr_pde pr_info
#endif
	p4d = p4d_offset(pgd, address);
	pud = pud_offset(p4d, address);
	pmd = pmd_offset(pud, address);
	pr_pde("*pde = %0*Lx ", sizeof(*pmd) * 2, (u64)pmd_val(*pmd));
#undef pr_pde

	/*
	 * We must not directly access the pte in the highpte
	 * case if the page table is located in highmem.
	 * And let's rather not kmap-atomic the pte, just in case
	 * it's allocated already:
	 */
	if (!low_pfn(pmd_pfn(*pmd)) || !pmd_present(*pmd) || pmd_large(*pmd))
		goto out;

	pte = pte_offset_kernel(pmd, address);
	pr_cont("*pte = %0*Lx ", sizeof(*pte) * 2, (u64)pte_val(*pte));
out:
	pr_cont("\n");
}