static inline pte_t gup_get_pte(pte_t *ptep)
{
	pte_t pte;

	do {
		pte.pte_low = ptep->pte_low;
		smp_rmb();
		pte.pte_high = ptep->pte_high;
		smp_rmb();
	} while (unlikely(pte.pte_low != ptep->pte_low));

	return pte;
}