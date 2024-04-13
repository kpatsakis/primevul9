int fixup_user_fault(struct task_struct *tsk, struct mm_struct *mm,
		     unsigned long address, unsigned int fault_flags,
		     bool *unlocked)
{
	struct vm_area_struct *vma;
	vm_fault_t ret, major = 0;

	address = untagged_addr(address);

	if (unlocked)
		fault_flags |= FAULT_FLAG_ALLOW_RETRY | FAULT_FLAG_KILLABLE;

retry:
	vma = find_extend_vma(mm, address);
	if (!vma || address < vma->vm_start)
		return -EFAULT;

	if (!vma_permits_fault(vma, fault_flags))
		return -EFAULT;

	if ((fault_flags & FAULT_FLAG_KILLABLE) &&
	    fatal_signal_pending(current))
		return -EINTR;

	ret = handle_mm_fault(vma, address, fault_flags);
	major |= ret & VM_FAULT_MAJOR;
	if (ret & VM_FAULT_ERROR) {
		int err = vm_fault_to_errno(ret, 0);

		if (err)
			return err;
		BUG();
	}

	if (ret & VM_FAULT_RETRY) {
		down_read(&mm->mmap_sem);
		*unlocked = true;
		fault_flags |= FAULT_FLAG_TRIED;
		goto retry;
	}

	if (tsk) {
		if (major)
			tsk->maj_flt++;
		else
			tsk->min_flt++;
	}
	return 0;
}