SYSCALL_DEFINE1(shmdt, char __user *, shmaddr)
{
	struct mm_struct *mm = current->mm;
	struct vm_area_struct *vma;
	unsigned long addr = (unsigned long)shmaddr;
	int retval = -EINVAL;
#ifdef CONFIG_MMU
	loff_t size = 0;
	struct file *file;
	struct vm_area_struct *next;
#endif

	if (addr & ~PAGE_MASK)
		return retval;

	if (down_write_killable(&mm->mmap_sem))
		return -EINTR;

	/*
	 * This function tries to be smart and unmap shm segments that
	 * were modified by partial mlock or munmap calls:
	 * - It first determines the size of the shm segment that should be
	 *   unmapped: It searches for a vma that is backed by shm and that
	 *   started at address shmaddr. It records it's size and then unmaps
	 *   it.
	 * - Then it unmaps all shm vmas that started at shmaddr and that
	 *   are within the initially determined size and that are from the
	 *   same shm segment from which we determined the size.
	 * Errors from do_munmap are ignored: the function only fails if
	 * it's called with invalid parameters or if it's called to unmap
	 * a part of a vma. Both calls in this function are for full vmas,
	 * the parameters are directly copied from the vma itself and always
	 * valid - therefore do_munmap cannot fail. (famous last words?)
	 */
	/*
	 * If it had been mremap()'d, the starting address would not
	 * match the usual checks anyway. So assume all vma's are
	 * above the starting address given.
	 */
	vma = find_vma(mm, addr);

#ifdef CONFIG_MMU
	while (vma) {
		next = vma->vm_next;

		/*
		 * Check if the starting address would match, i.e. it's
		 * a fragment created by mprotect() and/or munmap(), or it
		 * otherwise it starts at this address with no hassles.
		 */
		if ((vma->vm_ops == &shm_vm_ops) &&
			(vma->vm_start - addr)/PAGE_SIZE == vma->vm_pgoff) {

			/*
			 * Record the file of the shm segment being
			 * unmapped.  With mremap(), someone could place
			 * page from another segment but with equal offsets
			 * in the range we are unmapping.
			 */
			file = vma->vm_file;
			size = i_size_read(file_inode(vma->vm_file));
			do_munmap(mm, vma->vm_start, vma->vm_end - vma->vm_start, NULL);
			/*
			 * We discovered the size of the shm segment, so
			 * break out of here and fall through to the next
			 * loop that uses the size information to stop
			 * searching for matching vma's.
			 */
			retval = 0;
			vma = next;
			break;
		}
		vma = next;
	}

	/*
	 * We need look no further than the maximum address a fragment
	 * could possibly have landed at. Also cast things to loff_t to
	 * prevent overflows and make comparisons vs. equal-width types.
	 */
	size = PAGE_ALIGN(size);
	while (vma && (loff_t)(vma->vm_end - addr) <= size) {
		next = vma->vm_next;

		/* finding a matching vma now does not alter retval */
		if ((vma->vm_ops == &shm_vm_ops) &&
		    ((vma->vm_start - addr)/PAGE_SIZE == vma->vm_pgoff) &&
		    (vma->vm_file == file))
			do_munmap(mm, vma->vm_start, vma->vm_end - vma->vm_start, NULL);
		vma = next;
	}

#else	/* CONFIG_MMU */
	/* under NOMMU conditions, the exact address to be destroyed must be
	 * given
	 */
	if (vma && vma->vm_start == addr && vma->vm_ops == &shm_vm_ops) {
		do_munmap(mm, vma->vm_start, vma->vm_end - vma->vm_start, NULL);
		retval = 0;
	}

#endif

	up_write(&mm->mmap_sem);
	return retval;
}