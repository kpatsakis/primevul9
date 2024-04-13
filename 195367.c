access_error(unsigned long error_code, struct vm_area_struct *vma)
{
	/* This is only called for the current mm, so: */
	bool foreign = false;

	/*
	 * Read or write was blocked by protection keys.  This is
	 * always an unconditional error and can never result in
	 * a follow-up action to resolve the fault, like a COW.
	 */
	if (error_code & X86_PF_PK)
		return 1;

	/*
	 * Make sure to check the VMA so that we do not perform
	 * faults just to hit a X86_PF_PK as soon as we fill in a
	 * page.
	 */
	if (!arch_vma_access_permitted(vma, (error_code & X86_PF_WRITE),
				       (error_code & X86_PF_INSTR), foreign))
		return 1;

	if (error_code & X86_PF_WRITE) {
		/* write, present and write, not present: */
		if (unlikely(!(vma->vm_flags & VM_WRITE)))
			return 1;
		return 0;
	}

	/* read, present: */
	if (unlikely(error_code & X86_PF_PROT))
		return 1;

	/* read, not present: */
	if (unlikely(!(vma->vm_flags & (VM_READ | VM_EXEC | VM_WRITE))))
		return 1;

	return 0;
}