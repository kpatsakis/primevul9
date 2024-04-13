static inline void pmd_populate(struct mm_struct *mm,
				pmd_t *pmd, pgtable_t pte)
{
	pmd_val(*pmd) = _SEGMENT_ENTRY + __pa(pte);
}