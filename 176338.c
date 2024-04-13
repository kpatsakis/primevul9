int __mm_populate(unsigned long start, unsigned long len, int ignore_errors)
{
	struct mm_struct *mm = current->mm;
	unsigned long end, nstart, nend;
	struct vm_area_struct *vma = NULL;
	int locked = 0;
	long ret = 0;

	end = start + len;

	for (nstart = start; nstart < end; nstart = nend) {
		/*
		 * We want to fault in pages for [nstart; end) address range.
		 * Find first corresponding VMA.
		 */
		if (!locked) {
			locked = 1;
			down_read(&mm->mmap_sem);
			vma = find_vma(mm, nstart);
		} else if (nstart >= vma->vm_end)
			vma = vma->vm_next;
		if (!vma || vma->vm_start >= end)
			break;
		/*
		 * Set [nstart; nend) to intersection of desired address
		 * range with the first VMA. Also, skip undesirable VMA types.
		 */
		nend = min(end, vma->vm_end);
		if (vma->vm_flags & (VM_IO | VM_PFNMAP))
			continue;
		if (nstart < vma->vm_start)
			nstart = vma->vm_start;
		/*
		 * Now fault in a range of pages. populate_vma_page_range()
		 * double checks the vma flags, so that it won't mlock pages
		 * if the vma was already munlocked.
		 */
		ret = populate_vma_page_range(vma, nstart, nend, &locked);
		if (ret < 0) {
			if (ignore_errors) {
				ret = 0;
				continue;	/* continue at next VMA */
			}
			break;
		}
		nend = nstart + ret * PAGE_SIZE;
		ret = 0;
	}
	if (locked)
		up_read(&mm->mmap_sem);
	return ret;	/* 0 or negative error code */
}