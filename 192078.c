static inline void mce_gather_info(struct mce *m, struct pt_regs *regs)
{
	mce_setup(m);

	m->mcgstatus = mce_rdmsrl(MSR_IA32_MCG_STATUS);
	if (regs) {
		/*
		 * Get the address of the instruction at the time of
		 * the machine check error.
		 */
		if (m->mcgstatus & (MCG_STATUS_RIPV|MCG_STATUS_EIPV)) {
			m->ip = regs->ip;
			m->cs = regs->cs;

			/*
			 * When in VM86 mode make the cs look like ring 3
			 * always. This is a lie, but it's better than passing
			 * the additional vm86 bit around everywhere.
			 */
			if (v8086_mode(regs))
				m->cs |= 3;
		}
		/* Use accurate RIP reporting if available. */
		if (mca_cfg.rip_msr)
			m->ip = mce_rdmsrl(mca_cfg.rip_msr);
	}
}