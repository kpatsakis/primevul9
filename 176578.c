static inline pmd_t *pmd_alloc_one(struct mm_struct *mm, unsigned long vmaddr)
{
	unsigned long *table = crst_table_alloc(mm);

	if (!table)
		return NULL;
	crst_table_init(table, _SEGMENT_ENTRY_EMPTY);
	if (!pgtable_pmd_page_ctor(virt_to_page(table))) {
		crst_table_free(mm, table);
		return NULL;
	}
	return (pmd_t *) table;
}