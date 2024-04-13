int change_huge_pmd(struct vm_area_struct *vma, pmd_t *pmd,
		unsigned long addr, pgprot_t newprot, int prot_numa)
{
	struct mm_struct *mm = vma->vm_mm;
	spinlock_t *ptl;
	pmd_t entry;
	bool preserve_write;
	int ret;

	ptl = __pmd_trans_huge_lock(pmd, vma);
	if (!ptl)
		return 0;

	preserve_write = prot_numa && pmd_write(*pmd);
	ret = 1;

#ifdef CONFIG_ARCH_ENABLE_THP_MIGRATION
	if (is_swap_pmd(*pmd)) {
		swp_entry_t entry = pmd_to_swp_entry(*pmd);

		VM_BUG_ON(!is_pmd_migration_entry(*pmd));
		if (is_write_migration_entry(entry)) {
			pmd_t newpmd;
			/*
			 * A protection check is difficult so
			 * just be safe and disable write
			 */
			make_migration_entry_read(&entry);
			newpmd = swp_entry_to_pmd(entry);
			if (pmd_swp_soft_dirty(*pmd))
				newpmd = pmd_swp_mksoft_dirty(newpmd);
			set_pmd_at(mm, addr, pmd, newpmd);
		}
		goto unlock;
	}
#endif

	/*
	 * Avoid trapping faults against the zero page. The read-only
	 * data is likely to be read-cached on the local CPU and
	 * local/remote hits to the zero page are not interesting.
	 */
	if (prot_numa && is_huge_zero_pmd(*pmd))
		goto unlock;

	if (prot_numa && pmd_protnone(*pmd))
		goto unlock;

	/*
	 * In case prot_numa, we are under down_read(mmap_sem). It's critical
	 * to not clear pmd intermittently to avoid race with MADV_DONTNEED
	 * which is also under down_read(mmap_sem):
	 *
	 *	CPU0:				CPU1:
	 *				change_huge_pmd(prot_numa=1)
	 *				 pmdp_huge_get_and_clear_notify()
	 * madvise_dontneed()
	 *  zap_pmd_range()
	 *   pmd_trans_huge(*pmd) == 0 (without ptl)
	 *   // skip the pmd
	 *				 set_pmd_at();
	 *				 // pmd is re-established
	 *
	 * The race makes MADV_DONTNEED miss the huge pmd and don't clear it
	 * which may break userspace.
	 *
	 * pmdp_invalidate() is required to make sure we don't miss
	 * dirty/young flags set by hardware.
	 */
	entry = *pmd;
	pmdp_invalidate(vma, addr, pmd);

	/*
	 * Recover dirty/young flags.  It relies on pmdp_invalidate to not
	 * corrupt them.
	 */
	if (pmd_dirty(*pmd))
		entry = pmd_mkdirty(entry);
	if (pmd_young(*pmd))
		entry = pmd_mkyoung(entry);

	entry = pmd_modify(entry, newprot);
	if (preserve_write)
		entry = pmd_mk_savedwrite(entry);
	ret = HPAGE_PMD_NR;
	set_pmd_at(mm, addr, pmd, entry);
	BUG_ON(vma_is_anonymous(vma) && !preserve_write && pmd_write(entry));
unlock:
	spin_unlock(ptl);
	return ret;
}