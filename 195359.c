bad_area_nosemaphore(struct pt_regs *regs, unsigned long error_code,
		     unsigned long address, u32 *pkey)
{
	__bad_area_nosemaphore(regs, error_code, address, pkey, SEGV_MAPERR);
}