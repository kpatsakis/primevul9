static int msr_to_offset(u32 msr)
{
	unsigned bank = __this_cpu_read(injectm.bank);

	if (msr == mca_cfg.rip_msr)
		return offsetof(struct mce, ip);
	if (msr == msr_ops.status(bank))
		return offsetof(struct mce, status);
	if (msr == msr_ops.addr(bank))
		return offsetof(struct mce, addr);
	if (msr == msr_ops.misc(bank))
		return offsetof(struct mce, misc);
	if (msr == MSR_IA32_MCG_STATUS)
		return offsetof(struct mce, mcgstatus);
	return -1;
}