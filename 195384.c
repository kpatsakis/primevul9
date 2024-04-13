static void dump_pagetable(unsigned long address)
{
	pgd_t *base = __va(read_cr3_pa());
	pgd_t *pgd = base + pgd_index(address);
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	if (bad_address(pgd))
		goto bad;

	pr_info("PGD %lx ", pgd_val(*pgd));

	if (!pgd_present(*pgd))
		goto out;

	p4d = p4d_offset(pgd, address);
	if (bad_address(p4d))
		goto bad;

	pr_cont("P4D %lx ", p4d_val(*p4d));
	if (!p4d_present(*p4d) || p4d_large(*p4d))
		goto out;

	pud = pud_offset(p4d, address);
	if (bad_address(pud))
		goto bad;

	pr_cont("PUD %lx ", pud_val(*pud));
	if (!pud_present(*pud) || pud_large(*pud))
		goto out;

	pmd = pmd_offset(pud, address);
	if (bad_address(pmd))
		goto bad;

	pr_cont("PMD %lx ", pmd_val(*pmd));
	if (!pmd_present(*pmd) || pmd_large(*pmd))
		goto out;

	pte = pte_offset_kernel(pmd, address);
	if (bad_address(pte))
		goto bad;

	pr_cont("PTE %lx", pte_val(*pte));
out:
	pr_cont("\n");
	return;
bad:
	pr_info("BAD\n");
}