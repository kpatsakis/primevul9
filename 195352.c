__bad_area(struct pt_regs *regs, unsigned long error_code,
	   unsigned long address,  struct vm_area_struct *vma, int si_code)
{
	struct mm_struct *mm = current->mm;
	u32 pkey;

	if (vma)
		pkey = vma_pkey(vma);

	/*
	 * Something tried to access memory that isn't in our memory map..
	 * Fix it, but check if it's kernel or user first..
	 */
	up_read(&mm->mmap_sem);

	__bad_area_nosemaphore(regs, error_code, address,
			       (vma) ? &pkey : NULL, si_code);
}