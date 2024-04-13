static int smaps_pte_range(pmd_t *pmd, unsigned long addr, unsigned long end,
			   void *private)
{
	struct mem_size_stats *mss = private;
	struct vm_area_struct *vma = mss->vma;
	pte_t *pte, ptent;
	spinlock_t *ptl;
	struct page *page;
	int mapcount;

	pte = pte_offset_map_lock(vma->vm_mm, pmd, addr, &ptl);
	for (; addr != end; pte++, addr += PAGE_SIZE) {
		ptent = *pte;
		if (!pte_present(ptent))
			continue;

		mss->resident += PAGE_SIZE;

		page = vm_normal_page(vma, addr, ptent);
		if (!page)
			continue;

		/* Accumulate the size in pages that have been accessed. */
		if (pte_young(ptent) || PageReferenced(page))
			mss->referenced += PAGE_SIZE;
		mapcount = page_mapcount(page);
		if (mapcount >= 2) {
			if (pte_dirty(ptent))
				mss->shared_dirty += PAGE_SIZE;
			else
				mss->shared_clean += PAGE_SIZE;
			mss->pss += (PAGE_SIZE << PSS_SHIFT) / mapcount;
		} else {
			if (pte_dirty(ptent))
				mss->private_dirty += PAGE_SIZE;
			else
				mss->private_clean += PAGE_SIZE;
			mss->pss += (PAGE_SIZE << PSS_SHIFT);
		}
	}
	pte_unmap_unlock(pte - 1, ptl);
	cond_resched();
	return 0;
}