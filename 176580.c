static inline void pmd_free(struct mm_struct *mm, pmd_t *pmd)
{
	pgtable_pmd_page_dtor(virt_to_page(pmd));
	crst_table_free(mm, (unsigned long *) pmd);
}