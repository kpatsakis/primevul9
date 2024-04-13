int vmf_insert_pfn_pmd(struct vm_area_struct *vma, unsigned long addr,
			pmd_t *pmd, pfn_t pfn, bool write)
{
	pgprot_t pgprot = vma->vm_page_prot;
	pgtable_t pgtable = NULL;
	/*
	 * If we had pmd_special, we could avoid all these restrictions,
	 * but we need to be consistent with PTEs and architectures that
	 * can't support a 'special' bit.
	 */
	BUG_ON(!(vma->vm_flags & (VM_PFNMAP|VM_MIXEDMAP)));
	BUG_ON((vma->vm_flags & (VM_PFNMAP|VM_MIXEDMAP)) ==
						(VM_PFNMAP|VM_MIXEDMAP));
	BUG_ON((vma->vm_flags & VM_PFNMAP) && is_cow_mapping(vma->vm_flags));
	BUG_ON(!pfn_t_devmap(pfn));

	if (addr < vma->vm_start || addr >= vma->vm_end)
		return VM_FAULT_SIGBUS;

	if (arch_needs_pgtable_deposit()) {
		pgtable = pte_alloc_one(vma->vm_mm, addr);
		if (!pgtable)
			return VM_FAULT_OOM;
	}

	track_pfn_insert(vma, &pgprot, pfn);

	insert_pfn_pmd(vma, addr, pmd, pfn, pgprot, write, pgtable);
	return VM_FAULT_NOPAGE;
}