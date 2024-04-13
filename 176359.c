static inline pte_t gup_get_pte(pte_t *ptep)
{
	return READ_ONCE(*ptep);
}