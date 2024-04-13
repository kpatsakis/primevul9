int zap_huge_pmd(struct mmu_gather *tlb, struct vm_area_struct *vma,
		 pmd_t *pmd, unsigned long addr)
{
	pmd_t orig_pmd;
	spinlock_t *ptl;

	tlb_remove_check_page_size_change(tlb, HPAGE_PMD_SIZE);

	ptl = __pmd_trans_huge_lock(pmd, vma);
	if (!ptl)
		return 0;
	/*
	 * For architectures like ppc64 we look at deposited pgtable
	 * when calling pmdp_huge_get_and_clear. So do the
	 * pgtable_trans_huge_withdraw after finishing pmdp related
	 * operations.
	 */
	orig_pmd = pmdp_huge_get_and_clear_full(tlb->mm, addr, pmd,
			tlb->fullmm);
	tlb_remove_pmd_tlb_entry(tlb, pmd, addr);
	if (vma_is_dax(vma)) {
		if (arch_needs_pgtable_deposit())
			zap_deposited_table(tlb->mm, pmd);
		spin_unlock(ptl);
		if (is_huge_zero_pmd(orig_pmd))
			tlb_remove_page_size(tlb, pmd_page(orig_pmd), HPAGE_PMD_SIZE);
	} else if (is_huge_zero_pmd(orig_pmd)) {
		zap_deposited_table(tlb->mm, pmd);
		spin_unlock(ptl);
		tlb_remove_page_size(tlb, pmd_page(orig_pmd), HPAGE_PMD_SIZE);
	} else {
		struct page *page = NULL;
		int flush_needed = 1;

		if (pmd_present(orig_pmd)) {
			page = pmd_page(orig_pmd);
			page_remove_rmap(page, true);
			VM_BUG_ON_PAGE(page_mapcount(page) < 0, page);
			VM_BUG_ON_PAGE(!PageHead(page), page);
		} else if (thp_migration_supported()) {
			swp_entry_t entry;

			VM_BUG_ON(!is_pmd_migration_entry(orig_pmd));
			entry = pmd_to_swp_entry(orig_pmd);
			page = pfn_to_page(swp_offset(entry));
			flush_needed = 0;
		} else
			WARN_ONCE(1, "Non present huge pmd without pmd migration enabled!");

		if (PageAnon(page)) {
			zap_deposited_table(tlb->mm, pmd);
			add_mm_counter(tlb->mm, MM_ANONPAGES, -HPAGE_PMD_NR);
		} else {
			if (arch_needs_pgtable_deposit())
				zap_deposited_table(tlb->mm, pmd);
			add_mm_counter(tlb->mm, MM_FILEPAGES, -HPAGE_PMD_NR);
		}

		spin_unlock(ptl);
		if (flush_needed)
			tlb_remove_page_size(tlb, page, HPAGE_PMD_SIZE);
	}
	return 1;
}