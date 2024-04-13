static int map_vdso(const struct vdso_image *image, bool calculate_addr)
{
	struct mm_struct *mm = current->mm;
	struct vm_area_struct *vma;
	unsigned long addr, text_start;
	int ret = 0;
	static struct page *no_pages[] = {NULL};
	static struct vm_special_mapping vvar_mapping = {
		.name = "[vvar]",
		.pages = no_pages,
	};

	if (calculate_addr) {
		addr = vdso_addr(current->mm->start_stack,
				 image->size - image->sym_vvar_start);
	} else {
		addr = 0;
	}

	down_write(&mm->mmap_sem);

	addr = get_unmapped_area(NULL, addr,
				 image->size - image->sym_vvar_start, 0, 0);
	if (IS_ERR_VALUE(addr)) {
		ret = addr;
		goto up_fail;
	}

	text_start = addr - image->sym_vvar_start;
	current->mm->context.vdso = (void __user *)text_start;

	/*
	 * MAYWRITE to allow gdb to COW and set breakpoints
	 */
	vma = _install_special_mapping(mm,
				       text_start,
				       image->size,
				       VM_READ|VM_EXEC|
				       VM_MAYREAD|VM_MAYWRITE|VM_MAYEXEC,
				       &image->text_mapping);

	if (IS_ERR(vma)) {
		ret = PTR_ERR(vma);
		goto up_fail;
	}

	vma = _install_special_mapping(mm,
				       addr,
				       -image->sym_vvar_start,
				       VM_READ|VM_MAYREAD,
				       &vvar_mapping);

	if (IS_ERR(vma)) {
		ret = PTR_ERR(vma);
		goto up_fail;
	}

	if (image->sym_vvar_page)
		ret = remap_pfn_range(vma,
				      text_start + image->sym_vvar_page,
				      __pa_symbol(&__vvar_page) >> PAGE_SHIFT,
				      PAGE_SIZE,
				      PAGE_READONLY);

	if (ret)
		goto up_fail;

#ifdef CONFIG_HPET_TIMER
	if (hpet_address && image->sym_hpet_page) {
		ret = io_remap_pfn_range(vma,
			text_start + image->sym_hpet_page,
			hpet_address >> PAGE_SHIFT,
			PAGE_SIZE,
			pgprot_noncached(PAGE_READONLY));

		if (ret)
			goto up_fail;
	}
#endif

up_fail:
	if (ret)
		current->mm->context.vdso = NULL;

	up_write(&mm->mmap_sem);
	return ret;
}