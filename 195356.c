bad_area(struct pt_regs *regs, unsigned long error_code, unsigned long address)
{
	__bad_area(regs, error_code, address, NULL, SEGV_MAPERR);
}