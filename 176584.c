static inline void pgd_free(struct mm_struct *mm, pgd_t *pgd)
{
	if (mm->context.asce_limit == (1UL << 31))
		pgtable_pmd_page_dtor(virt_to_page(pgd));
	crst_table_free(mm, (unsigned long *) pgd);
}