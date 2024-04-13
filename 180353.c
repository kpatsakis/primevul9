static int special_mapping_fault(struct vm_fault *vmf)
{
	struct vm_area_struct *vma = vmf->vma;
	pgoff_t pgoff;
	struct page **pages;

	if (vma->vm_ops == &legacy_special_mapping_vmops) {
		pages = vma->vm_private_data;
	} else {
		struct vm_special_mapping *sm = vma->vm_private_data;

		if (sm->fault)
			return sm->fault(sm, vmf->vma, vmf);

		pages = sm->pages;
	}

	for (pgoff = vmf->pgoff; pgoff && *pages; ++pages)
		pgoff--;

	if (*pages) {
		struct page *page = *pages;
		get_page(page);
		vmf->page = page;
		return 0;
	}

	return VM_FAULT_SIGBUS;
}