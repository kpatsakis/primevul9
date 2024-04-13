bad_area_access_error(struct pt_regs *regs, unsigned long error_code,
		      unsigned long address, struct vm_area_struct *vma)
{
	/*
	 * This OSPKE check is not strictly necessary at runtime.
	 * But, doing it this way allows compiler optimizations
	 * if pkeys are compiled out.
	 */
	if (bad_area_access_from_pkeys(error_code, vma))
		__bad_area(regs, error_code, address, vma, SEGV_PKUERR);
	else
		__bad_area(regs, error_code, address, vma, SEGV_ACCERR);
}