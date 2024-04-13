static int do_huge_pmd_wp_page_fallback(struct vm_fault *vmf, pmd_t orig_pmd,
		struct page *page)
{
	struct vm_area_struct *vma = vmf->vma;
	unsigned long haddr = vmf->address & HPAGE_PMD_MASK;
	struct mem_cgroup *memcg;
	pgtable_t pgtable;
	pmd_t _pmd;
	int ret = 0, i;
	struct page **pages;
	unsigned long mmun_start;	/* For mmu_notifiers */
	unsigned long mmun_end;		/* For mmu_notifiers */

	pages = kmalloc(sizeof(struct page *) * HPAGE_PMD_NR,
			GFP_KERNEL);
	if (unlikely(!pages)) {
		ret |= VM_FAULT_OOM;
		goto out;
	}

	for (i = 0; i < HPAGE_PMD_NR; i++) {
		pages[i] = alloc_page_vma_node(GFP_HIGHUSER_MOVABLE, vma,
					       vmf->address, page_to_nid(page));
		if (unlikely(!pages[i] ||
			     mem_cgroup_try_charge(pages[i], vma->vm_mm,
				     GFP_KERNEL, &memcg, false))) {
			if (pages[i])
				put_page(pages[i]);
			while (--i >= 0) {
				memcg = (void *)page_private(pages[i]);
				set_page_private(pages[i], 0);
				mem_cgroup_cancel_charge(pages[i], memcg,
						false);
				put_page(pages[i]);
			}
			kfree(pages);
			ret |= VM_FAULT_OOM;
			goto out;
		}
		set_page_private(pages[i], (unsigned long)memcg);
	}

	for (i = 0; i < HPAGE_PMD_NR; i++) {
		copy_user_highpage(pages[i], page + i,
				   haddr + PAGE_SIZE * i, vma);
		__SetPageUptodate(pages[i]);
		cond_resched();
	}

	mmun_start = haddr;
	mmun_end   = haddr + HPAGE_PMD_SIZE;
	mmu_notifier_invalidate_range_start(vma->vm_mm, mmun_start, mmun_end);

	vmf->ptl = pmd_lock(vma->vm_mm, vmf->pmd);
	if (unlikely(!pmd_same(*vmf->pmd, orig_pmd)))
		goto out_free_pages;
	VM_BUG_ON_PAGE(!PageHead(page), page);

	/*
	 * Leave pmd empty until pte is filled note we must notify here as
	 * concurrent CPU thread might write to new page before the call to
	 * mmu_notifier_invalidate_range_end() happens which can lead to a
	 * device seeing memory write in different order than CPU.
	 *
	 * See Documentation/vm/mmu_notifier.txt
	 */
	pmdp_huge_clear_flush_notify(vma, haddr, vmf->pmd);

	pgtable = pgtable_trans_huge_withdraw(vma->vm_mm, vmf->pmd);
	pmd_populate(vma->vm_mm, &_pmd, pgtable);

	for (i = 0; i < HPAGE_PMD_NR; i++, haddr += PAGE_SIZE) {
		pte_t entry;
		entry = mk_pte(pages[i], vma->vm_page_prot);
		entry = maybe_mkwrite(pte_mkdirty(entry), vma);
		memcg = (void *)page_private(pages[i]);
		set_page_private(pages[i], 0);
		page_add_new_anon_rmap(pages[i], vmf->vma, haddr, false);
		mem_cgroup_commit_charge(pages[i], memcg, false, false);
		lru_cache_add_active_or_unevictable(pages[i], vma);
		vmf->pte = pte_offset_map(&_pmd, haddr);
		VM_BUG_ON(!pte_none(*vmf->pte));
		set_pte_at(vma->vm_mm, haddr, vmf->pte, entry);
		pte_unmap(vmf->pte);
	}
	kfree(pages);

	smp_wmb(); /* make pte visible before pmd */
	pmd_populate(vma->vm_mm, vmf->pmd, pgtable);
	page_remove_rmap(page, true);
	spin_unlock(vmf->ptl);

	/*
	 * No need to double call mmu_notifier->invalidate_range() callback as
	 * the above pmdp_huge_clear_flush_notify() did already call it.
	 */
	mmu_notifier_invalidate_range_only_end(vma->vm_mm, mmun_start,
						mmun_end);

	ret |= VM_FAULT_WRITE;
	put_page(page);

out:
	return ret;

out_free_pages:
	spin_unlock(vmf->ptl);
	mmu_notifier_invalidate_range_end(vma->vm_mm, mmun_start, mmun_end);
	for (i = 0; i < HPAGE_PMD_NR; i++) {
		memcg = (void *)page_private(pages[i]);
		set_page_private(pages[i], 0);
		mem_cgroup_cancel_charge(pages[i], memcg, false);
		put_page(pages[i]);
	}
	kfree(pages);
	goto out;
}