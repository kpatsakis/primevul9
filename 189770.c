static int clear_refs_pte_range(pmd_t *pmd, unsigned long addr,
				unsigned long end, void *private)
{
	struct vm_area_struct *vma = private;
	pte_t *pte, ptent;
	spinlock_t *ptl;
	struct page *page;

	pte = pte_offset_map_lock(vma->vm_mm, pmd, addr, &ptl);
	for (; addr != end; pte++, addr += PAGE_SIZE) {
		ptent = *pte;
		if (!pte_present(ptent))
			continue;

		page = vm_normal_page(vma, addr, ptent);
		if (!page)
			continue;

		/* Clear accessed and referenced bits. */
		ptep_test_and_clear_young(vma, addr, pte);
		ClearPageReferenced(page);
	}
	pte_unmap_unlock(pte - 1, ptl);
	cond_resched();
	return 0;
}