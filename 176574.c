static inline void pgd_populate(struct mm_struct *mm, pgd_t *pgd, pud_t *pud)
{
	pgd_val(*pgd) = _REGION2_ENTRY | __pa(pud);
}