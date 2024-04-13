SYSCALL_DEFINE5(remap_file_pages, unsigned long, start, unsigned long, size,
		unsigned long, prot, unsigned long, pgoff, unsigned long, flags)
{

	struct mm_struct *mm = current->mm;
	struct vm_area_struct *vma;
	unsigned long populate = 0;
	unsigned long ret = -EINVAL;
	struct file *file;

	pr_warn_once("%s (%d) uses deprecated remap_file_pages() syscall. See Documentation/vm/remap_file_pages.txt.\n",
		     current->comm, current->pid);

	if (prot)
		return ret;
	start = start & PAGE_MASK;
	size = size & PAGE_MASK;

	if (start + size <= start)
		return ret;

	/* Does pgoff wrap? */
	if (pgoff + (size >> PAGE_SHIFT) < pgoff)
		return ret;

	if (down_write_killable(&mm->mmap_sem))
		return -EINTR;

	vma = find_vma(mm, start);

	if (!vma || !(vma->vm_flags & VM_SHARED))
		goto out;

	if (start < vma->vm_start)
		goto out;

	if (start + size > vma->vm_end) {
		struct vm_area_struct *next;

		for (next = vma->vm_next; next; next = next->vm_next) {
			/* hole between vmas ? */
			if (next->vm_start != next->vm_prev->vm_end)
				goto out;

			if (next->vm_file != vma->vm_file)
				goto out;

			if (next->vm_flags != vma->vm_flags)
				goto out;

			if (start + size <= next->vm_end)
				break;
		}

		if (!next)
			goto out;
	}

	prot |= vma->vm_flags & VM_READ ? PROT_READ : 0;
	prot |= vma->vm_flags & VM_WRITE ? PROT_WRITE : 0;
	prot |= vma->vm_flags & VM_EXEC ? PROT_EXEC : 0;

	flags &= MAP_NONBLOCK;
	flags |= MAP_SHARED | MAP_FIXED | MAP_POPULATE;
	if (vma->vm_flags & VM_LOCKED) {
		struct vm_area_struct *tmp;
		flags |= MAP_LOCKED;

		/* drop PG_Mlocked flag for over-mapped range */
		for (tmp = vma; tmp->vm_start >= start + size;
				tmp = tmp->vm_next) {
			/*
			 * Split pmd and munlock page on the border
			 * of the range.
			 */
			vma_adjust_trans_huge(tmp, start, start + size, 0);

			munlock_vma_pages_range(tmp,
					max(tmp->vm_start, start),
					min(tmp->vm_end, start + size));
		}
	}

	file = get_file(vma->vm_file);
	ret = do_mmap_pgoff(vma->vm_file, start, size,
			prot, flags, pgoff, &populate, NULL);
	fput(file);
out:
	up_write(&mm->mmap_sem);
	if (populate)
		mm_populate(ret, populate);
	if (!IS_ERR_VALUE(ret))
		ret = 0;
	return ret;
}