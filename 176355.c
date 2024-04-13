static long __get_user_pages_locked(struct task_struct *tsk,
		struct mm_struct *mm, unsigned long start,
		unsigned long nr_pages, struct page **pages,
		struct vm_area_struct **vmas, int *locked,
		unsigned int foll_flags)
{
	struct vm_area_struct *vma;
	unsigned long vm_flags;
	int i;

	/* calculate required read or write permissions.
	 * If FOLL_FORCE is set, we only require the "MAY" flags.
	 */
	vm_flags  = (foll_flags & FOLL_WRITE) ?
			(VM_WRITE | VM_MAYWRITE) : (VM_READ | VM_MAYREAD);
	vm_flags &= (foll_flags & FOLL_FORCE) ?
			(VM_MAYREAD | VM_MAYWRITE) : (VM_READ | VM_WRITE);

	for (i = 0; i < nr_pages; i++) {
		vma = find_vma(mm, start);
		if (!vma)
			goto finish_or_fault;

		/* protect what we can, including chardevs */
		if ((vma->vm_flags & (VM_IO | VM_PFNMAP)) ||
		    !(vm_flags & vma->vm_flags))
			goto finish_or_fault;

		if (pages) {
			pages[i] = virt_to_page(start);
			if (pages[i])
				get_page(pages[i]);
		}
		if (vmas)
			vmas[i] = vma;
		start = (start + PAGE_SIZE) & PAGE_MASK;
	}

	return i;

finish_or_fault:
	return i ? : -EFAULT;
}