static inline pmd_t *vmalloc_sync_one(pgd_t *pgd, unsigned long address)
{
	unsigned index = pgd_index(address);
	pgd_t *pgd_k;
	p4d_t *p4d, *p4d_k;
	pud_t *pud, *pud_k;
	pmd_t *pmd, *pmd_k;

	pgd += index;
	pgd_k = init_mm.pgd + index;

	if (!pgd_present(*pgd_k))
		return NULL;

	/*
	 * set_pgd(pgd, *pgd_k); here would be useless on PAE
	 * and redundant with the set_pmd() on non-PAE. As would
	 * set_p4d/set_pud.
	 */
	p4d = p4d_offset(pgd, address);
	p4d_k = p4d_offset(pgd_k, address);
	if (!p4d_present(*p4d_k))
		return NULL;

	pud = pud_offset(p4d, address);
	pud_k = pud_offset(p4d_k, address);
	if (!pud_present(*pud_k))
		return NULL;

	pmd = pmd_offset(pud, address);
	pmd_k = pmd_offset(pud_k, address);
	if (!pmd_present(*pmd_k))
		return NULL;

	if (!pmd_present(*pmd))
		set_pmd(pmd, *pmd_k);
	else
		BUG_ON(pmd_page(*pmd) != pmd_page(*pmd_k));

	return pmd_k;
}