static void __split_huge_pmd_locked(struct vm_area_struct *vma, pmd_t *pmd,
		unsigned long haddr, bool freeze)
{
	struct mm_struct *mm = vma->vm_mm;
	struct page *page;
	pgtable_t pgtable;
	pmd_t _pmd;
	bool young, write, dirty, soft_dirty, pmd_migration = false;
	unsigned long addr;
	int i;

	VM_BUG_ON(haddr & ~HPAGE_PMD_MASK);
	VM_BUG_ON_VMA(vma->vm_start > haddr, vma);
	VM_BUG_ON_VMA(vma->vm_end < haddr + HPAGE_PMD_SIZE, vma);
	VM_BUG_ON(!is_pmd_migration_entry(*pmd) && !pmd_trans_huge(*pmd)
				&& !pmd_devmap(*pmd));

	count_vm_event(THP_SPLIT_PMD);

	if (!vma_is_anonymous(vma)) {
		_pmd = pmdp_huge_clear_flush_notify(vma, haddr, pmd);
		/*
		 * We are going to unmap this huge page. So
		 * just go ahead and zap it
		 */
		if (arch_needs_pgtable_deposit())
			zap_deposited_table(mm, pmd);
		if (vma_is_dax(vma))
			return;
		page = pmd_page(_pmd);
		if (!PageReferenced(page) && pmd_young(_pmd))
			SetPageReferenced(page);
		page_remove_rmap(page, true);
		put_page(page);
		add_mm_counter(mm, MM_FILEPAGES, -HPAGE_PMD_NR);
		return;
	} else if (is_huge_zero_pmd(*pmd)) {
		/*
		 * FIXME: Do we want to invalidate secondary mmu by calling
		 * mmu_notifier_invalidate_range() see comments below inside
		 * __split_huge_pmd() ?
		 *
		 * We are going from a zero huge page write protected to zero
		 * small page also write protected so it does not seems useful
		 * to invalidate secondary mmu at this time.
		 */
		return __split_huge_zero_page_pmd(vma, haddr, pmd);
	}

#ifdef CONFIG_ARCH_ENABLE_THP_MIGRATION
	pmd_migration = is_pmd_migration_entry(*pmd);
	if (pmd_migration) {
		swp_entry_t entry;

		entry = pmd_to_swp_entry(*pmd);
		page = pfn_to_page(swp_offset(entry));
	} else
#endif
		page = pmd_page(*pmd);
	VM_BUG_ON_PAGE(!page_count(page), page);
	page_ref_add(page, HPAGE_PMD_NR - 1);
	write = pmd_write(*pmd);
	young = pmd_young(*pmd);
	dirty = pmd_dirty(*pmd);
	soft_dirty = pmd_soft_dirty(*pmd);

	pmdp_huge_split_prepare(vma, haddr, pmd);
	pgtable = pgtable_trans_huge_withdraw(mm, pmd);
	pmd_populate(mm, &_pmd, pgtable);

	for (i = 0, addr = haddr; i < HPAGE_PMD_NR; i++, addr += PAGE_SIZE) {
		pte_t entry, *pte;
		/*
		 * Note that NUMA hinting access restrictions are not
		 * transferred to avoid any possibility of altering
		 * permissions across VMAs.
		 */
		if (freeze || pmd_migration) {
			swp_entry_t swp_entry;
			swp_entry = make_migration_entry(page + i, write);
			entry = swp_entry_to_pte(swp_entry);
			if (soft_dirty)
				entry = pte_swp_mksoft_dirty(entry);
		} else {
			entry = mk_pte(page + i, READ_ONCE(vma->vm_page_prot));
			entry = maybe_mkwrite(entry, vma);
			if (!write)
				entry = pte_wrprotect(entry);
			if (!young)
				entry = pte_mkold(entry);
			if (soft_dirty)
				entry = pte_mksoft_dirty(entry);
		}
		if (dirty)
			SetPageDirty(page + i);
		pte = pte_offset_map(&_pmd, addr);
		BUG_ON(!pte_none(*pte));
		set_pte_at(mm, addr, pte, entry);
		atomic_inc(&page[i]._mapcount);
		pte_unmap(pte);
	}

	/*
	 * Set PG_double_map before dropping compound_mapcount to avoid
	 * false-negative page_mapped().
	 */
	if (compound_mapcount(page) > 1 && !TestSetPageDoubleMap(page)) {
		for (i = 0; i < HPAGE_PMD_NR; i++)
			atomic_inc(&page[i]._mapcount);
	}

	if (atomic_add_negative(-1, compound_mapcount_ptr(page))) {
		/* Last compound_mapcount is gone. */
		__dec_node_page_state(page, NR_ANON_THPS);
		if (TestClearPageDoubleMap(page)) {
			/* No need in mapcount reference anymore */
			for (i = 0; i < HPAGE_PMD_NR; i++)
				atomic_dec(&page[i]._mapcount);
		}
	}

	smp_wmb(); /* make pte visible before pmd */
	/*
	 * Up to this point the pmd is present and huge and userland has the
	 * whole access to the hugepage during the split (which happens in
	 * place). If we overwrite the pmd with the not-huge version pointing
	 * to the pte here (which of course we could if all CPUs were bug
	 * free), userland could trigger a small page size TLB miss on the
	 * small sized TLB while the hugepage TLB entry is still established in
	 * the huge TLB. Some CPU doesn't like that.
	 * See http://support.amd.com/us/Processor_TechDocs/41322.pdf, Erratum
	 * 383 on page 93. Intel should be safe but is also warns that it's
	 * only safe if the permission and cache attributes of the two entries
	 * loaded in the two TLB is identical (which should be the case here).
	 * But it is generally safer to never allow small and huge TLB entries
	 * for the same virtual address to be loaded simultaneously. So instead
	 * of doing "pmd_populate(); flush_pmd_tlb_range();" we first mark the
	 * current pmd notpresent (atomically because here the pmd_trans_huge
	 * and pmd_trans_splitting must remain set at all times on the pmd
	 * until the split is complete for this pmd), then we flush the SMP TLB
	 * and finally we write the non-huge version of the pmd entry with
	 * pmd_populate.
	 */
	pmdp_invalidate(vma, haddr, pmd);
	pmd_populate(mm, pmd, pgtable);

	if (freeze) {
		for (i = 0; i < HPAGE_PMD_NR; i++) {
			page_remove_rmap(page + i, false);
			put_page(page + i);
		}
	}
}