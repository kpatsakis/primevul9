static inline bool userfaultfd_huge_must_wait(struct userfaultfd_ctx *ctx,
					 struct vm_area_struct *vma,
					 unsigned long address,
					 unsigned long flags,
					 unsigned long reason)
{
	struct mm_struct *mm = ctx->mm;
	pte_t *pte;
	bool ret = true;

	VM_BUG_ON(!rwsem_is_locked(&mm->mmap_sem));

	pte = huge_pte_offset(mm, address, vma_mmu_pagesize(vma));
	if (!pte)
		goto out;

	ret = false;

	/*
	 * Lockless access: we're in a wait_event so it's ok if it
	 * changes under us.
	 */
	if (huge_pte_none(*pte))
		ret = true;
	if (!huge_pte_write(*pte) && (reason & VM_UFFD_WP))
		ret = true;
out:
	return ret;
}