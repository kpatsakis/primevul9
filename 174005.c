int do_huge_pmd_wp_page(struct vm_fault *vmf, pmd_t orig_pmd)
{
	struct vm_area_struct *vma = vmf->vma;
	struct page *page = NULL, *new_page;
	struct mem_cgroup *memcg;
	unsigned long haddr = vmf->address & HPAGE_PMD_MASK;
	unsigned long mmun_start;	/* For mmu_notifiers */
	unsigned long mmun_end;		/* For mmu_notifiers */
	gfp_t huge_gfp;			/* for allocation and charge */
	int ret = 0;

	vmf->ptl = pmd_lockptr(vma->vm_mm, vmf->pmd);
	VM_BUG_ON_VMA(!vma->anon_vma, vma);
	if (is_huge_zero_pmd(orig_pmd))
		goto alloc;
	spin_lock(vmf->ptl);
	if (unlikely(!pmd_same(*vmf->pmd, orig_pmd)))
		goto out_unlock;

	page = pmd_page(orig_pmd);
	VM_BUG_ON_PAGE(!PageCompound(page) || !PageHead(page), page);
	/*
	 * We can only reuse the page if nobody else maps the huge page or it's
	 * part.
	 */
	if (!trylock_page(page)) {
		get_page(page);
		spin_unlock(vmf->ptl);
		lock_page(page);
		spin_lock(vmf->ptl);
		if (unlikely(!pmd_same(*vmf->pmd, orig_pmd))) {
			unlock_page(page);
			put_page(page);
			goto out_unlock;
		}
		put_page(page);
	}
	if (reuse_swap_page(page, NULL)) {
		pmd_t entry;
		entry = pmd_mkyoung(orig_pmd);
		entry = maybe_pmd_mkwrite(pmd_mkdirty(entry), vma);
		if (pmdp_set_access_flags(vma, haddr, vmf->pmd, entry,  1))
			update_mmu_cache_pmd(vma, vmf->address, vmf->pmd);
		ret |= VM_FAULT_WRITE;
		unlock_page(page);
		goto out_unlock;
	}
	unlock_page(page);
	get_page(page);
	spin_unlock(vmf->ptl);
alloc:
	if (transparent_hugepage_enabled(vma) &&
	    !transparent_hugepage_debug_cow()) {
		huge_gfp = alloc_hugepage_direct_gfpmask(vma);
		new_page = alloc_hugepage_vma(huge_gfp, vma, haddr, HPAGE_PMD_ORDER);
	} else
		new_page = NULL;

	if (likely(new_page)) {
		prep_transhuge_page(new_page);
	} else {
		if (!page) {
			split_huge_pmd(vma, vmf->pmd, vmf->address);
			ret |= VM_FAULT_FALLBACK;
		} else {
			ret = do_huge_pmd_wp_page_fallback(vmf, orig_pmd, page);
			if (ret & VM_FAULT_OOM) {
				split_huge_pmd(vma, vmf->pmd, vmf->address);
				ret |= VM_FAULT_FALLBACK;
			}
			put_page(page);
		}
		count_vm_event(THP_FAULT_FALLBACK);
		goto out;
	}

	if (unlikely(mem_cgroup_try_charge(new_page, vma->vm_mm,
					huge_gfp, &memcg, true))) {
		put_page(new_page);
		split_huge_pmd(vma, vmf->pmd, vmf->address);
		if (page)
			put_page(page);
		ret |= VM_FAULT_FALLBACK;
		count_vm_event(THP_FAULT_FALLBACK);
		goto out;
	}

	count_vm_event(THP_FAULT_ALLOC);

	if (!page)
		clear_huge_page(new_page, vmf->address, HPAGE_PMD_NR);
	else
		copy_user_huge_page(new_page, page, haddr, vma, HPAGE_PMD_NR);
	__SetPageUptodate(new_page);

	mmun_start = haddr;
	mmun_end   = haddr + HPAGE_PMD_SIZE;
	mmu_notifier_invalidate_range_start(vma->vm_mm, mmun_start, mmun_end);

	spin_lock(vmf->ptl);
	if (page)
		put_page(page);
	if (unlikely(!pmd_same(*vmf->pmd, orig_pmd))) {
		spin_unlock(vmf->ptl);
		mem_cgroup_cancel_charge(new_page, memcg, true);
		put_page(new_page);
		goto out_mn;
	} else {
		pmd_t entry;
		entry = mk_huge_pmd(new_page, vma->vm_page_prot);
		entry = maybe_pmd_mkwrite(pmd_mkdirty(entry), vma);
		pmdp_huge_clear_flush_notify(vma, haddr, vmf->pmd);
		page_add_new_anon_rmap(new_page, vma, haddr, true);
		mem_cgroup_commit_charge(new_page, memcg, false, true);
		lru_cache_add_active_or_unevictable(new_page, vma);
		set_pmd_at(vma->vm_mm, haddr, vmf->pmd, entry);
		update_mmu_cache_pmd(vma, vmf->address, vmf->pmd);
		if (!page) {
			add_mm_counter(vma->vm_mm, MM_ANONPAGES, HPAGE_PMD_NR);
		} else {
			VM_BUG_ON_PAGE(!PageHead(page), page);
			page_remove_rmap(page, true);
			put_page(page);
		}
		ret |= VM_FAULT_WRITE;
	}
	spin_unlock(vmf->ptl);
out_mn:
	/*
	 * No need to double call mmu_notifier->invalidate_range() callback as
	 * the above pmdp_huge_clear_flush_notify() did already call it.
	 */
	mmu_notifier_invalidate_range_only_end(vma->vm_mm, mmun_start,
					       mmun_end);
out:
	return ret;
out_unlock:
	spin_unlock(vmf->ptl);
	return ret;
}