static int spurious_fault_check(unsigned long error_code, pte_t *pte)
{
	if ((error_code & X86_PF_WRITE) && !pte_write(*pte))
		return 0;

	if ((error_code & X86_PF_INSTR) && !pte_exec(*pte))
		return 0;
	/*
	 * Note: We do not do lazy flushing on protection key
	 * changes, so no spurious fault will ever set X86_PF_PK.
	 */
	if ((error_code & X86_PF_PK))
		return 1;

	return 1;
}