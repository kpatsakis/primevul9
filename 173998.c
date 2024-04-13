void set_pmd_migration_entry(struct page_vma_mapped_walk *pvmw,
		struct page *page)
{
	struct vm_area_struct *vma = pvmw->vma;
	struct mm_struct *mm = vma->vm_mm;
	unsigned long address = pvmw->address;
	pmd_t pmdval;
	swp_entry_t entry;
	pmd_t pmdswp;

	if (!(pvmw->pmd && !pvmw->pte))
		return;

	mmu_notifier_invalidate_range_start(mm, address,
			address + HPAGE_PMD_SIZE);

	flush_cache_range(vma, address, address + HPAGE_PMD_SIZE);
	pmdval = *pvmw->pmd;
	pmdp_invalidate(vma, address, pvmw->pmd);
	if (pmd_dirty(pmdval))
		set_page_dirty(page);
	entry = make_migration_entry(page, pmd_write(pmdval));
	pmdswp = swp_entry_to_pmd(entry);
	if (pmd_soft_dirty(pmdval))
		pmdswp = pmd_swp_mksoft_dirty(pmdswp);
	set_pmd_at(mm, address, pvmw->pmd, pmdswp);
	page_remove_rmap(page, true);
	put_page(page);

	mmu_notifier_invalidate_range_end(mm, address,
			address + HPAGE_PMD_SIZE);
}