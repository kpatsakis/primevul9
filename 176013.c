static inline bool userfaultfd_must_wait(struct userfaultfd_ctx *ctx,
					 unsigned long address,
					 unsigned long flags,
					 unsigned long reason)
{
	struct mm_struct *mm = ctx->mm;
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd, _pmd;
	pte_t *pte;
	bool ret = true;

	VM_BUG_ON(!rwsem_is_locked(&mm->mmap_sem));

	pgd = pgd_offset(mm, address);
	if (!pgd_present(*pgd))
		goto out;
	p4d = p4d_offset(pgd, address);
	if (!p4d_present(*p4d))
		goto out;
	pud = pud_offset(p4d, address);
	if (!pud_present(*pud))
		goto out;
	pmd = pmd_offset(pud, address);
	/*
	 * READ_ONCE must function as a barrier with narrower scope
	 * and it must be equivalent to:
	 *	_pmd = *pmd; barrier();
	 *
	 * This is to deal with the instability (as in
	 * pmd_trans_unstable) of the pmd.
	 */
	_pmd = READ_ONCE(*pmd);
	if (!pmd_present(_pmd))
		goto out;

	ret = false;
	if (pmd_trans_huge(_pmd))
		goto out;

	/*
	 * the pmd is stable (as in !pmd_trans_unstable) so we can re-read it
	 * and use the standard pte_offset_map() instead of parsing _pmd.
	 */
	pte = pte_offset_map(pmd, address);
	/*
	 * Lockless access: we're in a wait_event so it's ok if it
	 * changes under us.
	 */
	if (pte_none(*pte))
		ret = true;
	pte_unmap(pte);

out:
	return ret;
}