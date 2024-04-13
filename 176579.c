static inline pud_t *pud_alloc_one(struct mm_struct *mm, unsigned long address)
{
	unsigned long *table = crst_table_alloc(mm);
	if (table)
		crst_table_init(table, _REGION3_ENTRY_EMPTY);
	return (pud_t *) table;
}